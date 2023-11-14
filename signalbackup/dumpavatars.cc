/*
  Copyright (C) 2021-2023  Selwin van Dijk

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

bool SignalBackup::dumpAvatars(std::string const &dir, std::vector<std::string> const &contacts, bool overwrite) const
{
  std::cout << "Dumping avatars to dir '" << dir << "'" << std::endl;

  if (!d_database.containsTable("recipient"))
  {
    std::cout << "Database too old, dumping avatars is not (yet) supported, consider a full decrypt by just passing a directory as output" << std::endl;
    return false;
  }

  if (!prepareOutputDirectory(dir, overwrite))
    return false;
  // if (!bepaald::isDir(dir))
  // {
  //   std::cout << "Output directory '" << dir << "' does not exist or is not a directory" << std::endl;
  //   return false;
  // }

  // if (!bepaald::isEmpty(dir))
  // {
  //   if (!overwrite)
  //   {
  //     std::cout << "Directory '" << dir << "' is not empty. Use --overwrite to clear directory before dump" << std::endl;
  //     return false;
  //   }
  //   std::cout << "Clearing contents of directory '" << dir << "'..." << std::endl;
  //   if (!bepaald::clearDirectory(dir))
  //   {
  //     std::cout << "Failed to empty directory '" << dir << "'" << std::endl;
  //     return false;
  //   }
  // }

#if __cplusplus > 201703L
  for (int count = 0; auto const &avframe : d_avatars)
#else
  int count = 0;
  for (auto const &avframe : d_avatars)
#endif
  {
    ++count;

    AvatarFrame *af = avframe.second.get();

    SqliteDB::QueryResults results;

    std::string query = "SELECT COALESCE(NULLIF(recipient." + d_recipient_system_joined_name + ", ''), " +
      (d_database.tableContainsColumn("recipient", "profile_joined_name") ? "NULLIF(recipient.profile_joined_name, ''),"s : ""s) +
      "NULLIF(recipient." + d_recipient_profile_given_name + ", ''), NULLIF(groups.title, ''), "
      "NULLIF(recipient." + d_recipient_e164 + ", ''), NULLIF(recipient." + d_recipient_aci + ", ''), "
      " recipient._id) AS 'chatpartner' "
      "FROM recipient LEFT JOIN groups ON recipient.group_id = groups.group_id WHERE recipient._id = ?";

    // if ! limit.empty()
    // query += " AND _id == something, or chatpartner == ''

    if (!d_database.exec(query, af->recipient(),  &results))
      return false;

    if (results.rows() != 1)
    {
      std::cout << " ERROR Unexpected number of results: " << results.rows()
                << " (recipient: " << af->recipient() << ")" << std::endl;
      continue;
    }

    std::string name = results.valueAsString(0, "chatpartner");

    if (!contacts.empty() &&
        std::find(contacts.begin(), contacts.end(), name) == contacts.end())
      continue;

    std::string filename = sanitizeFilename(name + ".jpg");
    if (filename.empty() || filename == ".jpg") // filename was not set in database or was not impossible
                                                // to sanitize (eg reserved name in windows 'COM1')
      filename = af->recipient() + ".jpg";

    // make filename unique
    while (bepaald::fileOrDirExists(dir + "/" + filename))
      filename += "(2)";

    std::ofstream attachmentstream(dir + "/" + filename, std::ios_base::binary);

    if (!attachmentstream.is_open())
    {
      std::cout << " ERROR Failed to open file for writing: " << dir << "/" << filename << std::endl;
      continue;
    }
    else
      if (!attachmentstream.write(reinterpret_cast<char *>(af->attachmentData()), af->attachmentSize()))
      {
        std::cout << " ERROR Failed to write data to file: " << dir << "/" << filename << std::endl;
        af->clearData();
        continue;
      }
    attachmentstream.close(); // need to close, or the auto-close will change files mtime again.
    af->clearData();
  }

  std::cout << std::endl << "done." << std::endl;
  return true;
}
