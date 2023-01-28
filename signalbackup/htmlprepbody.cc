/*
  Copyright (C) 2023  Selwin van Dijk

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

bool SignalBackup::HTMLprepMsgBody(std::string *body, std::vector<std::tuple<long long int, long long int, long long int>> const &mentions,
                                   std::map<long long int, RecipientInfo> const &recipients_info, bool incoming, bool isquote) const
{
  if (body->empty())
    return false;

  using namespace std::string_literals;

  // std::string orig(*body);
  // bool changed = false;

  std::set<int> positions_excluded_from_escape;

  // First, do mentions
  int positions_added = 0;
  for (auto const &m : mentions)
  {
    std::string author;
    if (recipients_info.contains(std::get<0>(m)))
      author = recipients_info.at(std::get<0>(m)).display_name;
    if (!author.empty())
    {
      // NOTE: range_length is always 1, even though we need to replace all
      // 3 bytes of the replacement character \xEF\xBF\xBC. Maybe the internal
      // database is UTF16 (where this is 1 char), even though the exported backup
      // is UTF8?
      if (!isquote)
      {
        std::string span = "<span class=\"mention-"s + (incoming ? "in" : "out") + "\">";

        body->replace(std::get<1>(m) + positions_added, 3, span + "@" + author + "</span>");

        positions_excluded_from_escape.insert(std::get<1>(m) + positions_added); // first '<'
        positions_excluded_from_escape.insert(std::get<1>(m) + positions_added + STRLEN("<span class=\"") - 1); // first '"'
        positions_excluded_from_escape.insert(std::get<1>(m) + positions_added + (span.size() - 2)); // last '"'
        positions_excluded_from_escape.insert(std::get<1>(m) + positions_added + (span.size() - 1)); // first '>'
        positions_excluded_from_escape.insert(std::get<1>(m) + positions_added + (span.size() - 1) + 1 + author.length() + 1); // last '<'
        positions_excluded_from_escape.insert(std::get<1>(m) + positions_added + (span.size() - 1) + 1 + author.length() + STRLEN("</span>")); // last '>'

        positions_added += author.length() + span.size() + 1 + STRLEN("</span>") - 1; // minus 1 for the char we are replacing
      }
      else
      {
        body->replace(std::get<1>(m) + positions_added, 3, "@" + author);
        positions_added += author.length() + 1 - 1; // minus 1 for the char we are replacing
      }
      //changed = true;
    }
  }

  // escape special html chars second, so the span's added by emojifinder (next) aren't escaped
  positions_added = 0;
  for (uint i = 0; i < body->length(); ++i)
  {
    //std::cout << "I, POSITIONS_ADDED: " << i << "," << positions_added << std::endl;
    switch (body->at(i))
    {
      case '&':
        if (!positions_excluded_from_escape.contains(i - positions_added))
        {
          body->replace(i, 1, "&amp;");
          positions_added += STRLEN("&amp;") - 1;
          i += STRLEN("&amp;") - 1;
          //changed = true;
        }
        break;
      case '<':
        if (!positions_excluded_from_escape.contains(i - positions_added))
        {
          body->replace(i, 1, "&lt;");
          positions_added += STRLEN("&lt;") - 1;
          i += STRLEN("&lt;") - 1;
          //changed = true;
        }
        break;
      case '>':
        if (!positions_excluded_from_escape.contains(i - positions_added))
        {
          body->replace(i, 1, "&gt;");
          i += STRLEN("&gt;") - 1;
          positions_added += STRLEN("&gt;") - 1;
          //changed = true;
        }
        break;
      case '"':
        if (!positions_excluded_from_escape.contains(i - positions_added))
        {
          body->replace(i, 1, "&quot;");
          i += STRLEN("&quot;") - 1;
          positions_added += STRLEN("&quot;") - 1;
          //changed = true;
        }
        break;
      case '\'':
        if (!positions_excluded_from_escape.contains(i - positions_added))
        {
          body->replace(i, 1, "&apos;");
          i += STRLEN("&apos;") - 1;
          positions_added += STRLEN("&apos;") - 1;
          //changed = true;
        }
        break;
    }
  }

  // now do the emoji
  std::vector<std::pair<unsigned int, unsigned int>> pos = HTMLgetEmojiPos(*body);

  // check if body is only emoji
  bool all_emoji = true;
  for (uint i = 0, posidx = 0; i < body->size(); ++i)
  {
    if (posidx >= pos.size() || i != pos[posidx].first)
    {
      if (body->at(i) == ' ') // spaces dont count
        continue;
      all_emoji = false;
      break;
    }
    else // body[i] == pos[posidx].first
      i += pos[posidx++].second - 1; // minus 1 for the ++i in loop
  }
  //std::cout << "ALL EMOJI: " << all_emoji << std::endl;

  // surround emoji with span
  std::string pre = "<span class=\"msg-emoji\">";
  std::string post = "</span>";
  int moved = 0;
  for (auto const &p : pos)
  {
    body->insert(p.first + moved, pre);
    body->insert(p.first + p.second + pre.size() + moved, post);
    moved += pre.size() + post.size();
  }

  return all_emoji;

  // if (changed)
  // {
  //   std::cout << "ORIG: " << orig << std::endl;
  //   std::cout << "NEW : " << *body << std::endl;
  // }
}
