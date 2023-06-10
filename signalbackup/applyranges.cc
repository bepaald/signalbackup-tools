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

/*
  While the data in 'body' is utf8 encoded, the range (start+length) deals with utf16 (one or two 16bit bytes)

  at each point in the string (which only deals with single 8bit bytes, we need to know the number of bytes to
  skip to the next char (which is its utf8 size) as well as compensate the index as if the data were 16bit
  utf16 (possibly multibyte).

  It's a mess, and I hope it works


  eg:

  RANGE: {start: 3, length: 1, replacement: 'AA'}
  '💩 ...'
  0xF0 0x9F 0x92 0xA9 0x20 0xef 0xbf 0xbc ...
   \_______________/   |     \_______/
        emoji        space     placeholder

  idx0: '0xF0' => utf8 size == 4 => idx0->idx4
               => utf16 size == 2 => range{start: 3 + (4 - 2)}
  idx4: '0x20' => utf8 size == 1 => idx4->idx5
               => utf16 size == 1 => range{start: 5 + (1 - 1)}
  idx5: MATCH! adjust length at this position from utf16 codepoints to 8bit bytes and replace
               => utf16 length 1 at idx 5 == 3 => replace 3 with 'AA'
               => 0xF0 0x9F 0x92 0xA9 0x20 0x41 0x41 ...
               => idx5->idx7 (5 +
 */

void SignalBackup::applyRanges(std::string *body, std::vector<Range> *ranges, std::set<int> *positions_excluded_from_escape) const
{
  prepRanges2(ranges);

  unsigned int rangesidx = 0;
  unsigned int bodyidx = 0;
  while (bodyidx < body->size() && rangesidx < ranges->size())
  {
    //std::cout << "Checking char idx: " << bodyidx << "('" << body->at(bodyidx) << "') for range with start: " << ranges->at(rangesidx).start << std::flush;

    int charsizeinbytes = bytesToUtf8CharSize(*body, bodyidx);
    int charsizeinutf16entities = utf16CharSize(*body, bodyidx);

    //std::cout << ", charsize: " << charsizeinbytes << " codepoints: " << charsizeinutf16entities <<  std::endl;

    if (bodyidx == ranges->at(rangesidx).start)
    {
      //int length = bytesToUtf8CharSize(*body, bodyidx, ranges->at(rangesidx).length);
      int length = numBytesInUtf16Substring(*body, bodyidx, ranges->at(rangesidx).length);
      std::string pre = ranges->at(rangesidx).pre;
      std::string replacement = ranges->at(rangesidx).replacement;
      std::string post = ranges->at(rangesidx).post;

      if (replacement.empty())
        replacement = body->substr(bodyidx, length);

      body->replace(bodyidx, length, pre + replacement + post);

      for (uint i = 0; i < pre.size(); ++i)
        if (pre[i] == '<' || pre[i] == '>' || pre[i] == '\'' || pre[i] == '"' || pre[i] == '&')
          if (positions_excluded_from_escape)
            positions_excluded_from_escape->insert(i + bodyidx);
      for (uint i = 0; i < post.size(); ++i)
        if (post[i] == '<' || post[i] == '>' || post[i] == '\'' || post[i] == '"' || post[i] == '&')
          if (positions_excluded_from_escape)
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
    for (uint i = rangesidx; i < ranges->size(); ++i)
      ranges->at(i).start += (charsizeinbytes - charsizeinutf16entities);

    // next char...
    bodyidx += charsizeinbytes;
  }
}

/*
  This should work for (possible) overlapping ranges with the
  depending on how html renders certain things.
*/
void SignalBackup::prepRanges(std::vector<Range> *ranges) const
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

      ! NOTE only one of them can have replacement, not both *
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
        return prepRanges(ranges);
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
      return prepRanges(ranges);
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
      return prepRanges(ranges);
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

        return prepRanges(ranges);
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
      return prepRanges(ranges);
    }
  }
  // std::cout << "DONE!" << std::endl;
}


