/*
  Copyright (C) 2023-2025  Selwin van Dijk

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
  // sort the ranges and adjust them
  // to deal with overlaps
  prepRanges(ranges);


  // then, apply the ranges:
  unsigned int rangesidx = 0;
  unsigned int bodyidx = 0;
  while (bodyidx < body->size() && rangesidx < ranges->size())
  {
    //std::cout << "Checking char idx: " << bodyidx << "('" << (*body)[bodyidx] << "') for range with start: " << (*ranges)[rangesidx].start << std::flush;

    int charsizeinbytes = bytesToUtf8CharSize(*body, bodyidx);
    int charsizeinutf16entities = utf16CharSize(*body, bodyidx);

    //std::cout << ", charsize: " << charsizeinbytes << " codepoints: " << charsizeinutf16entities <<  std::endl;

    if (bodyidx == (*ranges)[rangesidx].start)
    {
      //int length = bytesToUtf8CharSize(*body, bodyidx, (*ranges)[rangesidx].length);
      int length = numBytesInUtf16Substring(*body, bodyidx, (*ranges)[rangesidx].length);
      std::string pre = (*ranges)[rangesidx].pre;
      std::string replacement = (*ranges)[rangesidx].replacement;
      std::string post = (*ranges)[rangesidx].post;

      if (replacement.empty())
        replacement = body->substr(bodyidx, length);

      body->replace(bodyidx, length, bepaald::concat(pre, replacement, post));

      for (unsigned int i = 0; i < pre.size(); ++i)
        if (positions_excluded_from_escape)
          positions_excluded_from_escape->insert(i + bodyidx);
      for (unsigned int i = 0; i < post.size(); ++i)
        if (positions_excluded_from_escape)
          positions_excluded_from_escape->insert(i + bodyidx + pre.size() + replacement.size());

      //std::cout << "BODY: " << *body << std::endl;

      for (unsigned int i = rangesidx + 1; i < ranges->size(); ++i)
        (*ranges)[i].start += pre.size() + post.size() + (replacement.size() - (*ranges)[rangesidx].length);

      // skip the just inserted string
      bodyidx += pre.size() + post.size() + replacement.size();

      // look for next range
      // while the prepwork should make sure it is the first one,
      // interactions with mention replacements might throw it off?
      // just to be sure, lets not just `++rangesidx'
      while (++rangesidx < ranges->size() && (*ranges)[rangesidx].start < bodyidx)
        ;
      continue;
    }

    // update all following ranges for multibyte char
    for (unsigned int i = rangesidx; i < ranges->size(); ++i)
      (*ranges)[i].start += (charsizeinbytes - charsizeinutf16entities);

    // next char...
    bodyidx += charsizeinbytes;
  }
}

/*
  This should work for (possible) overlapping ranges
  depending on how html renders certain things.
*/
void SignalBackup::prepRanges(std::vector<Range> *ranges) const
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

    (4a)
      O1: <1>          </1>
      O2:      <2>                </2>
      ->
      N1: <1>  ""
      N2:      <2>     </2></1>
      N3:              <2>        </2>

      ! NOTE: <1> is unbroken!

      ! NOTE neither <1> or <2> can have replacement **

    (4b) ???
      O1: <1>                   </1>
      O2:      <2>                      </2>
      ->
      N1: <1>  </1><2>
      N2:      <1>              </1>
      N3:                       ""      </2>

      ! NOTE : <2> is unbroken!

      ! NOTE neither <1> or <2> can have replacement **

      !! NOTE CASE 4 (a&b) DO NOT SEEM TO BE ALLOWED IN SIGNAL
      !!! NOTE IN CASE 4ab, ONE OF THE RANGES MUST BE BROKEN (IF BOTH
          HAVE nobreak == true, ONE WILL BREAK ANYWAY)

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

  for (unsigned int i = 1; i < ranges->size(); ++i)
  {
    if ((*ranges)[i].start == (*ranges)[i - 1].start)
    {
      if ((*ranges)[i].length == (*ranges)[i -1].length)
      {
        // (1) SAME START, SAME FINISH
        if (!(*ranges)[i - 1].replacement.empty() &&
            !(*ranges)[i].replacement.empty())
        {
          Logger::warning("Illegal range-set (overlapping ranges both have replacement).");
          continue;
        }
        //std::cout << "CASE 1" << std::endl;
        (*ranges)[i - 1].pre += (*ranges)[i].pre;
        (*ranges)[i - 1].post = (*ranges)[i].post + (*ranges)[i - 1].post;
        if (!(*ranges)[i].replacement.empty()) // only one can have replacement, if it's i-1 its already kept...
          (*ranges)[i - 1].replacement = (*ranges)[i].replacement;
        ranges->erase(ranges->begin() + i);
        return prepRanges(ranges);
      }
      // (2) SAME START, LATER FINISH
      // else -> i.length > (i - 1).length
      if (!(*ranges)[i].replacement.empty())
      {
        Logger::warning("Illegal range-set (overlapping ranges both have replacement).");
        continue;
      }
      //std::cout << "CASE 2" << std::endl;
      // (*ranges)[i - 1].replacement is automatically kept if it has one
      (*ranges)[i - 1].pre = (*ranges)[i].pre + (*ranges)[i - 1].pre;
      (*ranges)[i].pre = "";
      (*ranges)[i].start = (*ranges)[i - 1].start + (*ranges)[i - 1].length;
      (*ranges)[i].length -= (*ranges)[i - 1].length;
      return prepRanges(ranges);
    }
    // else (i].start > (i - 1).start)
    if ((*ranges)[i].start + (*ranges)[i].length == (*ranges)[i - 1].start + (*ranges)[i - 1].length) // same end pos
    {
      // (3) LATER START, SAME FINISH
      if (!(*ranges)[i - 1].replacement.empty())
      {
        Logger::warning("Warning. Illegal range-set (overlapping ranges both have replacement).");
        continue;
      }
      //std::cout << "CASE 3" << std::endl;
      // (*ranges)[i].replacement is automatically kept if it has one
      (*ranges)[i].post = (*ranges)[i].post + (*ranges)[i - 1].post;
      (*ranges)[i - 1].post = "";
      (*ranges)[i - 1].length = (*ranges)[i].start - (*ranges)[i - 1].start;
      return prepRanges(ranges);
    }
    if ((*ranges)[i].start < (*ranges)[i - 1].start + (*ranges)[i - 1].length)
    {
      if ((*ranges)[i].start + (*ranges)[i].length > (*ranges)[i - 1].start + (*ranges)[i - 1].length) // end later
      {
        // (4) LATER START, LATER FINISH"
        if (!(*ranges)[i - 1].replacement.empty() ||
            !(*ranges)[i].replacement.empty())
        {
          Logger::warning("Warning. Illegal range-set (overlapping ranges both have replacement).");
          continue;
        }

        if ((*ranges)[i].nobreak && !(*ranges)[i - 1].nobreak)
        {
          //std::cout << "CASE 4b" << std::endl;
          Range newrange = {(*ranges)[i - 1].start,
                            (*ranges)[i].start - (*ranges)[i - 1].start,
                            (*ranges)[i - 1].pre,
                            "",
                            (*ranges)[i - 1].post + (*ranges)[i].pre,
                            false}; // N1
          ranges->emplace_back(newrange);

          (*ranges)[i - 1].start = (*ranges)[i].start;
          (*ranges)[i - 1].length = (*ranges)[i - 1].length - newrange.length; // N2

          (*ranges)[i].start = (*ranges)[i - 1].start + (*ranges)[i - 1].length;
          (*ranges)[i].pre = "";
          (*ranges)[i].length = (*ranges)[i].length - (*ranges)[i - 1].length; // N3
        }
        else
        {
          if ((*ranges)[i].nobreak && (*ranges)[i - 1].nobreak) [[unlikely]]
          {
            Logger::warning("Illegal ranges: overlapping but unbreakable");
            // std::cout << i << std::endl;
            // std::cout << (*ranges)[i].start << std::endl;
            // std::cout << (*ranges)[i].length << std::endl;
            // std::cout << (*ranges)[i].pre << std::endl;
            // std::cout << (*ranges)[i].post << std::endl;
            // std::cout << i - 1 << std::endl;
            // std::cout << (*ranges)[i-1].start << std::endl;
            // std::cout << (*ranges)[i-1].length << std::endl;
            // std::cout << (*ranges)[i-1].pre << std::endl;
            // std::cout << (*ranges)[i-1].post << std::endl;
          }

          //std::cout << "CASE 4a" << std::endl;
          Range newrange = {(*ranges)[i].start,
                            (*ranges)[i - 1].length - ((*ranges)[i].start - (*ranges)[i - 1].start),
                            (*ranges)[i].pre,
                            "",
                            (*ranges)[i].post + (*ranges)[i - 1].post,
                            false}; // N2
          ranges->emplace_back(newrange);

          (*ranges)[i - 1].length = newrange.start - (*ranges)[i - 1].start; // N1
          (*ranges)[i - 1].post = "";

          (*ranges)[i].start = newrange.start + newrange.length; // N3
          (*ranges)[i].length -= newrange.length;
        }
        return prepRanges(ranges);
      }
      //if ((*ranges)[i].start + (*ranges)[i].length < (*ranges)[i - 1].start + (*ranges)[i - 1].length) // end sooner
      // (5) LATER START, EARLIER FINISH
      if (!(*ranges)[i - 1].replacement.empty())
      {
        Logger::warning("Warning. Illegal range-set (overlapping ranges both have replacement).");
        continue;
      }
      //std::cout << "CASE 5" << std::endl;
      Range newrange = {(*ranges)[i].start + (*ranges)[i].length,
                        (*ranges)[i - 1].start + (*ranges)[i - 1].length - ((*ranges)[i].start + (*ranges)[i].length),
                        "",
                        "",
                        (*ranges)[i - 1].post,
                        false};
      ranges->emplace_back(newrange);                                           // -> N3

      (*ranges)[i - 1].post = "";
      (*ranges)[i - 1].length = (*ranges)[i].start - (*ranges)[i - 1].start; // O1 -> N1

      // (*ranges)[i].replacement is automatically kept if it has one
      // O2 == N2
      return prepRanges(ranges);
    }
  }
  // std::cout << "DONE!" << std::endl;
}
