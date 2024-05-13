/*
  Copyright (C) 2024  Selwin van Dijk

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

void SignalBackup::updateSnippetExtrasRecipient(long long int id1, long long int id2) const // if id2 == -1, id1 is an offset
{                                                                                           // else, change id1 into id2
  if (d_database.tableContainsColumn("thread", "snippet_extras"))
  {
    if (id2 == -1)
    {
      d_database.exec("UPDATE thread SET snippet_extras = "
                      "json_set(snippet_extras, '$.individualRecipientId', CAST(json_extract(snippet_extras, '$.individualRecipientId') + ? AS text))", id1);
      if (d_verbose) [[unlikely]]
        Logger::message("     Updated ", d_database.changed(), " recipients in thread.snippet_extras");
    }
    else
    {
      d_database.exec("UPDATE thread SET snippet_extras = "
                      "json_set(snippet_extras, '$.individualRecipientId', CAST(? AS text)) "
                      "WHERE json_extract(snippet_extras, '$.individualRecipientId') = CAST(? AS text)", {id2, id1});
      if (d_verbose) [[unlikely]]
        Logger::message("     Updated ", d_database.changed(), " recipients in thread.snippet_extras");
    }
  }
}
