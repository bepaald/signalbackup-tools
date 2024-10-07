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

#ifndef DESKTOPDATABASE_H_
#define DESKTOPDATABASE_H_

#include <cstdlib>
#include <memory>
#include <set>

#include "../logger/logger.h"
#include "../common_filesystem.h"
#include "../memsqlitedb/memsqlitedb.h"
#include "../sqlcipherdecryptor/sqlcipherdecryptor.h"

class DesktopDatabase
{
  std::unique_ptr<SqlCipherDecryptor> d_cipherdb;
  MemSqliteDB d_database;
  std::string d_configdir;
  std::string d_databasedir;
  std::string d_hexkey;
  bool d_ok;
  bool d_verbose;
  bool d_dbus_verbose;
  bool d_ignorewal;
  long long int d_cipherversion;
  bool d_truncate;
  bool d_showkey;
 public:
  inline DesktopDatabase(std::string const &hexkey, bool verbose, bool ignorewal, long long int cipherversion,
                         bool truncate, bool showkey, bool dbus_verbose);
  inline DesktopDatabase(std::string const &configdir, std::string const &databasedir, std::string const &hexkey,
                         bool verbose, bool ignorewal, long long int cipherversion, bool truncate, bool showkey,
                         bool dbus_verbose);
  DesktopDatabase(DesktopDatabase const &other) = delete;
  DesktopDatabase(DesktopDatabase &&other) = delete;
  DesktopDatabase &operator=(DesktopDatabase const &other) = delete;
  DesktopDatabase &operator=(DesktopDatabase &&other) = delete;
  inline bool ok() const;
  inline bool dumpDb(std::string const &file, bool overwrite) const;
  inline std::string const &getConfigDir() const;
  inline std::string const &getDatabaseDir() const;
  inline void runQuery(std::string const &q, bool pretty = true) const;

 private:
  bool init();
  inline std::pair<std::string, std::string> getDesktopDir() const;
  std::string readEncryptedKey() const;
  bool getKey();
  bool getKeyFromEncrypted();
#if defined(_WIN32) || defined(__MINGW64__)
  bool getKeyFromEncrypted_win();
#else
  bool getKeyFromEncrypted_mac_linux();
  std::string decryptKey_linux_mac(std::string const &secret, std::string const &encryptedkeystr, bool last = true) const;
#endif
#if defined(__APPLE__) && defined(__MACH__) // if apple...
  void getSecrets_mac(std::set<std::string> *secrets) const;
#elif !defined(_WIN32) && !defined(__MINGW64__) // not apple, but also not windows
  void getSecrets_linux_SecretService(std::set<std::string> *secrets) const;
  void getSecrets_linux_Kwallet(int version, std::set<std::string> *secrets) const;
#endif

  friend class SignalBackup;
  friend class DummyBackup;
};

inline DesktopDatabase::DesktopDatabase(std::string const &hexkey, bool verbose, bool ignorewal,
                                        long long int cipherversion, bool truncate, bool showkey,
                                        bool dbus_verbose)
  :
  DesktopDatabase(std::string(), std::string(), hexkey, verbose, ignorewal, cipherversion, truncate, showkey, dbus_verbose)
{}

inline DesktopDatabase::DesktopDatabase(std::string const &configdir, std::string const &databasedir,
                                        std::string const &hexkey, bool verbose, bool ignorewal,
                                        long long int cipherversion, bool truncate, bool showkey,
                                        bool dbus_verbose)
  :
  d_configdir(configdir),
  d_databasedir(databasedir),
  d_hexkey(hexkey),
  d_ok(false),
  d_verbose(verbose),
  d_dbus_verbose(dbus_verbose),
  d_ignorewal(ignorewal),
  d_cipherversion(cipherversion),
  d_truncate(truncate),
  d_showkey(showkey)
{
  d_ok = init();
}

inline bool DesktopDatabase::ok() const
{
  return d_ok;
}

inline std::pair<std::string, std::string> DesktopDatabase::getDesktopDir() const
{
#if defined(_WIN32) || defined(__MINGW64__)
  // Windows: concatenate HOMEDRIVE+HOMEPATH
  // probably only works on windows 7 and newer? (if at all)
  const char *homedrive_cs = std::getenv("HOMEDRIVE");
  const char *homepath_cs = std::getenv("HOMEPATH");
  if (homedrive_cs == nullptr || homepath_cs == nullptr)
    return {std::string(), std::string()};
  std::string home = std::string(homedrive_cs) + std::string(homepath_cs);
  if (home.empty())
    return {std::string(), std::string()};

  if (bepaald::isDir(home + "/AppData/Roaming/Signal"))
    return {home + "/AppData/Roaming/Signal", home + "/AppData/Roaming/Signal"};
  else if (bepaald::isDir(home + "/AppData/Roaming/Signal Beta"))
    return {home + "/AppData/Roaming/Signal Beta", home + "/AppData/Roaming/Signal Beta"};
  else
    return {std::string(), std::string()};
#else
  char const *homedir_cs = std::getenv("HOME");
  if (homedir_cs == nullptr)
    return {std::string(), std::string()};
  std::string homedir(homedir_cs);
  if (homedir.empty())
    return {std::string(), std::string()};
#if defined(__APPLE__) && defined(__MACH__)
  if (bepaald::isDir(homedir + "/Library/Application Support/Signal"))
    return {homedir + "/Library/Application Support/Signal", homedir + "/Library/Application Support/Signal"};
  if (bepaald::isDir(homedir + "/Library/Application Support/Signal Beta"))
    return {homedir + "/Library/Application Support/Signal Beta", homedir + "/Library/Application Support/Signal Beta"};
  else
    return {std::string(), std::string()};
#else // !windows && !mac
  if (bepaald::isDir(homedir + "/.config/Signal"))
    return {homedir + "/.config/Signal", homedir + "/.config/Signal"};
  if (bepaald::isDir(homedir + "/.config/Signal Beta"))
    return {homedir + "/.config/Signal Beta", homedir + "/.config/Signal Beta"};
  else
    return {std::string(), std::string()};
#endif
#endif
}

inline bool DesktopDatabase::dumpDb(std::string const &file, bool overwrite) const
{
  if (bepaald::fileOrDirExists(file) && !overwrite)
  {
    Logger::message("File '", file, "' exists, use `--overwrite` to overwrite.");
    return false;
  }

  if (!d_database.saveToFile(file))
  {
    Logger::error("Failed to save Signal Desktop sql database to file '", file, "'");
    return false;
  }
  return true;
}

inline std::string const &DesktopDatabase::getConfigDir() const
{
  return d_configdir;
}

inline std::string const &DesktopDatabase::getDatabaseDir() const
{
  return d_databasedir;
}

inline void DesktopDatabase::runQuery(std::string const &q, bool pretty) const
{
  Logger::message(" * Executing query: ", q);
  SqliteDB::QueryResults res;
  if (!d_database.exec(q, &res))
    return;

  std::string q_comm(q.substr(0, STRLEN("DELETE"))); // delete, insert and update are same length...
  std::for_each(q_comm.begin(), q_comm.end(), [] (char &ch) { ch = std::toupper(ch); });

  if (q_comm == "DELETE" || q_comm == "INSERT" || q_comm == "UPDATE")
  {
    Logger::message("Modified ", d_database.changed(), " rows");
    if (res.rows() == 0 && res.columns() == 0)
      return;
  }

  if (pretty)
    res.prettyPrint(d_truncate);
  else
    res.print();
}

#endif
