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

#if !defined (_WIN32) && !defined(__MINGW64__)

#include "desktopdatabase.ih"

#include "../common_bytes.h"

#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/err.h>

std::string DesktopDatabase::decryptKey_linux_mac(std::string const &secret, std::string const &encryptedkeystr, bool last) const
{
  std::string decryptedkey;

  //// 1. derive decryption key from secret:
  // set the salt
  uint64_t const salt_length = 9;
  unsigned char salt[salt_length] = {'s', 'a', 'l', 't', 'y', 's', 'a', 'l', 't'};

  // perform the KDF
  uint64_t key_length = 16;
  std::unique_ptr<unsigned char[]> key(new unsigned char[key_length]);
#if defined (__APPLE__) && defined (__MACH__)
  int iterations = 1003;
#else // linux
  int iterations = 1;
#endif
  if (PKCS5_PBKDF2_HMAC_SHA1(secret.data(), secret.size(), salt, salt_length, iterations, key_length, key.get()) != 1)
  {
    if (last)
      Logger::error("Error deriving key from password. No more secrets to try.");
    else
      Logger::warning("Error deriving key from password. Trying next secret...");
    return decryptedkey;
  }


  //// 2. decrypt keydata using key(1)
  // set encrypted key data
  uint64_t data_length = encryptedkeystr.size() / 2;
  std::unique_ptr<unsigned char[]> data(new unsigned char[data_length]);
  bepaald::hexStringToBytes(encryptedkeystr, data.get(), data_length);
  // check header
  int const version_header_length = 3;
#if defined (__APPLE__) && defined (__MACH__)
  unsigned char const version_header[version_header_length] = {'v', '1', '0'};
#else // linux
  unsigned char const version_header[version_header_length] = {'v', '1', '1'};
#endif
  if (std::memcmp(data.get(), version_header, 3) != 0) [[unlikely]]
    Logger::warning("Unexpected header value: ", bepaald::bytesToHexString(data.get(), 3));

  // set iv
  uint64_t const iv_length = 16;
  unsigned char const iv[iv_length] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}; // 16 spaces
  // init cipher and context
  std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), &::EVP_CIPHER_CTX_free);
  if (!ctx)
  {
    if (last)
      Logger::error("Failed to create decryption context. No more secrets to try.");
    else
      Logger::warning("Failed to create decryption context. Trying next secret...");
    return decryptedkey;
  }

  // init decrypt
  if (EVP_DecryptInit_ex(ctx.get(), EVP_aes_128_cbc(), nullptr, key.get(), iv) != 1) [[unlikely]]
  {
    if (last)
      Logger::error("Failed to initialize decryption operation. No more secrets to try.");
    else
      Logger::warning("Failed to initialize decryption operation. Trying next secret...");
    return decryptedkey;
  }

  // decrypt update
  int out_len = 0;
  int output_length = data_length - version_header_length;
  std::unique_ptr<unsigned char[]> output(new unsigned char[output_length]);
  if (EVP_DecryptUpdate(ctx.get(), output.get(), &out_len, data.get() + version_header_length, output_length) != 1)
  {
    if (last)
      Logger::error("Decrypt update");
    else
      Logger::error("Decrypt update. Trying next secret...");
    return decryptedkey;
  }

  // decrypt final
  int tail_len = 0;
  if (EVP_DecryptFinal_ex(ctx.get(), output.get() + out_len, &tail_len) != 1)
  {
    if (last)
      Logger::error("Finalizing desktop key decryption. No more secrets to try.");
    else if (d_verbose) [[unlikely]]
      Logger::warning("Finalizing desktop key decryption. Trying next secret...");
    return decryptedkey;
  }
  out_len += tail_len;

  decryptedkey = bepaald::bytesToPrintableString(output.get(), out_len);

  // this is probably not necessary at this point, but it cant hurt
  if (decryptedkey.find_first_not_of("abcdefghijklmnopqrstuvwxyz0123456789") != std::string::npos)
  {
    if (last)
      Logger::error("Failed to decrypt key correctly. No more secrets to try.");
    else
      Logger::warning("Failed to decrypt key correctly, attempting next secret...");
    decryptedkey.clear();
    //return empty string...
  }

  return decryptedkey;
}

#endif

