/*
  Copyright (C) 2024  Selwin van Dijk

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

#include "msgrange.h"

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

  pos = 0;
  std::smatch url_match_result;
  std::string bodycopy(body);
  while (std::regex_search(bodycopy, url_match_result, s_linkify_pattern))
  {
    //std::cout << "MATCH : " << url_match_result[0] << " (" << url_match_result.size() << " matches total)"
    //          << " : " << pos + url_match_result.position(0) << " " << url_match_result.length(0) << std::endl;
    // for (const auto &res : url_match_result)
    //   std::cout << (res.str().empty() ? "0" : "1");
    // std::cout << std::endl;

    // get offset+length if string was utf16
    long long int match_start = 0;
    for (unsigned int i = 0; i < pos + url_match_result.position(0); )
    {
      int utf8size = bytesToUtf8CharSize(body, i);
      match_start += utf16CharSize(body, i);
      i += utf8size;
    }
    //std::cout << "startpos : " << match_start << std::endl;

    long long int match_length = 0;
    for (unsigned int i = pos + url_match_result.position(0); i < pos + url_match_result.position(0) + url_match_result.length(0); )
    {
      int utf8size = bytesToUtf8CharSize(body, i);
      match_length += utf16CharSize(body, i);
      i += utf8size;
    }
    //std::cout << "match length : " << match_length << std::endl;

    // url_match_result.length(2) > 0 -> EMAIL
    // url_match_result.length(3) > 0 -> URL_WITH_PROTOCOL
    // url_match_result.length(9/10) > 0 -> URL_NO_PROTOCOL

    std::string match_link(url_match_result.str(0));
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
      }
    }

    if (url_match_result.length(3) > 0) // -> URL_WITH_PROTOCOL
      ranges->emplace_back(Range{match_start, //static_cast<long long int>(pos) + url_match_result.position(0),
                                 match_length, //url_match_result.length(0),
                                 "<a class=\"unstyled-link\" href=\"" + match_link + "\">",
                                 "",
                                 "</a>",
                                 true});
    else if (url_match_result.length(9) > 0) // -> URL_WITHOUT_PROTOCOL
      /* Here, we add the protocol manually (guessing it to be https),
         without a protocol, the 'link' will be interpreted as a location
         in the current domain (file://HTMLDIR/Conversation/Page.html/www.example.com),
         a workaround, using just "//" as the protocol does signal that the link
         is at a new root, but automatically uses the current protocol (which is file://,
         which is not correct.
      */
      ranges->emplace_back(Range{match_start, //static_cast<long long int>(pos) + url_match_result.position(0),
                                 match_length, //url_match_result.length(0),
                                 "<a class=\"unstyled-link\" href=\"https://" + match_link + "\">",
                                 "",
                                 "</a>",
                                 true});
    else if (url_match_result.length(2) > 0) // -> EMAIL
      ranges->emplace_back(Range{match_start, //static_cast<long long int>(pos) + url_match_result.position(0),
                                 match_length, //url_match_result.length(0),
                                 "<a class=\"unstyled-link\" href=\"mailto:" + match_link + "\">",
                                 "",
                                 "</a>",
                                 true});


    pos += url_match_result.position(0) + url_match_result.length(0);
    bodycopy = url_match_result.suffix();
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
