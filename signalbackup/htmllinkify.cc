/*
  Copyright (C) 2024-2025  Selwin van Dijk

  This file is part of signalbackup-tools.

  signalbackup-tools is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  signalbackup-tools is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with signalbackup-tools.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "signalbackup.ih"

#include "linkify_pattern.h"
#include "msgrange.h"

#include "../common_regex.h"

void SignalBackup::HTMLLinkify(std::string const &body, std::vector<Range> *ranges) const
{
  bool possible_link = false;
  long long unsigned int pos = 0;
  while ((pos = body.find('.', pos)) != std::string::npos)
  {
    ++pos;
    if (pos < body.size() &&
        //((body[pos] >= 'A' && body[pos] <= 'Z') || // this is slightly faster, but likely prevents
        //(body[pos] >= 'a' && body[pos] <= 'z')))   // some urls from linkifying (with unicode
        body[pos] >= 'A')                            // char in TLD)
    {
      possible_link = true;
      break;
    }
  }

  if (!possible_link) [[likely]]
    return;

  if (d_verbose) [[unlikely]]
    Logger::message("Searching for possible URL in message body");

  pos = 0;
  REGEX_SMATCH_RESULTS url_match_result;
  std::string bodycopy(body);
  std::string &bodycopy_reference(bodycopy);
  while (REGEX_SEARCH(bodycopy_reference, url_match_result, HTMLLinkify::pattern))
  {
    // std::cout << "MATCH : " << url_match_result[0] << " (" << url_match_result.size() << " matches total)"
    //           << " : " << pos + url_match_result.position(0) << " " << url_match_result.length(0) << std::endl;
    // for (const auto &res : url_match_result)
    //   std::cout << " : " << res.str() << std::endl;
    // std::cout << std::endl;

    // url_match_result.length(1) > 0 -> EMAIL
    // url_match_result.length(2) > 0 -> URL_WITH_PROTOCOL
    // url_match_result.length(3) > 0 -> URL_NO_PROTOCOL
    unsigned int match_index = 0;
    if (url_match_result.length(URL_WITH_PROTOCOL_MATCH) > 0)
      match_index = URL_WITH_PROTOCOL_MATCH;
    else if (url_match_result.length(URL_WITHOUT_PROTOCOL_MATCH) > 0)
      match_index = URL_WITHOUT_PROTOCOL_MATCH;
    else if (url_match_result.length(EMAIL_MATCH) > 0)
      match_index = EMAIL_MATCH;
    else [[unlikely]]
    {
      Logger::warning("Unexpected match result while linkifying message body. Skipping.");
      return;
    }

    // get offset+length if string was utf16
    long long int match_start = 0;
    for (unsigned int i = 0; i < pos + url_match_result.position(match_index); )
    {
      int utf8size = bytesToUtf8CharSize(body, i);
      match_start += utf16CharSize(body, i);
      i += utf8size;
    }
    //std::cout << "startpos : " << match_start << std::endl;

    long long int match_length = 0;
    for (unsigned int i = pos + url_match_result.position(match_index); i < pos + url_match_result.position(match_index) + url_match_result.length(match_index); )
    {
      int utf8size = bytesToUtf8CharSize(body, i);
      match_length += utf16CharSize(body, i);
      i += utf8size;
    }
    //std::cout << "match length : " << match_length << std::endl;


    std::string match_link(url_match_result.str(match_index));
    /*
      This really shouldn't happen I think, but I have a link with multiple # signs
      in my backup. This is not valid, and causes the HTML to not be valid, so
      we escape it.
      Other such issues may also appear in the future
    */
    size_t escapepos = 0;
    if ((escapepos = match_link.find('#')) != std::string::npos) [[unlikely]]
    {
      size_t start_pos = escapepos;
      while ((start_pos = match_link.find('#', start_pos + 1)) != std::string::npos)
      {
        match_link.replace(start_pos, 1, "%23");
        start_pos += STRLEN("%23");
        match_length += 2; // (len("%23") - len("#"))
      }
    }

    // let's assume the url does not end in some characters
    while (match_link.back() == ':' ||
           match_link.back() == '.' ||
           match_link.back() == ',')
    {
      match_link.pop_back();
      --match_length;
    }

    // special case for url with trailing ')'. Lets assume it does not belong to
    // the url if it does not contain a unclosed '('
    if (match_link.back() == ')') [[unlikely]]
    {
      int openbrackets = 0;
      bool belongs_in_url = false;
      for (unsigned int ri = match_link.size() - 1; ri--; )
      {
        if (match_link[ri] == '(')
          ++openbrackets;
        else if (match_link[ri] == ')')
          --openbrackets;

        if (openbrackets == 1)
        {
          belongs_in_url = true;
          break;
        }
      }

      if (!belongs_in_url)
      {
        match_link.pop_back();
        --match_length;
      }
    }

    if (match_index == URL_WITH_PROTOCOL_MATCH) // -> URL_WITH_PROTOCOL
      ranges->emplace_back(match_start, //static_cast<long long int>(pos) + url_match_result.position(0),
                           match_length, //url_match_result.length(0),
                           "<a class=\"unstyled-link\" href=\"" + match_link + "\">",
                           "",
                           "</a>",
                           true);
    else if (match_index == URL_WITHOUT_PROTOCOL_MATCH) // -> URL_WITHOUT_PROTOCOL
      /* Here, we add the protocol manually (guessing it to be https),
         without a protocol, the 'link' will be interpreted as a location
         in the current domain (file://HTMLDIR/Conversation/Page.html/www.example.com),
         a workaround, using just "//" as the protocol does signal that the link
         is at a new root, but automatically uses the current protocol (which is file://,
         which is also not correct.
      */
      ranges->emplace_back(match_start, //static_cast<long long int>(pos) + url_match_result.position(0),
                           match_length, //url_match_result.length(0),
                           "<a class=\"unstyled-link\" href=\"https://" + match_link + "\">",
                           "",
                           "</a>",
                           true);
    else if (match_index == EMAIL_MATCH) // -> EMAIL
      ranges->emplace_back(match_start, //static_cast<long long int>(pos) + url_match_result.position(0),
                           match_length, //url_match_result.length(0),
                           "<a class=\"unstyled-link\" href=\"mailto:" + match_link + "\">",
                           "",
                           "</a>",
                           true);


    pos += url_match_result.position() + url_match_result.length(0);
    bodycopy_reference = url_match_result.suffix();
  }
}



/*

     foo://example.com:8042/over/there?name=ferret#nose
     \_/   \______________/\_________/ \_________/ \__/
      |           |            |            |        |
   scheme     authority       path        query   fragment




   - The scheme may not contain a hash sign (only ALPHA *( ALPHA / DIGIT / "+" / "-" / ".")
   - The autority may not contain a hash. It is terminated by the next slash ("/"), question mark ("?"), or number sign ("#").
   - The path consists of a sequence of path segments separated by a slash ("/") character. The path segments in turn can only consist of pchars. It will also be terminated by the first question mark ("?") or number sign ("#"), or by the end of the URI.
   - The query part (indicated by the first "?") may only consist of pchar, "/" or "?" and will be terminated by a number sign ("#") character or by the end of the URI.
   - The fragment is indicated by the presence of a number sign ("#")' and also consists only of pchar, "/" or "?". It is terminated by the end of the URI.

*/
