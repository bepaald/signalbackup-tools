/*
  Copyright (C) 2023-2024  Selwin van Dijk

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

#include "signalbackup.h"

#include <list>

struct GroupInfo
{
  std::list<long long int> admin_ids;
  long long int expiration_timer = 0;
  bool link_invite_enabled = false;
  std::string description;
  std::vector<long long int> pending_members;
  std::vector<long long int> requesting_members;
  std::vector<long long int> banned_members;
  std::string access_control_attributes;
  std::string access_control_members;
  std::string access_control_addfromlinkinvite;
  bool isannouncementgroup = false;
};