/*
  (spaces added in output before the padding)

[~] $ echo -ne "exactly 32 bytes exactly 32 byte" > input.txt ; openssl enc -aes-128-cbc -nosalt -e -in input.txt -K '2222233333232323' -iv '5a04ec902686fb05a6b7a338b6e07760' > output.txt ; openssl enc -nopad -aes-128-cbc -nosalt -d -in output.txt -K '2222233333232323' -iv '5a04ec902686fb05a6b7a338b6e07760' | xxd -ps -g 1 -c 64
65786163746c792033322062797465732065786163746c792033322062797465 10101010101010101010101010101010
[~] $ echo -ne "exactly 33 bytes exactly 33 bytes" > input.txt ; openssl enc -aes-128-cbc -nosalt -e -in input.txt -K '2222233333232323' -iv '5a04ec902686fb05a6b7a338b6e07760' > output.txt ; openssl enc -nopad -aes-128-cbc -nosalt -d -in output.txt -K '2222233333232323' -iv '5a04ec902686fb05a6b7a338b6e07760' | xxd -ps -g 1 -c 64
65786163746c792033332062797465732065786163746c79203333206279746573 0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f
[~] $ echo -ne "exactly 34 bytes exactly 34 bytes " > input.txt ; openssl enc -aes-128-cbc -nosalt -e -in input.txt -K '2222233333232323' -iv '5a04ec902686fb05a6b7a338b6e07760' > output.txt ; openssl enc -nopad -aes-128-cbc -nosalt -d -in output.txt -K '2222233333232323' -iv '5a04ec902686fb05a6b7a338b6e07760' | xxd -ps -g 1 -c 64
65786163746c792033342062797465732065786163746c7920333420627974657320 0e0e0e0e0e0e0e0e0e0e0e0e0e0e
[~] $ echo -ne "exactly 35 bytes exactly 35 bytes e" > input.txt ; openssl enc -aes-128-cbc -nosalt -e -in input.txt -K '2222233333232323' -iv '5a04ec902686fb05a6b7a338b6e07760' > output.txt ; openssl enc -nopad -aes-128-cbc -nosalt -d -in output.txt -K '2222233333232323' -iv '5a04ec902686fb05a6b7a338b6e07760' | xxd -ps -g 1 -c 64
65786163746c792033352062797465732065786163746c792033352062797465732065 0d0d0d0d0d0d0d0d0d0d0d0d0d
[...]

[~] $ echo -ne "exactly 46 bytes exactly 46 bytes exactly 46 b" > input.txt ; openssl enc -aes-128-cbc -nosalt -e -in input.txt -K '2222233333232323' -iv '5a04ec902686fb05a6b7a338b6e07760' > output.txt ; openssl enc -nopad -aes-128-cbc -nosalt -d -in output.txt -K '2222233333232323' -iv '5a04ec902686fb05a6b7a338b6e07760' | xxd -ps -g 1 -c 64
65786163746c792034362062797465732065786163746c792034362062797465732065786163746c792034362062 0202
[~] $ echo -ne "exactly 47 bytes exactly 47 bytes exactly 47 by" > input.txt ; openssl enc -aes-128-cbc -nosalt -e -in input.txt -K '2222233333232323' -iv '5a04ec902686fb05a6b7a338b6e07760' > output.txt ; openssl enc -nopad -aes-128-cbc -nosalt -d -in output.txt -K '2222233333232323' -iv '5a04ec902686fb05a6b7a338b6e07760' | xxd -ps -g 1 -c 64
65786163746c792034372062797465732065786163746c792034372062797465732065786163746c79203437206279 01
[~] $ echo -ne "exactly 48 bytes exactly 48 bytes exactly 48 byt" > input.txt ; openssl enc -aes-128-cbc -nosalt -e -in input.txt -K '2222233333232323' -iv '5a04ec902686fb05a6b7a338b6e07760' > output.txt ; openssl enc -nopad -aes-128-cbc -nosalt -d -in output.txt -K '2222233333232323' -iv '5a04ec902686fb05a6b7a338b6e07760' | xxd -ps -g 1 -c 64
65786163746c792034382062797465732065786163746c792034382062797465732065786163746c7920343820627974 10101010101010101010101010101010

*/
