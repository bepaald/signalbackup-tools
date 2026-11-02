/*
  Copyright (C) 2026  Selwin van Dijk

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

#ifdef BEPAALD_BV2_ENABLED

#include "signalbackup.ih"

/*
  Called once, after all recipient frames have been processed (they always come
  before any chat or chatitem).

  The backup does not contain the ACI of 'self', but a lot of functions in this tool
  need it (for example to show 'You' in group updates). Every recipient Android knows
  about is exported as a contact, except for self. Every group member also has a
  recipient. So, an ACI that is a member of a group but has no contact is (almost
  certainly) self.
*/
bool SignalBackup::bv2FinalizeRecipients(BackupV2ImportState *state)
{
  state->recipientsfinalized = true;

  if (d_selfid == -1) [[unlikely]]
  {
    Logger::error("No AccountData found in backup");
    return false;
  }

  // find self aci
  std::map<std::string, int> candidates;
  for (auto const &[group_id, members] : state->groupmembers)
    for (auto const &[aci, role] : members)
      if (!aci.empty() && bv2GetRecipientIdForAci(aci, false) == -1)
        ++candidates[aci];

  std::string selfaci;
  int selfaci_count = 0;
  for (auto const &[aci, count] : candidates)
    if (count > selfaci_count)
    {
      selfaci = aci;
      selfaci_count = count;
    }

  if (!selfaci.empty())
  {
    if (candidates.size() > 1) [[unlikely]]
      Logger::warning("Multiple candidates for ACI of self, picked the most likely (", selfaci_count, " groups)");
    if (!d_database.exec("UPDATE recipient SET aci = ? WHERE _id = ?", {selfaci, d_selfid})) [[unlikely]]
      return false;
    d_selfuuid = selfaci;
    if (d_verbose) [[unlikely]]
      Logger::message("Determined self ACI: ", selfaci);
  }
  else
    Logger::warning("Failed to determine ACI of self (this is expected when not a member of any group)");

  // set group members
  for (auto const &[group_id, members] : state->groupmembers)
  {
    for (auto const &[aci, role] : members)
    {
      long long int rid = bv2GetRecipientIdForAci(aci, true);
      if (rid == -1) [[unlikely]]
      {
        Logger::warning("Failed to get recipient for group member");
        continue;
      }

      if (!insertRow("group_membership", {{"group_id", group_id}, {"recipient_id", rid}})) [[unlikely]]
        Logger::warning("Failed to insert group member");

      if (rid == d_selfid)
        d_database.exec("UPDATE groups SET active = 1 WHERE group_id = ?", group_id);
    }
  }
  state->groupmembers.clear();

  return true;
}

#endif