/*
  This should work for (possible) overlapping ranges with the
  depending on how html renders certain things.
*/
void SignalBackup::prepRanges2(std::vector<Range> *ranges) const
{
  std::sort(ranges->begin(), ranges->end());

  // if (ranges->size())
  // {
  //   std::cout << "got range:" << std::endl;
  //   for (auto const &r : *ranges)
  //     std::cout << "[" << r.start << "-" << r.start + r.length << "] '" << r.pre << "' '" << r.post << "'" << std::endl;
  // }

  /*
    After sorting, there are the following possible overlaps between range_i-1 and range_i:

    (1)
      O1: <1>      </1>
      O1: <2>      </2>
      ->
      N1: <1><2>   </2></1>

      ! NOTE only one of them can have replacement, not both *
      ! <N1> copies replacement from 1/2

    (2)
      O1: <1>      </1>
      O2: <2>               </2>
      ->
      N1: <2><1>   </1>
      N2:          ""       </2>

      ! NOTE <2> cannot have replacement (<1> would fall in the middle of it) **
      ! IF <1> has replacement <N1> has replacement

    (3)
      O1: <1>          </1>
      O2:      <2>     </2>
      ->
      N1: <1>  ""
      N2:      <2>     </2></1>

      ! NOTE <1> cannot have replacement (<2> would fall in the middle of it) **
      ! IF <2> has replacement <N2> has replacement

    (4)
      O1: <1>          </1>
      O2:      <2>                </2>
      ->
      N1: <1>
      N2:      <2>     </2></1>
      N3:              <2>        </2>

      ! NOTE neither <1> or <2> can have replacement **

    (5)
      O1: <1>                    </1>
      O2:      <2>    </2>
      ->
      N1: <1>  ""
      N2: (O2)
      N3:             ""         </1>

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
        ranges->at(i - 1).pre += ranges->at(i).pre;
        ranges->at(i - 1).post = ranges->at(i).post + ranges->at(i - 1).post;
        if (!ranges->at(i).replacement.empty()) // only one can have replacement, if it's i-1 its already kept...
          ranges->at(i - 1).replacement = ranges->at(i).replacement;
        ranges->erase(ranges->begin() + i);
        return prepRanges2(ranges);
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
      ranges->at(i - 1).pre += ranges->at(i).pre;
      ranges->at(i).pre = "";
      ranges->at(i).start = ranges->at(i - 1).start + ranges->at(i - 1).length;
      ranges->at(i).length -= ranges->at(i - 1).length;
      return prepRanges2(ranges);
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
      // ranges->at(i).replacement is automatically kept if it has one
      ranges->at(i).post = ranges->at(i - 1).post + ranges->at(i).post;
      ranges->at(1 - 1).post = "";
      ranges->at(i - 1).length = ranges->at(i).start - ranges->at(i - 1).start;
      return prepRanges2(ranges);
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
                          ranges->at(i).pre,
                          "",
                          ranges->at(i).post + ranges->at(i - 1).post}; // N2
        ranges->emplace_back(newrange);

        ranges->at(i - 1).length = newrange.start - ranges->at(i - 1).start; // N1
        ranges->at(i - 1).post = "";

        ranges->at(i).start = newrange.start + newrange.length; // N3
        ranges->at(i).length -= newrange.length;

        return prepRanges2(ranges);
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
                        "",
                        "",
                        ranges->at(i - 1).post};
      ranges->emplace_back(newrange);                                           // -> N3

      ranges->at(i - 1).post = "";
      ranges->at(i - 1).length = ranges->at(i).start - ranges->at(i - 1).start; // O1 -> N1

      // ranges->at(i).replacement is automatically kept if it has one
      // O2 == N2
      return prepRanges2(ranges);
    }
  }
  // std::cout << "DONE!" << std::endl;
}
