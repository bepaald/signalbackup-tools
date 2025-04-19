/*
  Copyright (C) 2021-2025  Selwin van Dijk

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

#include "../mimetypes/mimetypes.h"
#include "../attachmentmetadata/attachmentmetadata.h"
#include "../common_filesystem.h"

bool SignalBackup::dumpAvatars(std::string const &dir, std::vector<std::string> const &contacts, bool overwrite) const
{
  Logger::message_overwrite("Dumping avatars to dir '", dir, "'...");

  if (!d_database.containsTable("recipient"))
  {
    Logger::error("Database too old, dumping avatars is not (yet) supported, consider a full decrypt by just passing a directory as output");
    return false;
  }

  if (!prepareOutputDirectory(dir, overwrite))
    return false;

#if __cplusplus > 201703L
  for (int count = 0; auto const &avframe : d_avatars)
#else
  int count = 0;
  for (auto const &avframe : d_avatars)
#endif
  {
    ++count;

    Logger::message_overwrite("Dumping avatars to dir '", dir, "'... ", count, "/", d_avatars.size());

    AvatarFrame *af = avframe.second.get();

    SqliteDB::QueryResults results;

    std::string query = "SELECT COALESCE(" + (d_database.tableContainsColumn("recipient", "nickname_joined_name") ? "NULLIF(recipient.nickname_joined_name, ''),"s : ""s) +
      "NULLIF(recipient." + d_recipient_system_joined_name + ", ''), " +
      (d_database.tableContainsColumn("recipient", "profile_joined_name") ? "NULLIF(recipient.profile_joined_name, ''),"s : ""s) +
      "NULLIF(recipient." + d_recipient_profile_given_name + ", ''), NULLIF(groups.title, ''), " +
      (d_database.containsTable("distribution_list") ? "NULLIF(distribution_list.name, ''), " : "") +
      "NULLIF(recipient." + d_recipient_e164 + ", ''), NULLIF(recipient." + d_recipient_aci + ", ''), "
      " recipient._id) AS 'chatpartner' "
      "FROM recipient "
      "LEFT JOIN groups ON recipient.group_id = groups.group_id " +
      (d_database.containsTable("distribution_list") ? "LEFT JOIN distribution_list ON recipient._id = distribution_list.recipient_id " : "") +
      "WHERE recipient._id = ?";

    // if ! limit.empty()
    // query += " AND _id == something, or chatpartner == ''

    if (!d_database.exec(query, af->recipient(),  &results))
      return false;

    if (results.rows() != 1)
    {
      Logger::error("Unexpected number of results: ", results.rows(), " (recipient: ", af->recipient(), ")");
      continue;
    }

    std::string name = results.valueAsString(0, "chatpartner");

    if (!contacts.empty() &&
        std::find(contacts.begin(), contacts.end(), name) == contacts.end())
      continue;

    // get avatar data, to get extension
    std::string extension;
    unsigned char *avatardata = af->attachmentData(d_verbose);
    uint64_t avatarsize = af->attachmentSize();
    AttachmentMetadata amd = AttachmentMetadata::getAttachmentMetaData(std::string(), avatardata, avatarsize, true/*skiphash*/);
    extension = "." + std::string(MimeTypes::getExtension(amd.filetype, "jpg"));
    std::string filename = sanitizeFilename(name + extension);
    if (filename.empty() || filename == extension) // filename was not set in database or was not impossible
                                                   // to sanitize (eg reserved name in windows 'COM1')
      filename = af->recipient() + extension;

    // make filename unique
    while (bepaald::fileOrDirExists(bepaald::concat(dir, "/", filename)))
      filename += "(2)";

    std::ofstream attachmentstream(bepaald::concat(dir, "/", filename), std::ios_base::binary);

    if (!attachmentstream.is_open())
    {
      Logger::error("Failed to open file for writing: ", dir, "/", filename);
      af->clearData();
      continue;
    }

    if (!attachmentstream.write(reinterpret_cast<char *>(af->attachmentData(d_verbose)), af->attachmentSize()))
    {
      Logger::error("Failed to write data to file: ", dir, "/", filename);
      af->clearData();
      continue;
    }

    attachmentstream.close(); // need to close, or the auto-close will change files mtime again.
    af->clearData();
  }

  Logger::message("done.");
  return true;
}
