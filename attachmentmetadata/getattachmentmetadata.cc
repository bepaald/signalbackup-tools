/*
  Copyright (C) 2022-2024  Selwin van Dijk

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

#include "attachmentmetadata.h"

#include <openssl/sha.h>

#include "../base64/base64.h"
#include "../common_filesystem.h"

AttachmentMetadata AttachmentMetadata::getAttachmentMetaData(std::string const &file, unsigned char *data, long long int data_size, bool skiphash) // static
{
  //struct AttachmentMetadata
  //{
  //  int width;
  //  int height;
  //  std::string filetype;
  //  unsigned long filesize;
  //  std::string hash;
  //  std::string filename;
  //  operator bool() const { return (width != -1 && height != -1 && !filetype.empty() && filesize != 0); }
  //};



  if (data_size == 0)
  {
    Logger::warning("Attachment '", file, "' is zero bytes");
    return AttachmentMetadata{-1, -1, std::string(), data_size, std::string(), file};
  }



  std::string hash;
  if (!skiphash)
  {
    // gethash
    unsigned char rawhash[SHA256_DIGEST_LENGTH];
    std::unique_ptr<EVP_MD_CTX, decltype(&::EVP_MD_CTX_free)> sha256(EVP_MD_CTX_new(), &::EVP_MD_CTX_free);
    if (!sha256 ||
        EVP_DigestInit_ex(sha256.get(), EVP_sha256(), nullptr) != 1 ||
        EVP_DigestUpdate(sha256.get(), data, data_size) != 1 ||
        EVP_DigestFinal_ex(sha256.get(), rawhash, nullptr) != 1) [[unlikely]]
    {
      Logger::warning("Failed to set hash");
      hash = std::string();
    }
    hash = Base64::bytesToBase64String(rawhash, SHA256_DIGEST_LENGTH);
    //std::cout << bepaald::bytesToHexString(rawhash, SHA256_DIGEST_LENGTH) << std::endl;
    //std::cout << "GOT HASH: " << hash << std::endl;
  }

  // set buffer for file header
  int bufsize = std::min(data_size, 30ll);
  unsigned char *buf = data;





  // PNG: the first frame is by definition an IHDR frame, which gives dimensions
  // NEED 24 bytes
  if (bufsize >= 24 &&
      buf[0] == 0x89 && buf[1] == 'P' && buf[2] == 'N' && buf[3] == 'G' && buf[4] == 0x0D && buf[5] == 0x0A && buf[6] == 0x1A && buf[7] == 0x0A &&
      buf[12] == 'I' && buf[13] == 'H' && buf[14] == 'D' && buf[15] == 'R')
  {
    //*x = (buf[16] << 24) + (buf[17] << 16) + (buf[18] << 8) + (buf[19] << 0);
    //*y = (buf[20] << 24) + (buf[21] << 16) + (buf[22] << 8) + (buf[23] << 0);
    return AttachmentMetadata{(buf[16] << 24) + (buf[17] << 16) + (buf[18] << 8) + (buf[19] << 0),
      (buf[20] << 24) + (buf[21] << 16) + (buf[22] << 8) + (buf[23] << 0),
      "image/png", data_size, hash, file};
  }







  // GIF: first three bytes say "GIF", next three give version number. Then dimensions
  // NEEDS 10 bytes
  if (bufsize >= 10 &&
      buf[0] == 'G' && buf[1] == 'I' && buf[2] == 'F')
  {
    //*x = buf[6] + (buf[7] << 8);
    //*y = buf[8] + (buf[9] << 8);
    return AttachmentMetadata{buf[8] + (buf[9] << 8),
      buf[6] + (buf[7] << 8),
      "image/gif", data_size, hash, file};
  }








  // WEBP
  // https://developers.google.com/speed/webp/docs/riff_container
  // Starts with 'R','I','F','F','?','?','?','?','W','E','B','P'
  if (bufsize >= 30 &&
      buf[0] == 'R' && buf[1] == 'I' && buf[2] == 'F' && buf[3] == 'F' &&
      buf[8] == 'W' && buf[9] == 'E' && buf[10] == 'B' && buf[11] == 'P')
  {
    if (std::memcmp(buf + 12, "VP8 ", 4) == 0) // 'lossless'
    {
      //std::cout << "lossy" << std::endl;
      int w = ((buf[26] | buf[27] << 8) & 0x3fff);
      int h = ((buf[28] | buf[29] << 8) & 0x3fff);
      //std::cout << "WidhtxHeight: " << w << "x" << h << std::endl<< std::endl;
      return AttachmentMetadata{w, h, "image/webp", data_size, hash, file};
    }
    else if (std::memcmp(buf + 12, "VP8L", 4) == 0) // 'lossy'
    {
      //std::cout << "lossless" << std::endl;
      uint32_t size = (buf[21] | (buf[22] << 8) | (buf[23] << 16) | (buf[24] << 24));
      int w = (size & 0x3fff) + 1;
      int h = ((size >> 14) & 0x3fff) + 1;
      //std::cout << "WidhtxHeight: " << w << "x" << h << std::endl<< std::endl;
      return AttachmentMetadata{w, h, "image/webp", data_size, hash, file};
    }
    else if (std::memcmp(buf + 12, "VP8X", 4) == 0) // 'extended'
    {
      //std::cout << "extended" << std::endl;

      // first byte of VPX8 header = RrILEXAR
      //    where Rr = reserved 00
      //    R = reserved 0
      //    (ILEXA are all 1 bit flags for something)
      // then 24 bits reserved 0
      // then 24 bits canvas width - 1
      // then 24 bits canvas height - 1
      if ((buf[20] & 0b11000000) == 0 &&
          (buf[20] & 0b00000001) == 0)
      {
        int w = (buf[24] | (buf[25] << 8) | (buf[26] << 16)) + 1;
        int h = (buf[27] | (buf[28] << 8) | (buf[29] << 16)) + 1;
        //std::cout << "WidhtxHeight: " << w << "x" << h << std::endl<< std::endl;
        return AttachmentMetadata{w, h, "image/webp", data_size, hash, file};
      }
      else
        return AttachmentMetadata{-1, -1, "image/webp", data_size, hash, file};
    }
    else
      return AttachmentMetadata{-1, -1, "image/webp", data_size, hash, file};
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

  //std::cout << "DATA: " << bepaald::bytesToHexString(buf, 100) << std::endl;

  if (/*(buf[0] == 0xFF && buf[1] == 0xD8 && buf[2] == 0xFF && buf[3] == 0xE0 &&
       buf[6] == 'J' && buf[7] == 'F' && buf[8] == 'I' && buf[9] == 'F' &&
       buf[10] == 0x00) ||
      (buf[0] == 0xFF && buf[1] == 0xD8 && buf[2] == 0xFF && buf[3] == 0xE1 &&
       buf[6] == 'E' && buf[7] == 'x' && buf[8] == 'i' && buf[9] == 'f' &&
       buf[10] == 0x00) ||*/
      (buf[0] == 0xFF && buf[1] == 0xD8 && buf[2] == 0xFF))
  {

    int jpeg_bufsize = 9;
    int seekpos = 0;
    int pos = 2; // offset for 0xff 0xd8 which always seem to be the first two bytes

    while (true)
    {
      // check frame marker
      if (buf[pos] != 0xFF)
      {
        Logger::warning("Failed to find start of JPEG header frame");
        return AttachmentMetadata{-1, -1, std::string(), data_size, hash, file};
      }
      // skip any extra frame markers
      while (buf[pos + 1] == 0xFF)
      {
        //std::cout << "Skipping extra frame marker" << std::endl;
        ++pos;
        if (pos >= jpeg_bufsize)
        {
          Logger::message("This could be fixed...");
          return AttachmentMetadata{-1, -1, std::string(), data_size, hash, file};
        }
      }

      if (buf[pos + 1] == 0xC0 || buf[pos + 1] == 0xC1 || buf[pos + 1] == 0xC2 || buf[pos + 1] == 0xC3 ||
          buf[pos + 1] == 0xC9 || buf[pos + 1] == 0xCA || buf[pos + 1] == 0xCB) // FOUND OUR MARKER!
      {
        //*y = (buf[pos + 5] << 8) + buf[pos + 6];
        //*x = (buf[pos + 7] << 8) + buf[pos + 8];

        //std::cout << "GOT MARKER" << std::endl;

        return AttachmentMetadata{(buf[pos + 7] << 8) + buf[pos + 8],
          (buf[pos + 5] << 8) + buf[pos + 6],
          "image/jpeg", data_size, hash, file};
      }
      else // this was a different frame, skip it
      {
        //std::cout << "DIFFERENT MARKER, SKIP!" << std::endl;

        int block_length = (buf[pos + 2] << 8) + buf[pos + 3];
        //std::cout << "Skipping frame (" << block_length << ")" << std::endl;

        //std::cout << block_length << " " << jpeg_bufsize << " " << pos << " " << block_length + 2 - (jpeg_bufsize - pos) << std::endl;

        if ((block_length + pos + 2) > static_cast<int64_t>(data_size - jpeg_bufsize))
        {
          Logger::warning("Failed to read next jpeg_buffer from data");
          return AttachmentMetadata{-1, -1, std::string(), data_size, hash, file};
        }

        //filestream.seekg(block_length + 2 - (jpeg_bufsize - pos), std::ios_base::cur); // + 2 skip marker itself
        // if (!filestream.read(reinterpret_cast<char *>(buf.get()), jpeg_bufsize))
        // {
        //   Logger::warning("Failed to read next 24 bytes from file");
        //   return AttachmentMetadata{-1, -1, std::string(), data_size, hash, file};
        // }
        //std::cout << "Pos is now: " << filestream.tellg() << std::endl;

        seekpos += block_length + pos + 2;
        buf = data + seekpos;


        //for (int i = 0; i < bufsize; ++i)
        //  std::cout << i << " : " << std::hex << reinterpret_cast<int>(buf[i] & 0xff) << std::dec << std::endl;

        pos = 0;
      }
    }

  }
  return AttachmentMetadata{-1, -1, std::string(), data_size, hash, file};
}

