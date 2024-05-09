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

#include <cmath>
#if __cpp_lib_math_constants >= 201907L
#include <numbers>
#endif

std::pair<std::string, std::string> SignalBackup::getCustomColor(std::pair<std::shared_ptr<unsigned char []>, size_t> const &colordata) const
{
  std::pair<std::string, std::string> custom_colors;

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

  ProtoBufParser<
    ProtoBufParser<protobuffer::optional::INT32>,
    ProtoBufParser<protobuffer::optional::FLOAT, protobuffer::repeated::INT32, protobuffer::repeated::FLOAT>,
    ProtoBufParser<protobuffer::optional::STRING>,
    protobuffer::optional::FLOAT> color_proto(colordata.first.get(), colordata.second);

  if (color_proto.getField<1>().has_value() &&
      color_proto.getField<1>().value().getField<1>().has_value())
  {
    uint32_t c = color_proto.getField<1>().value().getField<1>().value();

    uint8_t r = (c >> (2 * 8)) & 0xFF;
    uint8_t g = (c >> (1 * 8)) & 0xFF;
    uint8_t b = (c >> (0 * 8)) & 0xFF;

    std::stringstream ss;
    ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(r)
       << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(g)
       << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b);
    custom_colors.first = ss.str();
    //std::cout << custom_colors.first << std::endl;

    if (color_proto.getField<4>().has_value())
    {
      double dimfactor = 1 - color_proto.getField<4>().value();

      // to HSV
      double maxrgb = std::max(r, std::max(g, b));
      double minrgb = std::min(r, std::min(g, b));

      double v = maxrgb / 255;
      double s = (maxrgb > 0) ? 1 - minrgb / maxrgb : 0;

#if __cpp_lib_math_constants >= 201907L
      double h = std::acos((r - 0.5 * g - 0.5 * b) / std::sqrt(r * r + g * g + b * b - r * g - r * b - g * b)) * 180 / std::numbers::pi;
#else
      double h = std::acos((r - 0.5 * g - 0.5 * b) / std::sqrt(r * r + g * g + b * b - r * g - r * b - g * b)) * 180 / 3.14159265;
#endif
      if (b > g)
        h = 360 - h;

      // apply dimming
      v *= dimfactor;

      // back to rgb:
      maxrgb = 255 * v;
      minrgb = maxrgb * (1 - s);

      double z = (maxrgb - minrgb) * (1 - std::abs(std::fmod((h / 60), 2) - 1));

      std::stringstream ss2;
      if (h < 60)
        ss2 << std::hex << std::setfill('0') << std::setw(2) << std::lround(maxrgb) // r
            << std::hex << std::setfill('0') << std::setw(2) << std::lround(z + minrgb) // g
            << std::hex << std::setfill('0') << std::setw(2) << std::lround(minrgb); // b
      else if (h < 120)
        ss2 << std::hex << std::setfill('0') << std::setw(2) << std::lround(z + minrgb) // r
            << std::hex << std::setfill('0') << std::setw(2) << std::lround(maxrgb) // g
            << std::hex << std::setfill('0') << std::setw(2) << std::lround(minrgb); // b
      else if (h < 180)
        ss2 << std::hex << std::setfill('0') << std::setw(2) << std::lround(minrgb) // r
            << std::hex << std::setfill('0') << std::setw(2) << std::lround(maxrgb) // g
            << std::hex << std::setfill('0') << std::setw(2) << std::lround(z + minrgb); // b
      else if (h < 240)
        ss2 << std::hex << std::setfill('0') << std::setw(2) << std::lround(minrgb) // r
            << std::hex << std::setfill('0') << std::setw(2) << std::lround(z + minrgb) // g
            << std::hex << std::setfill('0') << std::setw(2) << std::lround(maxrgb); // b
      else if (h < 300)
        ss2 << std::hex << std::setfill('0') << std::setw(2) << std::lround(z + minrgb) // r
            << std::hex << std::setfill('0') << std::setw(2) << std::lround(minrgb) // g
            << std::hex << std::setfill('0') << std::setw(2) << std::lround(maxrgb); // b
      else //if (h < 360)
        ss2 << std::hex << std::setfill('0') << std::setw(2) << std::lround(maxrgb) // r
            << std::hex << std::setfill('0') << std::setw(2) << std::lround(minrgb) // g
            << std::hex << std::setfill('0') << std::setw(2) << std::lround(z + minrgb); // b
      custom_colors.second = ss2.str();
      //std::cout << custom_colors.second << std::endl;
    }
  }
  return custom_colors;
}
