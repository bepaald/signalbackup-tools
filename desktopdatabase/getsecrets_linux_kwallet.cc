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

#if !defined(_WIN32) && !defined(__MINGW64__) && (!defined(__APPLE__) || !defined(__MACH__))

#if defined WITHOUT_DBUS

#include "desktopdatabase.ih"

void DesktopDatabase::getSecrets_linux_Kwallet(int /*version*/, std::set<std::string> */*secrets*/) const
{
  Logger::error       ("Found encrypted sqlcipher key in config file. To decrypt this key");
  Logger::error_indent("a secret must be retrieved from your keyring through dbus, but");
  Logger::error_indent("this program was explicitly compiled without dbus.");
  return;
}

#else

#include "desktopdatabase.ih"
#include "../dbuscon/dbuscon.h"

void DesktopDatabase::getSecrets_linux_Kwallet(int version, std::set<std::string> *secrets) const
{
  if (d_dbus_verbose) [[unlikely]]
    Logger::message("Getting secret from Kwallet through DBUS (version ", version, ")");

  if (!secrets)
    return;

  DBusCon dbuscon(d_dbus_verbose);
  if (!dbuscon.ok())
  {
    Logger::error("Failed to connect to dbus session");
    return;
  }

  std::string destination("org.kde.kwalletd" + std::to_string(version));
  std::string path("/modules/kwalletd" + std::to_string(version));
  std::string interface("org.kde.KWallet");

  /* GET WALLET */
  if (d_dbus_verbose) Logger::message("[networkWallet]");
  dbuscon.callMethod(destination.c_str(),
                     path.c_str(),
                     interface.c_str(),
                     "networkWallet");
  std::string walletname = dbuscon.get<std::string>("s", 0);
  if (walletname.empty())
  {
    Logger::error("Failed to get wallet name");
    return;
  }
  if (d_dbus_verbose) Logger::message(" *** Wallet name: ", walletname);

  // ON KDE THE 'open' METHOD SEEMS TO BLOCK FOR PASSWORD PROMPT BY ITSELF...
  // /* Register to wait for opening wallet */
  // if (!matchSignal("member='walletOpened'"))
  //   Logger::message("WARN: Failed to register for signal");

  /* OPEN WALLET */
  if (d_dbus_verbose) Logger::message("[open]");
  dbuscon.callMethod(destination.c_str(),
                     path.c_str(),
                     interface.c_str(),
                     "open",
                     {walletname, int64_t{0}, "signalbackup-tools"s});
  int32_t handle = dbuscon.get<int32_t>("i", 0 - 1);
  if (handle < 0)
  {
    Logger::error("Failed to open wallet");
    return;
  }
  if (d_dbus_verbose) Logger::message(" *** Handle: ", handle);



  /* GET FOLDERS */
  if (d_dbus_verbose) Logger::message("[folderList]");
  dbuscon.callMethod(destination.c_str(),
                     path.c_str(),
                     interface.c_str(),
                     "folderList",
                     {handle, "signalbackup-tools"s});
  std::vector<std::string> folders = dbuscon.get<std::vector<std::string>>("as", 0);
  if (folders.empty())
  {
    Logger::error("Failed to get any folders from wallet");
    return;
  }

  for (auto const &folder : folders)
  {
#if __cpp_lib_string_contains >= 202011L
    if ((folder.contains("Chrome") || folder.contains("Chromium")) &&
        (folder.contains("Safe Storage") || folder.contains("Keys")))
#else
    if ((folder.find("Chrome") != std::string::npos || folder.find("Chromium") != std::string::npos) &&
        (folder.find("Safe Storage") != std::string::npos || folder.find("Keys") != std::string::npos))
#endif
    {
      /* GET PASSWORD */
      if (d_dbus_verbose) Logger::message("[passwordList]");
      dbuscon.callMethod(destination.c_str(),
                         path.c_str(),
                         interface.c_str(),
                         "passwordList",
                         {handle, folder, "signalbackup-tools"s});
      /*
        The password list returns a dict (dicts are always (in) an array as per dbus spec)
        the signature is a{sv} -> the v in our case is a string again, pretty much a map<std::string, std::string>,

        The value we want seems to have the key "Chrom[e|ium] Safe Storage"...
      */
      std::map<std::string, std::string> passwordmap = dbuscon.get<std::map<std::string, std::string>>("a{sv}", 0);

      if (passwordmap.empty())
      {
        Logger::error("Failed to get password map");
        return;
      }

      for (auto const &e : passwordmap)
        if (e.first == "Chromium Safe Storage" || e.first == "Chrome Safe Storage")
        {
          if (d_dbus_verbose) [[unlikely]]
            Logger::message(" *** SECRET: ", e.second);
          secrets->insert(e.second);
        }
    }
  }


  /* CLOSE WALLET */
  if (d_dbus_verbose) Logger::message("[close (wallet)]");
  dbuscon.callMethod(destination.c_str(),
                     path.c_str(),
                     interface.c_str(),
                     "close",
                     {walletname, false});

  /* CLOSE SESSION */
  if (d_dbus_verbose) Logger::message("[close (session)]");
  dbuscon.callMethod(destination.c_str(),
                     path.c_str(),
                     interface.c_str(),
                     "close",
                     {handle, false, "signalbackup-tools"s});



  return;
}

#endif

#endif
