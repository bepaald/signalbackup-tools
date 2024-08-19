/*
  Copyright (C) 2021-2024  Selwin van Dijk

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

#ifndef MIMETYPES_H_
#define MIMETYPES_H_

#include <map>
#include <string_view>
#include <string>

class MimeTypes
{
  static std::map<std::string_view, std::string_view> const s_mimetypemap;
 public:
  inline static std::string_view getExtension(std::string const &mime, std::string const &def = std::string());
};

inline std::string_view MimeTypes::getExtension(std::string const &mime, std::string const &def) // static
{
  if (s_mimetypemap.find(mime.c_str()) != s_mimetypemap.end())
    return s_mimetypemap.at(mime.c_str());
  return def;
}

#endif
