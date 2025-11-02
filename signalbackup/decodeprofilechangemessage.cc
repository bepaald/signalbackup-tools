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

std::string SignalBackup::decodeProfileChangeMessage(std::string const &body, std::string const &name, IconType *icon) const
{
  /*
    // from app/src/main/protowire/Database.proto
    message ProfileChangeDetails {
      message StringChange {
        string previous = 1;
        string new      = 2;
      }
      message LearnedProfileName {
        oneof PreviouslyKnownAs {
          string e164 = 1;
          string username = 2;
        }
      }

      StringChange profileNameChange = 1;
      // Deprecated - Use learnedProfileName instead
      StringChange       deprecatedLearnedProfileName = 2;
      LearnedProfileName learnedProfileName           = 3;
    }
  */

  ProfileChangeDetails profchangefull(body);

  //std::cout << body << std::endl;
  //profchangefull.print();

  auto profilenamechange = profchangefull.getField<1>();
  if (profilenamechange.has_value())
  {
    auto oldname_protobuf = profilenamechange.value().getField<1>();
    if (!oldname_protobuf.has_value() || oldname_protobuf.value().empty())
      return name + " has changed their profile name.";
    auto newname_protobuf = profilenamechange.value().getField<2>();
    if (!newname_protobuf.has_value() || newname_protobuf.value().empty())
      return name + " has changed their profile name.";
    return oldname_protobuf.value() + " changed their profile name to " + newname_protobuf.value() + ".";
  }

  // no StringChange (field 1), maybe field 3?
  auto learnedprofile = profchangefull.getField<3>();
  if (learnedprofile.has_value())
  {
    if (icon && *icon == IconType::NONE)
      *icon = IconType::THREAD;

    auto learned_e164 = learnedprofile.value().getField<1>();
    if (learned_e164.has_value())
      return "You started this chat with " + learned_e164.value();

    auto learned_username = learnedprofile.value().getField<2>();
    if (learned_username.has_value())
      return "You started this chat with " + learned_username.value();
  }

  if (profchangefull.getField<2>().has_value())
  {
    Logger::warning("Encountered 'deprecatedLearnedProfileName' in profileChange message. This format is not (yet) supported.");
    return "";
  }

  return name + " has changed their profile name.";

}
