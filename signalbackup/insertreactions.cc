/*
  Copyright (C) 2022-2023  Selwin van Dijk

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

void SignalBackup::insertReactions(long long int message_id, std::vector<std::vector<std::string>> const &reactions,
                                   bool mms, std::map<std::string, long long int> *savedmap) const
{
  if (d_verbose && reactions.size()) [[unlikely]]
    std::cout << "Inserting " << reactions.size() << " message reactions." << std::endl;

  // insert into reactions
  for (auto const &r : reactions)
  {
    // r[0] : emoji
    // r[1] : timestamp
    // r[2] : author uuid
    // r[3] : author phone

    long long int author = -1;
    if (!r[2].empty())
      author = getRecipientIdFromUuid(r[2], savedmap);
    if (author == -1)
      author = getRecipientIdFromPhone(r[3], savedmap);

    if (author == -1)
    {
      std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << ": Reaction author not found. Skipping" << std::endl;
      continue;
    }
    if (d_database.tableContainsColumn("reaction", "is_mms")) // not actually removed yet? just unused...
    {
      if (!insertRow("reaction",
                     {{"message_id", message_id},
                      {"is_mms", mms ? 1 : 0},
                      {"author_id", author},
                      {"emoji", r[0]},
                      {"date_sent", bepaald::toNumber<long long int>(r[1])},
                      {"date_received", bepaald::toNumber<long long int>(r[1])}}))
      {
        std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Failed to insert into reaction table" << std::endl;
      }
    }
    else
    {
      if (!insertRow("reaction",
                     {{"message_id", message_id},
                      {"author_id", author},
                      {"emoji", r[0]},
                      {"date_sent", bepaald::toNumber<long long int>(r[1])},
                      {"date_received", bepaald::toNumber<long long int>(r[1])}}))
        std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Failed to insert into reaction table" << std::endl;
    }
  }
}
