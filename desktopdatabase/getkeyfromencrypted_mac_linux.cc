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

#if !defined(__WIN32) && !defined(__MINGW64__)

#include "desktopdatabase.ih"

bool DesktopDatabase::getKeyFromEncrypted_mac_linux()
{
  // 1. get the encrypted key from config.json
  std::string keystr = readEncryptedKey();
  if (keystr.empty())
    return false;

  // 2. get the secrets
  std::set<std::string> secrets;
  auto tryDecrypt = [&]()
  {
    for (auto s = secrets.begin(); s != secrets.end(); ++s)
    {
      d_hexkey = decryptKey_linux_mac(*s, keystr, s == std::prev(secrets.end()));
      if (!d_hexkey.empty())
        return true;
    }
    return false;
  };
#if defined(__APPLE__) && defined(__MACH__)
  bool beta = d_databasedir.contains("Signal Beta");
  getSecrets_mac(&secrets, beta);
  if (tryDecrypt())
    return true;
  else
  {
    Logger::warning("tryDecrypt() failed with initial secret, trying for Signal ", beta ? "non-" : "", "Beta...");
    secrets.clear();
    getSecrets_mac(&secrets, !beta);
    if (tryDecrypt())
      return true;
  }
#else
  getSecrets_linux_SecretService(&secrets);
  if (tryDecrypt())
    return true;
  getSecrets_linux_Kwallet(6, &secrets); // nothing from libsecret, try kwallet6...
  if (tryDecrypt())
    return true;
  getSecrets_linux_Kwallet(5, &secrets); // nothing from kwallet6, try kwallet5...
  if (tryDecrypt())
    return true;
#endif
  if (secrets.empty())
  {
    Logger::error("Failed to get any secrets");
    return false;
  }

  if (d_hexkey.empty())
  {
    Logger::error("Failed to decrypt valid key. :(");
    return false;
  }

  return false;
}

#endif
