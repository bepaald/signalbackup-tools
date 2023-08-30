/*
  Copyright (C) 2023  Selwin van Dijk

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
  bool only_emoji;
  bool is_deleted;
  bool is_viewonce;
  bool isgroup;
  bool incoming;
  bool nobackground;
  bool hasquote;
  bool overwrite;
  bool append;
  long long int type;
  long long int msg_id;
  long long int msg_recipient_id;
  long long int original_message_id;
  unsigned int idx;

  SqliteDB::QueryResults *messages;
  SqliteDB::QueryResults *quote_attachment_results;
  SqliteDB::QueryResults *attachment_results;
  SqliteDB::QueryResults *reaction_results;
  SqliteDB::QueryResults *edit_revisions;

  std::string body;
  std::string quote_body;
  std::string readable_date;
  std::string directory;
  std::string threaddir;
  std::string filename;
  std::string link_preview_title;
  std::string link_preview_description;

  IconType icon;
};
