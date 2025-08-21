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

#include "signalbackup.h"
#include "htmlicontypes.h"

struct HTMLMessageInfo
{
  std::string body;
  std::string quote_body;
  std::string readable_date;
  std::string const &directory;
  std::string const &threaddir;
  std::string const &filename;
  std::string link_preview_url;
  std::string link_preview_title;
  std::string link_preview_description;
  std::string shared_contacts;

  SqliteDB::QueryResults *messages;
  SqliteDB::QueryResults *quote_attachment_results;
  SqliteDB::QueryResults *attachment_results;
  SqliteDB::QueryResults *reaction_results;
  SqliteDB::QueryResults *edit_revisions;

  long long int type;
  long long int expires_in;
  long long int msg_id;
  long long int msg_recipient_id;
  long long int original_message_id;
  long long int quote_id; // 0 if none

  IconType icon; //size: 4
  unsigned int idx;

  bool only_emoji;
  bool is_quoted;
  bool is_deleted;
  bool is_viewonce;
  bool isgroup;
  bool incoming;
  bool nobackground;
  bool quote_missing;
  bool orig_filename;
  bool overwrite;
  bool append;
  bool story_reply;
};
