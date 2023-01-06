/*
  Copyright (C) 2020-2023  Selwin van Dijk

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

#include "reactionlist.ih"

bool ReactionList::setAuthor(unsigned int idx, uint64_t author)
{
  //std::cout << "BEFORE EDIT  : " << getDataString() << std::endl;

  if (idx >= numReactions())
    return false;

  std::vector<ProtoBufParser<protobuffer::optional::STRING,
                             protobuffer::optional::UINT64,
                             protobuffer::optional::UINT64,
                             protobuffer::optional::UINT64>> reactions = getField<1>();

  std::vector<ProtoBufParser<protobuffer::optional::STRING,
                             protobuffer::optional::UINT64,
                             protobuffer::optional::UINT64,
                             protobuffer::optional::UINT64>> newreactions;
  for (uint i = 0; i < reactions.size(); ++i)
  {
    if (i != idx)
      newreactions.push_back(reactions[i]); // just copy all unchanged
    else
    {
      ProtoBufParser<protobuffer::optional::STRING, protobuffer::optional::UINT64,
                     protobuffer::optional::UINT64, protobuffer::optional::UINT64> tmp;
      if (!tmp.addField<1>(reactions[i].getField<1>().value()) ||
          !tmp.addField<2>(author) ||
          !tmp.addField<3>(reactions[i].getField<3>().value()) ||
          !tmp.addField<4>(reactions[i].getField<4>().value()))
        return false;
      newreactions.push_back(tmp);
    }
  }

  // clear entire list
  clear();
  // set new data
  for (uint i = 0; i < newreactions.size(); ++i)
    if (!addField<1>(newreactions[i]))
      return false;

  //std::cout << "AFTER EDIT 1 : " << getDataString() << std::endl;

  return true;

  // // get the reaction we're editing
  // ProtoBufParser<protobuffer::optional::STRING,
  //                protobuffer::optional::UINT64,
  //                protobuffer::optional::UINT64,
  //                protobuffer::optional::UINT64> entry = getField<1>()[idx];

  // // delete that reaction from list
  // if (deleteFields(1, &entry) != 1)
  //   return false;

  // // delete the current author
  // if (entry.deleteFields(2) != 1)
  //   return false;

  // // set new field
  // if (!(entry.addField<2>(protobuffer::optional::UINT64{author})))
  //   return false;

  // //std::cout << "Adding updated reaction" << std::endl;
  // return addField<1>(entry);
}
