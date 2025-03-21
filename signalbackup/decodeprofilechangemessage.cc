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

#include "../groupv2statusmessageproto_typedef/groupv2statusmessageproto_typedef.h"
#include "../protobufparser/protobufparser.h"

std::string SignalBackup::decodeProfileChangeMessage(std::string const &body, std::string const &name) const
{
  /*
    // from app/src/main/proto/Database.proto
    message ProfileChangeDetails {
      message StringChange {
        string previous = 1;
        string new      = 2;
      }
      StringChange profileNameChange = 1;
    }
  */

  ProfileChangeDetails profchangefull(body);

  //std::cout << body << std::endl;
  //profchangefull.print();

  if (!profchangefull.getField<1>().has_value())
    return name + " has changed their profile name.";

  StringChange profilenamechange = profchangefull.getField<1>().value();

  if ((!profilenamechange.getField<1>().has_value() || profilenamechange.getField<1>().value() == "") ||
      (!profilenamechange.getField<2>().has_value() || profilenamechange.getField<2>().value() == ""))
    return name + " has changed their profile name.";

  std::string oldname = profilenamechange.getField<1>().value();
  std::string newname = profilenamechange.getField<2>().value();

  return oldname + " changed their profile name to " + newname + "."; //decodeProfileChange(body);
}
