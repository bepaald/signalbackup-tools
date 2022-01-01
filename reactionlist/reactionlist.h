/*
    Copyright (C) 2020-2022  Selwin van Dijk

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

#ifndef REACTIONLIST_H_
#define REACTIONLIST_H_

#include "../protobufparser/protobufparser.h"

/*
message ReactionList {
    message Reaction {
        string emoji        = 1;
        uint64 author       = 2;
        uint64 sentTime     = 3;
        uint64 receivedTime = 4;
    }

    repeated Reaction reactions = 1;
}
*/

class ReactionList : public ProtoBufParser<std::vector<ProtoBufParser<protobuffer::optional::STRING,
                                                                      protobuffer::optional::UINT64,
                                                                      protobuffer::optional::UINT64,
                                                                      protobuffer::optional::UINT64>>>
{
 public:
  inline ReactionList(std::pair<std::shared_ptr<unsigned char []>, size_t> const &data);

  inline unsigned int numReactions() const;
  inline std::string getEmoji(int idx) const;
  inline uint64_t getAuthor(int idx) const;
  inline uint64_t getSentTime(int idx) const;
  inline uint64_t getReceivedTime(int idx) const;

  bool setAuthor(unsigned int idx, uint64_t author);
};

inline ReactionList::ReactionList(std::pair<std::shared_ptr<unsigned char []>, size_t> const &data)
  :
  ProtoBufParser(data)
{}

inline unsigned int ReactionList::numReactions() const
{
  return getField<1>().size();
}

inline std::string ReactionList::getEmoji(int idx) const
{
  return getField<1>()[idx].getField<1>().value_or("");
}

inline uint64_t ReactionList::getAuthor(int idx) const
{
  return getField<1>()[idx].getField<2>().value_or(0);
}

inline uint64_t ReactionList::getSentTime(int idx) const
{
  return getField<1>()[idx].getField<3>().value_or(0);
}

inline uint64_t ReactionList::getReceivedTime(int idx) const
{
  return getField<1>()[idx].getField<4>().value_or(0);
}

#endif
