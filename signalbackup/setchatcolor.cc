/*
  Copyright (C) 2024-2025  Selwin van Dijk

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

#include "../protobufparser/protobufparser.h"

bool SignalBackup::setChatColors(std::vector<std::pair<long long int, std::string>> const &colorlist)
{

  if (!d_database.tableContainsColumn("recipient", "chat_colors"))
  {
    Logger::error("Recipient table does not appear to contain chat_colors-column");
    return false;
  }

  /*
    message chatcolor/wallpaper {
      message SingleColor {
        int32 color = 1;
      }

      message LinearGradient {
        float rotation  = 1;
        repeated int32 colors    = 2;
        repeated float positions = 3;
      }

      message File {
        string uri = 1;
      }

      oneof wallpaper {
        SingleColor    singleColor    = 1;
        LinearGradient linearGradient = 2;
        File           file           = 3; // ONLY IN WALLPAPER
      }

      float dimLevelInDarkTheme = 4; // ONLY IN WALLPAPER
    }
  */

  bool ok = false; // will be true if _any_ color setting succeeds
  for (auto [rid, colorstr] : colorlist)
  {
    // colorstring to number:
    if ((!(colorstr.size() == 7 && colorstr[0] == '#') &&
         !(colorstr.size() == 9 && colorstr[0] == '#') &&
         colorstr.size() != 6 && colorstr.size() != 8) ||
        colorstr.find_first_not_of("#0123456789ABCDEFabcdef") != std::string::npos)
    {
      Logger::warning("Skipping [", rid, " -> ", colorstr, "]. Illegal colorstring format. Use (#)(AA)RRGGBB in hexadecimal notation.");
      continue;
    }

    // chop off leading #
    if (colorstr[0] == '#')
      colorstr = colorstr.substr(1);

    // append opacity
    if (colorstr.size() == 6)
      colorstr = bepaald::concat("FF", colorstr);
    //Logger::message("GOT COLOR STRING: ", colorstr);

    std::unique_ptr<unsigned char[]> color_data(new unsigned char[4]);
    bepaald::hexStringToBytes(colorstr, color_data.get(), 4);
    uint32_t color_value = 0;
    std::memcpy(&color_value, color_data.get(), 4);
    color_value = bepaald::swap_endian(color_value);
    //Logger::message("GOT COLOR NUMBER VALUE: ", color_value);

    ProtoBufParser<protobuffer::optional::INT32> single_color;
    single_color.addField<1>(color_value);

    ProtoBufParser<
      ProtoBufParser<protobuffer::optional::INT32>,
      ProtoBufParser<protobuffer::optional::FLOAT, protobuffer::repeated::INT32, protobuffer::repeated::FLOAT>,
      ProtoBufParser<protobuffer::optional::STRING>,
      protobuffer::optional::FLOAT> color_proto;
    color_proto.addField<1>(single_color);

    //Logger::message(color_proto.getDataString());
    //color_proto.print();

    std::pair<unsigned char *, size_t> color_bindata(color_proto.data(), color_proto.size());

    if (!d_database.exec("UPDATE recipient SET chat_colors = ? WHERE _id = ?", {color_bindata, rid}) ||
        d_database.changed() != 1)
      Logger::warning("Failed to set custom chat color for recipient ", rid);
    else
      ok |= true;
  }

  return ok;
}