AttachmentMetadata AttachmentMetadata::getAttachmentMetaData(std::string const &file, bool skiphash) //static
{

  //struct AttachmentMetadata
  //{
  //  int width;
  //  int height;
  //  std::string filetype;
  //  unsigned long filesize;
  //  std::string hash;
  //  std::string filename;
  //  operator bool() const { return (width != -1 && height != -1 && !filetype.empty() && filesize != 0); }
  //};

  std::ifstream filestream(std::filesystem::path(file), std::ios_base::binary | std::ios_base::in);
  if (!filestream.is_open())
  {
    Logger::warning("Failed to open image for reading: ", file);
    return AttachmentMetadata{-1, -1, std::string(), 0, std::string(), std::string()};
  }

  filestream.seekg(0, std::ios_base::end);
  long long int file_size = filestream.tellg();
  filestream.seekg(0, std::ios_base::beg);

  if (file_size == 0)
  {
    Logger::warning("Attachment '", file, "' is zero bytes");
    return AttachmentMetadata{-1, -1, std::string(), file_size, std::string(), file};
  }

  std::unique_ptr<unsigned char[]> file_data(new unsigned char[file_size]);
  if (!filestream.read(reinterpret_cast<char *>(file_data.get()), file_size) ||
      filestream.gcount() != static_cast<long>(file_size))
  {
    Logger::warning("Failed to read ", file_size, " bytes from file '", file, "'");
    return AttachmentMetadata{-1, -1, std::string(), file_size, std::string(), file};
  }

  return getAttachmentMetaData(file, file_data.get(), file_size, skiphash);
}
