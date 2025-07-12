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

#if defined(__APPLE__) && defined(__MACH__)

/*
$ security dump-keychain:

keychain: "/Users/username/Library/Keychains/login.keychain-db"
version: 512
class: "genp"                                                       <--- class: general password
attributes:
    0x00000007 <blob>="Signal Safe Storage"
    0x00000008 <blob>=<NULL>
    "acct"<blob>="Signal Key"                                       <--- account: Signal Key
    "cdat"<timedate>=0x32303234303831353134333230395A00  "20240815143209Z\000"
    "crtr"<uint32>="aapl"
    "cusi"<sint32>=<NULL>
    "desc"<blob>=<NULL>
    "gena"<blob>=<NULL>
    "icmt"<blob>=<NULL>
    "invi"<sint32>=<NULL>
    "mdat"<timedate>=0x32303234303831353134333230395A00  "20240815143209Z\000"
    "nega"<sint32>=<NULL>
    "prot"<blob>=<NULL>
    "scrp"<sint32>=<NULL>
    "svce"<blob>="Signal Safe Storage"                              <--- service: Signal Safe Storage
    "type"<uint32>=<NULL>
*/

#include "desktopdatabase.ih"
#include <Security/Security.h>

void DesktopDatabase::getSecrets_mac(std::set<std::string> *secrets, bool beta) const
{
  // create query to search the keychain:
  int const dict_size = 3; // 4 if using account as well;
  void const *keys[dict_size] = {kSecClass,
				 //kSecAttrAccount,
				 kSecAttrService,
				 kSecReturnData};
  //CFStringRef account = CFStringCreateWithCString(nullptr, beta ? "Signal Beta" : "Signal Key", kCFStringEncodingUTF8);
  CFStringRef service = CFStringCreateWithCString(nullptr, beta ? "Signal Beta Safe Storage" : "Signal Safe Storage", kCFStringEncodingUTF8);
  void const *values[dict_size] = {kSecClassGenericPassword,
				   //account,
				   service,
				   kCFBooleanTrue};
  CFDictionaryRef query = CFDictionaryCreate(nullptr, keys, values, dict_size, nullptr, nullptr);

  // do the search
  CFDataRef item_data = nullptr;
  OSStatus ret = SecItemCopyMatching(query, reinterpret_cast<CFTypeRef *>(&item_data));

  // clean up
  CFRelease(query);
  //CFRelease(account);
  CFRelease(service);

  if (ret != 0) // error
  {
    CFStringRef errmsg_ref = SecCopyErrorMessageString(ret, nullptr);
    CFIndex length = CFStringGetLength(errmsg_ref);
    CFIndex max_length = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8) + 1;
    std::unique_ptr<char[]> error_string(new char[max_length]);
    if (CFStringGetCString(errmsg_ref, error_string.get(), max_length, kCFStringEncodingUTF8))
      Logger::error(error_string.get());
    else
      Logger::error("Unknown error searching keychain");
    return;
  }

  // parse returned items...
  int secret_length = CFDataGetLength(item_data);
  if (secret_length != 24)
  {
    Logger::warning("Unexpected secret_length (was ", secret_length, ", expected 24)");
    return;
  }
  //std::string secret(reinterpret_cast<char const *>(CFDataGetBytePtr(item_data)), secret_length);
  //secrets->emplace(secret);
  secrets->emplace(std::string(reinterpret_cast<char const *>(CFDataGetBytePtr(item_data)), secret_length));
}

#endif
