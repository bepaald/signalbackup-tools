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

std::string SignalBackup::sanitizeFilename(std::string const &filename, bool aggressive [[maybe_unused]]) const
{
  std::string result;

#if !defined(_WIN32) && !defined(__MINGW64__)

  /*
  // attempt to determine target filesystem
  static std::string filesystem_type;
  if (filesystem_type.empty())
  {
    Logger::message("Attempting to get type of filesystem...");

    filesystem_type = "Unknown";

    Logger::message("Got filesystem type '", filesystem_type, "'");
  }
  */

  if (!aggressive)
  {
    // filter disallowed characters. (Note this is not an exact science)
    for (char c : filename)
      result += ((c == '/' || c == '\0' || c == '\n') ? '_' : c); // newline is technically allowed I think
  }
  else // aggressive sanitizing
  {
#endif // WINDOWS || aggressive_sanitizing = true

  auto icasecmp = [](char a, char b) STATICLAMBDA
  {
    return ((a == b) || (tolower(static_cast<unsigned char>(a)) == tolower(static_cast<unsigned char>(b))));
  };

  std::vector<std::string> const reserved =
    {
      "CON",
      "PRN",
      "AUX",
      "CLOCK$",
      "NUL",
      "COM0", "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
      "LPT0", "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9",
      /* these are ntfs things, best just not allow them.... */
      "$Mft",
      "$MftMirr",
      "$LogFile",
      "$Volume",
      "$AttrDef",
      "$Bitmap",
      "$Boot",
      "$BadClus",
      "$Secure",
      "$Upcase",
      "$Extend",
      "$Quota",
      "$ObjId",
      "$Reparse",
    };

  // filter reserved filenames
  for (auto const &r : reserved)
    if (filename.size() == r.size() &&
        std::equal(filename.begin(), filename.end(),
                   r.begin(), r.end(), icasecmp))
      return "_" + filename;

  // filter disallowed characters. (Note this is not an exact science)
  for (char c : filename)
    result += ((c == '/' || c == '\\' || c == '?' ||
                c == '*' || c == ':' || c == '|' ||
                c == '"' || c == '<' || c == '>' ||
                c <= 0x1f || c == 0x7f) ? '_' : c);

  // trailing whitespace or periods are (possibly) technically allowed
  // by the filesystem, but not supported by windows shell and UI
  while (result.back() == ' ' ||
         result.back() == '.')
    result.pop_back();

#if !defined(_WIN32) && !defined(__MINGW64__)
  }
#endif


  return result;
}
