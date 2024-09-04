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

#if !defined(_WIN32) && !defined(__MINGW64__) && (!defined(__APPLE__) || !defined(__MACH__))

#if defined WITHOUT_DBUS

#include "desktopdatabase.ih"

void DesktopDatabase::getSecrets_linux_SecretService(std::set<std::string> */*secrets*/) const
{
  Logger::error       ("Found encrypted sqlcipher key in config file. To decrypt this key");
  Logger::error_indent("a secret must be retrieved from your keyring through dbus, but");
  Logger::error_indent("this program was explicitly compiled without dbus.");
  return;
}

#else

#include "desktopdatabase.ih"
#include "../dbuscon/dbuscon.h"

void DesktopDatabase::getSecrets_linux_SecretService(std::set<std::string> *secrets) const
{
  if (d_dbus_verbose) [[unlikely]]
    Logger::message("Getting secret from SecretService through DBUS");

  if (!secrets)
    return;

  DBusCon dbuscon(d_dbus_verbose);
  if (!dbuscon.ok())
  {
    Logger::error("Failed to connect to dbus session");
    return;
  }

  /* OPEN SESSION */
  if (d_dbus_verbose) Logger::message("[OpenSession]");
  dbuscon.callMethod("org.freedesktop.secrets",
                     "/org/freedesktop/secrets",
                     "org.freedesktop.Secret.Service",
                     "OpenSession",
                     {"plain",
                      DBusVariant{""}});
  std::string session_objectpath = dbuscon.get<std::string>("vo", 1);
  if (session_objectpath.empty())
  {
    Logger::error("Failed to get session");
    return;
  }
  if (d_dbus_verbose) Logger::message(" *** Session: ", session_objectpath);

  // if constexpr (false)
  // {
  //   /* SEARCHITEMS */
  //   // note searching is of no use on KDE, the secret does not seem to have any attributes set.
  //   // so lets just get all items and inspect them
  //   Logger::message("[SearchItems(label:Chromium Keys/Chromium Safe Storage)]");
  //   dbuscon.callMethod("org.freedesktop.secrets",
  //                      "/org/freedesktop/secrets",
  //                      "org.freedesktop.Secret.Service",
  //                      "SearchItems",
  //                      {DBusDict{{"org.freedesktop.Secret.Collection.Label", "Chromium Keys/Chromium Safe Storage"},
  //                                {"Label", "Chromium Keys/Chromium Safe Storage"}}});
  // }

  // if constexpr (false)
  // {
  //   /* GET DEFAULT COLLECTION */
  //   // not necessary we can address the default directly (without knowing what it points to), through
  //   // the aliases/default path...
  //   Logger::message("[ReadAlias(default)]");
  //   dbuscon.callMethod("org.freedesktop.secrets",
  //                      "/org/freedesktop/secrets",
  //                      "org.freedesktop.Secret.Service",
  //                      "ReadAlias",
  //                      {"default"});
  // }

  /* UNLOCK THE DEFAULT COLLECTION */
  if (d_dbus_verbose) Logger::message("[Unlock]");
  dbuscon.callMethod("org.freedesktop.secrets",
                     "/org/freedesktop/secrets",
                     "org.freedesktop.Secret.Service",
                     "Unlock",
                     std::vector<DBusArg>{DBusArray{DBusObjectPath{"/org/freedesktop/secrets/aliases/default"}}});
  // This returns an array of already unlocked object paths (out of the input ones) and a prompt to unlock any locked ones.
  // if no collections need unlocking, the prompt is '/';
  std::string prompt = dbuscon.get<std::string>("aoo", 1);
  if (prompt.empty())
  {
    Logger::error("Error getting prompt");
    return;
  }
  if (d_dbus_verbose) Logger::message(" *** Prompt: ", prompt);

  bool unlocked_by_us = false;
  if (prompt != "/")
  {
    /* REGISTER FOR SIGNAL */
    if (!dbuscon.matchSignal("member='Completed'"))
      Logger::message("WARN: Failed to register for prompt signal");

    /* PROMPT FOR UNLOCK */
    if (d_dbus_verbose) Logger::message("[Prompt]");
    dbuscon.callMethod("org.freedesktop.secrets",
                       prompt.c_str(),
                       "org.freedesktop.Secret.Prompt",
                       "Prompt",
                       {""}); // 'Platform specific window handle to use for showing the prompt.'

    /* WAIT FOR PROMPT COMPLETED SIGNAL */
    // note, we will not even check the signal contents (dismissed/result), since we check if we're
    // unlocked next anyway...
    if (!dbuscon.waitSignal(20, 2500, "org.freedesktop.Secret.Prompt", "Completed"))
      if (d_dbus_verbose) Logger::error("Failed to wait for unlock prompt...");

    unlocked_by_us = true;
  }

  /* CHECK COLLECTION IS UNLOCKED NOW */
  dbuscon.callMethod("org.freedesktop.secrets",
                     "/org/freedesktop/secrets/aliases/default",
                     "org.freedesktop.DBus.Properties",
                     "Get",
                     {"org.freedesktop.Secret.Collection", "Locked"});
  bool islocked = dbuscon.get<bool>("v", 0, true);
  if (islocked)
  {
    Logger::error("Failed to unlock collection");
    return;
  }

  /* GET ITEMS */
  if (d_dbus_verbose) Logger::message("[GetItems]");
  dbuscon.callMethod("org.freedesktop.secrets",
                     "/org/freedesktop/secrets/aliases/default",
                     "org.freedesktop.DBus.Properties",
                     "Get",
                     {"org.freedesktop.Secret.Collection", "Items"});
  std::vector<std::string> items = dbuscon.get<std::vector<std::string>>("v", 0);
  if (items.empty())
  {
    Logger::error("Failed to get any items");
    return;
  }
  else
    if (d_dbus_verbose) Logger::message("Got ", items.size(), " items to check");

  for (auto const &item : items)
  {
    // check label
    dbuscon.callMethod("org.freedesktop.secrets",
                       item.c_str(),
                       "org.freedesktop.DBus.Properties",
                       "Get",
                       {"org.freedesktop.Secret.Item", "Label"});
    std::string label = dbuscon.get<std::string>("v", 0);
    if (d_dbus_verbose) Logger::message(" *** Label: ", label);

#if __cpp_lib_string_contains >= 202011L
    if ((label.contains("Chrome") || label.contains("Chromium")) &&
        (label.contains("Safe Storage") || label.contains("Keys")) &&
        (!label.contains("Control")))
#else
    if ((label.find("Chrome") != std::string::npos || label.find("Chromium") != std::string::npos) &&
        (label.find("Safe Storage") != std::string::npos || label.find("Keys") != std::string::npos) &&
        (label.find("Control") == std::string::npos))
#endif
    {
      /* GET SECRETS */
      if (d_dbus_verbose) Logger::message("[GetSecret]");
      dbuscon.callMethod("org.freedesktop.secrets",
                         item,
                         "org.freedesktop.Secret.Item",
                         "GetSecret",
                         {DBusObjectPath{session_objectpath}});
      /*
        The secret returned by SecretService is a struct:

          struct Secret {
            ObjectPath session ;
            Array<Byte> parameters ;
            Array<Byte> value ;
            String content_type ;
          };

        A struct has signature (oayays), the brackets meaning 'struct'. we want the 'value' (the second ay);
      */
      std::vector<unsigned char> secret_bytes = dbuscon.get<std::vector<unsigned char>>("(oayays)", {0, 2});

      // Since the secret is always 16 bytes, in base64 encoding,
      // its length must be [16/3]*4 + two '=' padding.
      if (secret_bytes.size() != 24 ||
          secret_bytes[23] != '=' ||
          secret_bytes[22] != '=')
      {
        if (d_dbus_verbose) [[unlikely]] Logger::message("Retrieved data is not a valid secret");
        continue;
      }

      if (d_dbus_verbose) [[unlikely]]
        Logger::message(" *** SECRET: ", Logger::VECTOR(secret_bytes));

      secrets->emplace(std::string{secret_bytes.begin(), secret_bytes.end()});
    }
  }

  /* LOCK COLLECTION */
  if (unlocked_by_us)
  {
    if (d_dbus_verbose) Logger::message("[Lock]");
    dbuscon.callMethod("org.freedesktop.secrets",
                       "/org/freedesktop/secrets",
                       "org.freedesktop.Secret.Service",
                       "Lock",
                       std::vector<DBusArg>{DBusArray{DBusObjectPath{"/org/freedesktop/secrets/aliases/default"}}});
  }

  /* CLOSE SESSION */
  if (d_dbus_verbose) Logger::message("[Close]");
  dbuscon.callMethod("org.freedesktop.secrets",
                     session_objectpath.c_str(), //"/org/freedesktop/secrets",
                     "org.freedesktop.Secret.Session",
                     "Close");

}

#endif

#endif
