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
#include "msgrange.h"

bool SignalBackup::HTMLprepMsgBody(std::string *body, std::vector<std::tuple<long long int, long long int, long long int>> const &mentions,
                                   std::map<long long int, RecipientInfo> *recipient_info, bool incoming,
                                   std::pair<std::shared_ptr<unsigned char []>, size_t> const &brdata, bool isquote) const
{
  if (body->empty())
    return false;

  std::vector<Range> ranges;

  // First, do mentions
  for (auto const &m : mentions)
  {
    // m1 : uuid, m2: start, m3: length
    std::string author = getRecipientInfoFromMap(recipient_info, std::get<0>(m)).display_name;
    if (!author.empty())
    {
      ranges.emplace_back(Range{std::get<1>(m), std::get<2>(m),
                                (isquote ? "" : "<span class=\"mention-"s + (incoming ? "in" : "out") + "\">"),
                                "@" + author,
                                (isquote ? "" : "</span>"),
                                true});
    }
  }

  // now do other stylings?
  if (brdata.second)
  {
    BodyRanges brsproto(brdata);
    //brsproto.print();

    auto brs = brsproto.getField<1>();
    for (auto const &br : brs)
    {
      int start = br.getField<1>().value_or(0);
      int length = br.getField<2>().value_or(0);
      if (!length) // maybe legal? no length == rest of string? (like no start is beg)
        continue;

      // get mention
      std::string mentionuuid = br.getField<3>().value_or(std::string());
      if (!mentionuuid.empty())
      {
        long long int authorid = getRecipientIdFromUuid(mentionuuid, nullptr);
        std::string author = getRecipientInfoFromMap(recipient_info, authorid).display_name;
        if (!author.empty())
          ranges.emplace_back(Range{start, length,
                                    (isquote ? "" : "<span class=\"mention-"s + (incoming ? "in" : "out") + "\">"),
                                    "@" + author,
                                    (isquote ? "" : "</span>"),
                                    true});
      }

      // get style
      int style = br.getField<4>().value_or(-1);

      // get link
      std::string link = br.getField<5>().value_or(std::string());

      if (style > -1)
      {
        //std::cout << "Adding style to range [" << start << "-" << start+length << "] : " << style << std::endl;
        switch (style)
        {
          case 0: // BOLD
          {
            ranges.emplace_back(Range{start, length, "<b>", "", "</b>", false});
            break;
          }
          case 1: // ITALIC
          {
            ranges.emplace_back(Range{start, length, "<i>", "", "</i>", false});
            break;
          }
          case 2: // SPOILER
          {
            ranges.emplace_back(Range{start, length, "<span class=\"spoiler\">", "", "</span>", true});
            break;
          }
          case 3: // STRIKETHROUGH
          {
            ranges.emplace_back(Range{start, length, "<s>", "", "</s>", false}); // or <del>? or <span class="strikthrough">?
            break;
          }
          case 4: // MONOSPACE
          {
            ranges.emplace_back(Range{start, length, "<span class=\"monospace\">", "", "</span>", false});
            break;
          }
          default:
          {
            std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off
                      << ": Unsupported range-style: " << style << std::endl;
          }
        }
      }
      if (!link.empty())
      {
        //std::cout << "Adding link to range [" << start << "-" << start+length << link << "'" << std::endl;
        //std::string bodydouble = *body;
        //std::cout << *body << std::endl;
        //applyRange(body, start, length, "<a href=\"" + link + "\">", "", "</a>", -1, &positions_added, &positions_excluded_from_escape);
        ranges.emplace_back(Range{start, length, "<a class=\"styled-link\" href=\"" + link + "\">", "", "</a>", true});
        //std::cout << *body << std::endl;
        //for (auto n : positions_excluded_from_escape)
        //  std::cout << n << std::endl;
        //*body = bodydouble;
      }
      // if (!mentionuuid.empty())
      //   std::cout << "Adding mention to range [" << start << "-" << start+length << "] : '" << mentionuuid << "'" << std::endl;
    }
  }

  std::set<int> positions_excluded_from_escape;
  applyRanges(body, &ranges, &positions_excluded_from_escape);

  HTMLescapeString(body, &positions_excluded_from_escape);

  // now do the emoji
  std::vector<std::pair<unsigned int, unsigned int>> pos = HTMLgetEmojiPos(*body);

  // check if body is only emoji
  bool all_emoji = true;
  if (pos.size() > 5)
    all_emoji = false; // could technically still be only emoji, but it gets a bubble in html
  else
  {
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
  }

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
