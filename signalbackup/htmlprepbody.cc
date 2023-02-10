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
                                   std::map<long long int, RecipientInfo> const &recipients_info, bool incoming,
                                   std::pair<std::shared_ptr<unsigned char []>, size_t> const &brdata, bool isquote) const
{
  if (body->empty())
    return false;

  using namespace std::string_literals;

  std::vector<Range> ranges;

  // First, do mentions
  for (auto const &m : mentions)
  {
    // m1 : uuid, m2: start, m3: length
    std::string author;
    if (recipients_info.contains(std::get<0>(m)))
      author = recipients_info.at(std::get<0>(m)).display_name;
    if (!author.empty())
    {
      ranges.emplace_back(Range{std::get<1>(m), std::get<2>(m),
                                (isquote ? "" : "<span class=\"mention-"s + (incoming ? "in" : "out") + "\">"),
                                "@" + author,
                                (isquote ? "" : "</span>")});
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
        std::string author;
        if (recipients_info.contains(authorid))
          author = recipients_info.at(authorid).display_name;
        if (!author.empty())
          ranges.emplace_back(Range{start, length,
                                    (isquote ? "" : "<span class=\"mention-"s + (incoming ? "in" : "out") + "\">"),
                                    "@" + author,
                                    (isquote ? "" : "</span>")});
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
            ranges.emplace_back(Range{start, length, "<b>", "", "</b>"});
            break;
          }
          case 1: // ITALIC
          {
            ranges.emplace_back(Range{start, length, "<i>", "", "</i>"});
            break;
          }
          case 2: // SPOILER
          {
            //ranges.emplace_back(Range{start, length, "<<span class=\"spoiler\">", "", "</span>"});
            /* PLUS:
               .spoiler
               {
                 color: black;
                 background-color: black;
               }

               .spoiler:hover
               {
                 background-color: white; // inherit/default?
               }
             */
            break;
          }
          case 3: // STRIKETHROUGH
          {
            ranges.emplace_back(Range{start, length, "<s>", "", "</s>"}); // or <del>? or <span class="strikthrough">?
            break;
          }
          case 4: // MONOSPACE
          {
            // <span class=\"monospace\"> </span>
            /* PLUS:
               .monospace
               {
                 font-family: 'Roboto Mono', 'others...',  monospace;
               }
             */
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
        //HTMLhandleRange(body, start, length, "<a href=\"" + link + "\">", "", "</a>", -1, &positions_added, &positions_excluded_from_escape);
        ranges.emplace_back(Range{start, length, "<a class=\"styled-link\" href=\"" + link + "\">", "", "</a>"});
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
  HTMLhandleRanges(body, &ranges, &positions_excluded_from_escape);

  HTMLescapeString(body, &positions_excluded_from_escape);

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

void SignalBackup::HTMLhandleRanges(std::string *body, std::vector<Range> *ranges, std::set<int> *positions_excluded_from_escape) const
{
  HTMLprepRanges(ranges);

  unsigned int rangesidx = 0;
  unsigned int bodyidx = 0;
  while (bodyidx < body->size() && rangesidx < ranges->size())
  {
    //std::cout << "Checking char idx: " << bodyidx << "('" << body->at(bodyidx) << "') for range with start: " << ranges->at(rangesidx).start << std::endl;

    int charsize = bytesToUtf8CharSize(body, bodyidx);
    if (bodyidx == ranges->at(rangesidx).start)
    {
      int length = bytesToUtf8CharSize(body, bodyidx, ranges->at(rangesidx).length);
      std::string pre = ranges->at(rangesidx).pre;
      std::string replacement = ranges->at(rangesidx).replacement;
      std::string post = ranges->at(rangesidx).post;

      if (replacement.empty())
        replacement = body->substr(bodyidx, length);

      body->replace(bodyidx, length, pre + replacement + post);

      for (uint i = 0; i < pre.size(); ++i)
        if (pre[i] == '<' || pre[i] == '>' || pre[i] == '\'' || pre[i] == '"' || pre[i] == '&')
          positions_excluded_from_escape->insert(i + bodyidx);
      for (uint i = 0; i < post.size(); ++i)
        if (post[i] == '<' || post[i] == '>' || post[i] == '\'' || post[i] == '"' || post[i] == '&')
          positions_excluded_from_escape->insert(i + bodyidx + pre.size() + replacement.size());

      //std::cout << "BODY: " << *body << std::endl;

      for (uint i = rangesidx + 1; i < ranges->size(); ++i)
        ranges->at(i).start += pre.size() + post.size() + (replacement.size() - ranges->at(rangesidx).length);

      // skip the just inserted string
      bodyidx += pre.size() + post.size() + replacement.size();

      // look for next range
      // while the prepwork should make sure it is the first one,
      // interactions with mention replacements might throw it off?
      // just to be sure, lets not just `++rangesidx'
      while (++rangesidx < ranges->size() && ranges->at(rangesidx).start < bodyidx)
        ;
      continue;
    }

    // update all following ranges for multibyte char
    if (charsize > 1)
      for (uint i = rangesidx; i < ranges->size(); ++i)
        ranges->at(i).start += charsize - 1;

    // next char...
    bodyidx += charsize;
  }
}

/*
  This should work for (possible) overlapping ranges with the
  depending on how html renders certain things.
*/
void SignalBackup::HTMLprepRanges(std::vector<Range> *ranges) const
{
  std::sort(ranges->begin(), ranges->end());
  // std::cout << "got range:" << std::endl;
  // for (auto const &r : *ranges)
  //   std::cout << "[" << r.start << "-" << r.start + r.length << "] '" << r.pre << "' '" << r.post << "'" << std::endl;

  /*
    After sorting, there are the following possible overlaps between range_i-1 and range_i:

    (1)
      O1: <1>      </1>
      O1: <2>      </2>
      ->
      N1: <1><2>   </2></1>

      ! NOTE only one of the can have replacement, not both *
      ! <N1> copies replacement from 1/2

    (2)
      O1: <1>      </1>
      O2: <2>               </2>
      ->
      N1: <1><2>   </2></1>
      N2:          <2>      </2>

      ! NOTE <2> cannot have replacement (<1> would fall in the middle of it) **
      ! IF <1> has replacement <N1> has replacement

    (3)
      O1: <1>          </1>
      O2:      <2>     </2>
      ->
      N1: <1>  </1>
      N2:      <1><2>  </2></1>

      ! NOTE <1> cannot have replacement (<2> would fall in the middle of it) **
      ! IF <2> has replacement <N2> has replacement

    (4)
      O1: <1>          </1>
      O2:      <2>                </2>
      ->
      N1: <1>  </1>
      N2:      <1><2>  </2></1>
      N3:              <2>        </2>

      ! NOTE neither <1> or <2> can have replacement **

    (5)
      O1: <1>                    </1>
      O2:      <2>    </2>
      ->
      N1: <1>  </1>
      N2:      <1><2>  </2></1>
      N3:              <1>       </1>

      ! NOTE <1> cannot have replacement (<2> would fall in the middle of it) **
      ! IF <2> has replacement <N2> has replacement


      *) This is hypothetical, no action can currently cause the same string location
         to be replaced by multiple substrings
      **) This is also hypothetical, all replacement-ranges (mentions) currently have
          length == 1, which makes it impossible for another range to start/end in the
          middle of it

  */

  for (uint i = 1; i < ranges->size(); ++i)
  {
    if (ranges->at(i).start == ranges->at(i - 1).start)
    {
      // combine into 1
      ranges->at(i - 1).pre += ranges->at(i).pre;
      ranges->at(i - 1).post = ranges->at(i).post + ranges->at(i - 1).post;
      if (ranges->at(i).length == ranges->at(i -1).length)
      {
        // (1) SAME START, SAME FINISH
        if (!ranges->at(i - 1).replacement.empty() &&
            !ranges->at(i).replacement.empty())
        {
          std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off
                    << ": Illegal range-set (overlapping ranges both have replacement)." << std::endl;
          continue;
        }
        if (!ranges->at(i).replacement.empty()) // only one can have replacement, if it's i-1 its already kept...
          ranges->at(i - 1).replacement = ranges->at(i).replacement;
        ranges->erase(ranges->begin() + i);
        return HTMLprepRanges(ranges);
      }
      // (2) SAME START, LATER FINISH
      // else -> i.length > (i - 1).length
      if (!ranges->at(i).replacement.empty())
      {
        std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off
                  << ": Illegal range-set (overlapping ranges both have replacement)." << std::endl;
        continue;
      }
      // ranges->at(i - 1).replacement is automatically kept if it has one
      ranges->at(i).start = ranges->at(i - 1).start + ranges->at(i - 1).length;
      ranges->at(i).length -= ranges->at(i - 1).length;
      return HTMLprepRanges(ranges);
    }
    // else (i).start > (i - 1).start)
    if (ranges->at(i).start + ranges->at(i).length == ranges->at(i - 1).start + ranges->at(i - 1).length) // same end pos
    {
      // (3) LATER START, SAME FINISH
      if (!ranges->at(i - 1).replacement.empty())
      {
        std::cout << "Warning. Illegal range-set (overlapping ranges both have replacement)." << std::endl;
        continue;
      }
      ranges->at(i - 1).length = ranges->at(i).start - ranges->at(i - 1).start;
      // ranges->at(i).replacement is automatically kept if it has one
      ranges->at(i).pre += ranges->at(i - 1).pre;
      ranges->at(i).post = ranges->at(i - 1).post + ranges->at(i).post;
      return HTMLprepRanges(ranges);
    }
    if (ranges->at(i).start < ranges->at(i - 1).start + ranges->at(i - 1).length)
    {
      if (ranges->at(i).start + ranges->at(i).length > ranges->at(i - 1).start + ranges->at(i - 1).length) // end later
      {
        // (4) LATER START, LATER FINISH"
        if (!ranges->at(i - 1).replacement.empty() ||
            !ranges->at(i).replacement.empty())
        {
          std::cout << "Warning. Illegal range-set (overlapping ranges both have replacement)." << std::endl;
          continue;
        }

        Range newrange = {ranges->at(i).start,
                          ranges->at(i - 1).length - (ranges->at(i).start - ranges->at(i - 1).start),
                          ranges->at(i - 1).pre + ranges->at(i).pre,
                          "",
                          ranges->at(i).post + ranges->at(i - 1).post};
        ranges->emplace_back(newrange);

        ranges->at(i - 1).length = newrange.start - ranges->at(i - 1).start;

        ranges->at(i).start = newrange.start + newrange.length;
        ranges->at(i).length -= newrange.length;

        return HTMLprepRanges(ranges);
      }
      //if (ranges->at(i).start + ranges->at(i).length < ranges->at(i - 1).start + ranges->at(i - 1).length) // end sooner
      // (5) LATER START, EARLIER FINISH
      if (!ranges->at(i - 1).replacement.empty())
      {
        std::cout << "Warning. Illegal range-set (overlapping ranges both have replacement)." << std::endl;
        continue;
      }
      Range newrange = {ranges->at(i).start + ranges->at(i).length,
                        ranges->at(i - 1).start + ranges->at(i - 1).length - (ranges->at(i).start + ranges->at(i).length),
                        ranges->at(i - 1).pre,
                        "",
                        ranges->at(i - 1).post};
      ranges->emplace_back(newrange);                                           // -> N3

      ranges->at(i - 1).length = ranges->at(i).start - ranges->at(i - 1).start; // O1 -> N1

      // ranges->at(i).replacement is automatically kept if it has one
      ranges->at(i).pre += ranges->at(i - 1).pre;
      ranges->at(i).post = ranges->at(i - 1).post + ranges->at(i).post;         // O2 -> N2
      return HTMLprepRanges(ranges);
    }
  }
  // std::cout << "DONE!" << std::endl;
}
