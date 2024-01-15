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

#include "../../signalbackup/signalbackup.h"

int main()
{
  SignalBackup sb("DEVsignal-2023-12-31-11-13-42.backup", "000000000000000000000000000000", /*verbose*/ false, /*progress*/false,
                  /*replaceattachments*/ false,
                  /*assumebadframesizeonbadmac*/ false, /*editattachmentsize*/ std::vector<long long int>(),
                  /*stoponerror*/ true, /*fulldecode*/ false);

  sb.runQuery("SELECT COUNT(*) FROM message", false);
  sb.runQuery("SELECT COUNT(*) FROM recipient", false);

  {
    std::cout << "Copying database" << std::endl;
    SignalBackup sb2(sb);
    std::cout << "Assigning database" << std::endl;
    SignalBackup sb3 = sb;

    sb2.runQuery("DELETE FROM message", false);
    sb2.runQuery("SELECT COUNT(*) FROM message", false);
    sb2.runQuery("SELECT COUNT(*) FROM recipient", false);

    sb3.runQuery("DELETE FROM recipient", false);
    sb3.runQuery("SELECT COUNT(*) FROM message", false);
    sb3.runQuery("SELECT COUNT(*) FROM recipient", false);

    std::cout << "Move assigning" << std::endl;
    sb3 = std::move(sb2);

    sb3.runQuery("DELETE FROM recipient", false);
    sb3.runQuery("SELECT COUNT(*) FROM message", false);
    sb3.runQuery("SELECT COUNT(*) FROM recipient", false);

    std::cout << "Move constructing" << std::endl;
    SignalBackup sb4(std::move(sb3));
    sb4.runQuery("SELECT COUNT(*) FROM message", false);
    sb4.runQuery("SELECT COUNT(*) FROM recipient", false);

  }
  sb.runQuery("SELECT COUNT(*) FROM message", false);
  sb.runQuery("SELECT COUNT(*) FROM recipient", false);

  std::cout << "SIGNALBACKUP COPY TESTS PASSED" << std::endl;
  return 0;
}
