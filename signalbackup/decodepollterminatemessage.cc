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

std::string SignalBackup::decodePollTerminateMessage(std::string const &body, long long int type, std::string const &name, IconType *icon) const
{
  /*
    // from app/src/main/protowire/Database.proto

    message PollTerminate {
      string question = 1;
      uint64 messageId = 2;
      uint64 targetTimestamp = 3;
    }
  */

  PollTerminate pollterminate(body);

  //std::cout << body << std::endl;
  //profchangefull.print();

  std::string status_message((Types::isOutgoing(type) ? "You" : name) + " ended the poll");

  auto title = pollterminate.getField<1>();
  if (title.has_value())
    status_message.append(": \"" + title.value() + "\"");

  if (icon)
    *icon = IconType::POLL_TERMINATE;

  return status_message;
}
