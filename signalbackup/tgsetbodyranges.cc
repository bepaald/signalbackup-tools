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

#include "signalbackup.ih"

bool SignalBackup::tgSetBodyRanges(std::string const &bodyjson, long long int message_id)
{
  //std::cout << "bodydata: " << bodyjson << std::endl;

  long long int fragments = d_database.getSingleResultAs<long long int>("SELECT json_array_length(?, '$')", bodyjson, -1);
  if (fragments == -1)
  {
    Logger::error("Failed to get number of text fragments from message body. Body data: '" + bodyjson + "'");
    return false;
  }

  long long int currentpos = 0;
  BodyRanges bodyrangelist;

  SqliteDB::QueryResults br;
  for (unsigned int i = 0; i < fragments; ++i)
  {
    if (!d_database.exec("SELECT "
                         "json_extract(?1, '$[" + bepaald::toString(i) + "].text') AS text, "
                         "json_extract(?1, '$[" + bepaald::toString(i) + "].type') AS type",
                         bodyjson, &br) || br.rows() != 1)
    {
      Logger::error("Failed to get text fragment (", i, ") from message body. Body data: '" + bodyjson + "'");
      return false;
    }

    BodyRange bodyrange;
    std::string bodyfrag = br("text");
    unsigned int fraglen = 0;
    for (unsigned int c = 0; c < bodyfrag.length(); c += bytesToUtf8CharSize(bodyfrag, c))
      fraglen += utf16CharSize(bodyfrag, c);

    if (br("type") == "plain") [[likely]]
      ;
    else if (bepaald::contains(std::vector{"bold", "italic", "spoiler", "strikethrough", "code"}, br("type")))
    {
      // start pos
      if (currentpos != 0)
        bodyrange.addField<1>(currentpos);

      // length
      bodyrange.addField<2>(fraglen);

      // type
      if (br("type") == "bold")
        bodyrange.addField<4>(0);
      else if (br("type") == "italic")
        bodyrange.addField<4>(1);
      else if (br("type") == "spoiler")
        bodyrange.addField<4>(2);
      else if (br("type") == "strikethrough")
        bodyrange.addField<4>(3);
      else if (br("type") == "code") // monospace
        bodyrange.addField<4>(4);

      // add to list
      bodyrangelist.addField<1>(bodyrange);
    }
    else if (br("type") == "underline")
      warnOnce("Underline text styling is not supported by Signal");
    else if (br("type") == "link")
      warnOnce("'Link' text styling is not supported by Signal");
    else
      warnOnce("(unknown text styling: '" + br("type") + "')");

    currentpos += fraglen;
  }

  //bodyrangelist.print();

  // set it
  if (bodyrangelist.size())
  {
    std::pair<unsigned char *, size_t> bodyrangesdata(bodyrangelist.data(), bodyrangelist.size());
    if (!d_database.exec("UPDATE " + d_mms_table + " SET " + d_mms_ranges + " = ? WHERE rowid = ?", {bodyrangesdata, message_id}) ||
        d_database.changed() != 1)
    {
      Logger::error("Failed to set body ranges for message. Body data: '" + bodyjson + "'");
      return false;
    }
  }

  return true;
}
