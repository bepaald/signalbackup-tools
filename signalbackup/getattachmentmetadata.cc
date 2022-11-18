/*
  Copyright (C) 2022  Selwin van Dijk

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

#ifndef USE_CRYPTOPP
#include <openssl/sha.h>
#endif

#include "../base64/base64.h"

SignalBackup::AttachmentMetadata SignalBackup::getAttachmentMetaData(std::string const &file) const
{
  #ifndef USE_CRYPTOPP

  std::ifstream imagefile(file, std::ios_base::binary | std::ios_base::in);
  if (!imagefile.is_open())
  {
    std::cout << "Failed to open image for reading: " << file << std::endl;
          return AttachmentMetadata{-1, -1, std::string(), 0, std::string(), std::string()};
  }

  imagefile.seekg(0, std::ios_base::end);
  unsigned long imagefile_size = imagefile.tellg();
  imagefile.seekg(0, std::ios_base::beg);

  if (imagefile_size == 0)
    return AttachmentMetadata{-1, -1, std::string(), 0, std::string(), std::string()};

  // gethash
  std::string hash;
  int buffer_size = std::max(imagefile_size, 1024 * 1024ul);
  std::unique_ptr<unsigned char[]> buffer(new unsigned char[buffer_size]);
  unsigned char rawhash[SHA256_DIGEST_LENGTH];
  bool fail = true;
  std::unique_ptr<EVP_MD_CTX, decltype(&::EVP_MD_CTX_free)> sha256(EVP_MD_CTX_new(), &::EVP_MD_CTX_free);
  if (sha256.get() && EVP_DigestInit_ex(sha256.get(), EVP_sha256(), nullptr) == 1)
  {
    fail = false;
    while (imagefile)
    {
      imagefile.read(reinterpret_cast<char *>(buffer.get()), buffer_size);
      if (EVP_DigestUpdate(sha256.get(), buffer.get(), imagefile.gcount()) != 1)
      {
        fail |= true;
        break;
      }
    }
    fail |= (EVP_DigestFinal_ex(sha256.get(), rawhash, nullptr) != 1);
  }
  hash = fail ? std::string() : Base64::bytesToBase64String(rawhash, SHA256_DIGEST_LENGTH);
  //std::cout << bepaald::bytesToHexString(rawhash, SHA256_DIGEST_LENGTH) << std::endl;
  //std::cout << "GOT HASH: " << hash << std::endl;

  // rewind and reset
  imagefile.clear();
  imagefile.seekg(0, std::ios_base::beg);
  int bufsize = 24;

  if (imagefile_size < static_cast<unsigned int>(bufsize))
  {
    std::cout << "File unexpectedly small" << std::endl;
    return AttachmentMetadata{-1, -1, std::string(), 0, hash, std::string()};
  }

  std::unique_ptr<unsigned char[]> buf(new unsigned char[bufsize]);
  if (!imagefile.read(reinterpret_cast<char *>(buf.get()), bufsize))
  {
    std::cout << "Failed to read 24 bytes from file" << std::endl;
    return AttachmentMetadata{-1, -1, std::string(), 0, hash, std::string()};
  }

  // PNG: the first frame is by definition an IHDR frame, which gives dimensions
  if (buf[0] == 0x89 && buf[1] == 'P' && buf[2] == 'N' && buf[3] == 'G' && buf[4] == 0x0D && buf[5] == 0x0A && buf[6] == 0x1A && buf[7] == 0x0A &&
      buf[12] == 'I' && buf[13] == 'H' && buf[14] == 'D' && buf[15] == 'R')
  {
    //*x = (buf[16] << 24) + (buf[17] << 16) + (buf[18] << 8) + (buf[19] << 0);
    //*y = (buf[20] << 24) + (buf[21] << 16) + (buf[22] << 8) + (buf[23] << 0);
    return AttachmentMetadata{(buf[16] << 24) + (buf[17] << 16) + (buf[18] << 8) + (buf[19] << 0),
      (buf[20] << 24) + (buf[21] << 16) + (buf[22] << 8) + (buf[23] << 0),
      "image/png", imagefile_size, hash, std::string()};
  }

  // GIF: first three bytes say "GIF", next three give version number. Then dimensions
  if (buf[0] == 'G' && buf[1] == 'I' && buf[2] == 'F')
  {
    //*x = buf[6] + (buf[7] << 8);
    //*y = buf[8] + (buf[9] << 8);
    return AttachmentMetadata{buf[8] + (buf[9] << 8),
      buf[6] + (buf[7] << 8),
      "image/gif", imagefile_size, hash, std::string()};
  }

  // JPEG
  // For jpeg we read the width and height from JPEG header, more precisely, the frame marked 0xFFC0.
  // At buf[4] + buf[5], we find the block length of the first block (which is never the block
  // with size information, so can be skipped.
  // Then, we can just skip to the start of the next block, read 9 bytes and get:
  // 0xFF(*) | 0xC0(**) | ushort length | uchar precision | ushort x | ushort y
  // if first byte is not 0xff, something is wrong
  // if second byte is not 0xc0, this is not our frame and we use 3rd and 4th to skip
  // else bytes 6-9 are the wanted numbers.
  //
  // * Note, apparently, markers can start with any number of 0xff's
  // ** Apparently, frames marked C0-C3 & C9-CB all contain the desired resolution
  //
  // Note, though I think it is required the first frame is a JFIF, or Exif frame,
  // I have images that don't have this. They just start with 0xff0xd8 : 'start-of-image'
  //
  // from : (https://web.archive.org/web/20131016210645/)http://www.64lines.com/jpeg-width-height

  //for (int i = 0; i < bufsize; ++i)
  //  std::cout << i << " : " << std::hex << reinterpret_cast<int>(buf[i] & 0xff) << std::dec << std::endl;

  if (/*(buf[0] == 0xFF && buf[1] == 0xD8 && buf[2] == 0xFF && buf[3] == 0xE0 &&
       buf[6] == 'J' && buf[7] == 'F' && buf[8] == 'I' && buf[9] == 'F' &&
       buf[10] == 0x00) ||
      (buf[0] == 0xFF && buf[1] == 0xD8 && buf[2] == 0xFF && buf[3] == 0xE1 &&
       buf[6] == 'E' && buf[7] == 'x' && buf[8] == 'i' && buf[9] == 'f' &&
       buf[10] == 0x00) ||*/
      (buf[0] == 0xFF && buf[1] == 0xD8 && buf[2] == 0xFF))
  {

    int jpeg_bufsize = 9;
    imagefile.seekg(jpeg_bufsize, std::ios_base::beg); // seek to this position, as if we were always reading just jpeg_bufsize bytes
    int pos = 2; // offset for 0xff 0xd8 which always seem to be the first two bytes

    while (true)
    {
      // check frame marker
      if (buf[pos] != 0xFF)
      {
        std::cout << "Failed to find start of JPEG header frame" << std::endl;
        return AttachmentMetadata{-1, -1, std::string(), 0, hash, std::string()};
      }
      // skip any extra frame markers
      while (buf[pos + 1] == 0xFF)
      {
        //std::cout << "Skipping extra frame marker" << std::endl;
        ++pos;
        if (pos >= jpeg_bufsize)
        {
          std::cout << "This could be fixed..." << std::endl;
          return AttachmentMetadata{-1, -1, std::string(), 0, hash, std::string()};
        }
      }

      if (buf[pos + 1] == 0xC0 || buf[pos + 1] == 0xC1 || buf[pos + 1] == 0xC2 || buf[pos + 1] == 0xC3 ||
          buf[pos + 1] == 0xC9 || buf[pos + 1] == 0xCA || buf[pos + 1] == 0xCB) // FOUND OUR MARKER!
      {
        //*y = (buf[pos + 5] << 8) + buf[pos + 6];
        //*x = (buf[pos + 7] << 8) + buf[pos + 8];

        return AttachmentMetadata{(buf[pos + 7] << 8) + buf[pos + 8],
          (buf[pos + 5] << 8) + buf[pos + 6],
          "image/jpeg", imagefile_size, hash, std::string()};
      }
      else // this was a different frame, skip it
      {
        int block_length = (buf[pos + 2] << 8) + buf[pos + 3];
        //std::cout << "Skipping frame (" << block_length << ")" << std::endl;

        //std::cout << block_length << " " << jpeg_bufsize << " " << pos << std::endl;
        //std::cout << block_length + 2 - (jpeg_bufsize - pos) << std::endl;

        imagefile.seekg(block_length + 2 - (jpeg_bufsize - pos), std::ios_base::cur); // + 2 skip marker itself

        //std::cout << "Pos is now: " << imagefile.tellg() << std::endl;

        if (!imagefile.read(reinterpret_cast<char *>(buf.get()), jpeg_bufsize))
        {
          std::cout << "Failed to read next 24 bytes from file" << std::endl;
          return AttachmentMetadata{-1, -1, std::string(), 0, hash, std::string()};
        }

        //for (int i = 0; i < bufsize; ++i)
        //  std::cout << i << " : " << std::hex << reinterpret_cast<int>(buf[i] & 0xff) << std::dec << std::endl;

        pos = 0;
      }
    }

  }
  return AttachmentMetadata{-1, -1, std::string(), 0, hash, std::string()};
  #else
  // todo, maybe? write this function for cryptopp
  return AttachmentMetadata{-1, -1, std::string(), 0, hash, std::string()};
  #endif
}
