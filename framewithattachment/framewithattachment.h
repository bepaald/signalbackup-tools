/*
    Copyright (C) 2019-2022  Selwin van Dijk

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

#ifndef FRAMEWITHATTACHMENT_H_
#define FRAMEWITHATTACHMENT_H_

#include <cstring>
#include <memory>
#include <fstream>

#include "../backupframe/backupframe.h"

class FrameWithAttachment : public BackupFrame
{
 protected:
  unsigned char *d_attachmentdata;
  uint32_t d_attachmentdata_size;
  uint64_t d_filepos;
  uint32_t d_iv_size;
  unsigned char *d_iv;
  uint64_t d_mackey_size;
  unsigned char *d_mackey;
  uint64_t d_cipherkey_size;
  unsigned char *d_cipherkey;
  std::string d_filename;

  //BaseDecryptor *d_dec; // DONT OWN THIS!
 public:
  inline explicit FrameWithAttachment(uint64_t count = 0);
  inline FrameWithAttachment(unsigned char *bytes, size_t length, uint64_t count = 0);
  inline FrameWithAttachment(FrameWithAttachment &&other);
  inline FrameWithAttachment &operator=(FrameWithAttachment &&other);
  inline FrameWithAttachment(FrameWithAttachment const &other);
  inline FrameWithAttachment &operator=(FrameWithAttachment const &other);
  inline virtual ~FrameWithAttachment();
  inline bool setAttachmentData(unsigned char *data) override;
  inline bool setAttachmentData(std::string const &filename);
  inline unsigned char *iv() const;
  inline uint32_t iv_size() const;
  inline uint64_t filepos() const;
  inline uint32_t length() const;
  inline unsigned char *mackey() const;
  inline uint64_t mackey_size() const;
  inline unsigned char *cipherkey() const;
  inline uint64_t cipherkey_size() const;
  inline std::string const &filename() const;
  inline void setLazyData(unsigned char *iv, uint32_t iv_size, unsigned char *mackey, uint64_t mackey_size, unsigned char *cipherkey, uint64_t cipherkey_size, uint32_t attsize, std::string const &filename, uint64_t filepos);
  //inline virtual void setLazyData(unsigned char *iv, uint32_t iv_size, uint32_t attsize, uint64_t filepos, BaseDecryptor *dec);
  inline unsigned char *attachmentData();
  inline void clearData();
};

inline FrameWithAttachment::FrameWithAttachment(uint64_t count)
  :
  BackupFrame(count),
  d_attachmentdata(nullptr),
  d_attachmentdata_size(0),
  d_filepos(0),
  d_iv_size(0),
  d_iv(nullptr),
  d_mackey_size(0),
  d_mackey(nullptr),
  d_cipherkey_size(0),
  d_cipherkey(nullptr)
  //d_dec(nullptr)
{}

inline FrameWithAttachment::FrameWithAttachment(unsigned char *bytes, size_t length, uint64_t count)
  :
  BackupFrame(bytes, length, count),
  d_attachmentdata(nullptr),
  d_attachmentdata_size(0),
  d_filepos(0),
  d_iv_size(0),
  d_iv(nullptr),
  d_mackey_size(0),
  d_mackey(nullptr),
  d_cipherkey_size(0),
  d_cipherkey(nullptr)
  //d_dec(nullptr)
{}

inline FrameWithAttachment::FrameWithAttachment(FrameWithAttachment &&other)
  :
  BackupFrame(std::move(other)),
  d_attachmentdata(std::move(other.d_attachmentdata)),
  d_attachmentdata_size(std::move(other.d_attachmentdata_size)),
  d_filepos(std::move(other.d_filepos)),
  d_iv_size(std::move(other.d_iv_size)),
  d_iv(std::move(other.d_iv)),
  d_mackey_size(std::move(other.d_mackey_size)),
  d_mackey(std::move(other.d_mackey)),
  d_cipherkey_size(std::move(other.d_cipherkey_size)),
  d_cipherkey(std::move(other.d_cipherkey)),
  d_filename(std::move(other.d_filename))
//d_dec(std::move(other.d_dec))
{
  other.d_attachmentdata = nullptr;
  other.d_attachmentdata_size = 0;
  other.d_iv_size = 0;
  other.d_iv = nullptr;
  other.d_mackey_size = 0;
  other.d_mackey = nullptr;
  other.d_cipherkey_size = 0;
  other.d_cipherkey = nullptr;
  //other.d_dec = nullptr;
}

inline FrameWithAttachment &FrameWithAttachment::operator=(FrameWithAttachment &&other)
{
  if (this != &other)
  {
    bepaald::destroyPtr(&d_attachmentdata, &d_attachmentdata_size);
    bepaald::destroyPtr(&d_iv, &d_iv_size);
    bepaald::destroyPtr(&d_mackey, &d_mackey_size);
    bepaald::destroyPtr(&d_cipherkey, &d_cipherkey_size);

    BackupFrame::operator=(std::move(other));
    d_attachmentdata = std::move(other.d_attachmentdata);
    d_attachmentdata_size = std::move(other.d_attachmentdata_size);
    d_filepos = std::move(other.d_filepos);
    d_iv_size = std::move(other.d_iv_size);
    d_iv = std::move(other.d_iv);
    d_mackey_size = std::move(other.d_mackey_size);
    d_mackey = std::move(other.d_mackey);
    d_cipherkey_size = std::move(other.d_cipherkey_size);
    d_cipherkey = std::move(other.d_cipherkey);
    //d_dec = std::move(other.d_dec);

    other.d_attachmentdata = nullptr;
    other.d_attachmentdata_size = 0;
    other.d_iv = nullptr;
    other.d_iv_size = 0;
    other.d_mackey_size = 0;
    other.d_mackey = nullptr;
    other.d_cipherkey_size = 0;
    other.d_cipherkey = nullptr;
    //other.d_dec = nullptr;
  }
  return *this;
}

inline FrameWithAttachment::FrameWithAttachment(FrameWithAttachment const &other)
  :
  BackupFrame(other),
  d_attachmentdata(nullptr),
  d_attachmentdata_size(other.d_attachmentdata_size),
  d_filepos(other.d_filepos),
  d_iv_size(other.d_iv_size),
  d_iv(nullptr),
  d_mackey_size(other.d_mackey_size),
  d_mackey(nullptr),
  d_cipherkey_size(other.d_cipherkey_size),
  d_cipherkey(nullptr)
  //d_dec(other.d_dec)
{
  if (other.d_attachmentdata)
  {
    d_attachmentdata = new unsigned char[d_attachmentdata_size];
    std::memcpy(d_attachmentdata, other.d_attachmentdata, d_attachmentdata_size);
  }

  if (other.d_iv)
  {
    d_iv = new unsigned char[d_iv_size];
    std::memcpy(d_iv, other.d_iv, d_iv_size);
  }

  if (other.d_mackey)
  {
    d_mackey = new unsigned char[d_mackey_size];
    std::memcpy(d_mackey, other.d_mackey, d_mackey_size);
  }

  if (other.d_cipherkey)
  {
    d_cipherkey = new unsigned char[d_cipherkey_size];
    std::memcpy(d_cipherkey, other.d_cipherkey, d_cipherkey_size);
  }
}

inline FrameWithAttachment &FrameWithAttachment::operator=(FrameWithAttachment const &other)
{
  if (this != &other)
  {
    bepaald::destroyPtr(&d_attachmentdata, &d_attachmentdata_size);
    bepaald::destroyPtr(&d_iv, &d_iv_size);
    bepaald::destroyPtr(&d_mackey, &d_mackey_size);
    bepaald::destroyPtr(&d_cipherkey, &d_cipherkey_size);

    BackupFrame::operator=(other);
    d_attachmentdata_size = other.d_attachmentdata_size;
    d_iv_size = other.d_iv_size;
    d_mackey_size = other.d_mackey_size;
    d_cipherkey_size = other.d_cipherkey_size;
    d_filepos = other.d_filepos;
    //d_dec = other.d_dec;

    if (other.d_attachmentdata)
    {
      d_attachmentdata = new unsigned char[d_attachmentdata_size];
      std::memcpy(d_attachmentdata, other.d_attachmentdata, d_attachmentdata_size);
    }

    if (other.d_iv)
    {
      d_iv = new unsigned char[d_iv_size];
      std::memcpy(d_iv, other.d_iv, d_iv_size);
    }

    if (other.d_mackey)
    {
      d_mackey = new unsigned char[d_mackey_size];
      std::memcpy(d_mackey, other.d_mackey, d_mackey_size);
    }

    if (other.d_cipherkey)
    {
      d_cipherkey = new unsigned char[d_cipherkey_size];
      std::memcpy(d_cipherkey, other.d_cipherkey, d_cipherkey_size);
    }
  }
  return *this;
}

inline FrameWithAttachment::~FrameWithAttachment()
{
  bepaald::destroyPtr(&d_attachmentdata, &d_attachmentdata_size);
  bepaald::destroyPtr(&d_iv, &d_iv_size);
  bepaald::destroyPtr(&d_mackey, &d_mackey_size);
  bepaald::destroyPtr(&d_cipherkey, &d_cipherkey_size);
}

inline bool FrameWithAttachment::setAttachmentData(unsigned char *data) // override
{
  if (!data)
    return false;
  d_attachmentdata = data;
  d_attachmentdata_size = length();
  return true;
}

inline bool FrameWithAttachment::setAttachmentData(std::string const &filename) // override
{
  std::ifstream file(filename, std::ios_base::binary | std::ios_base::in);
  if (!file.is_open())
  {
    std::cout << "Failed to open '" << filename << "' for reading" << std::endl;
    return false;
  }
  file.seekg(0, std::ios_base::end);
  d_attachmentdata_size = file.tellg();
  file.seekg(0);
  d_attachmentdata = new unsigned char[d_attachmentdata_size];
  if (!file.read(reinterpret_cast<char *>(d_attachmentdata), d_attachmentdata_size))
  {
    std::cout << "Failed to read data from '" << filename << "'" << std::endl;
    bepaald::destroyPtr(&d_attachmentdata, &d_attachmentdata_size);
    return false;
  }
  return true;
}

inline unsigned char *FrameWithAttachment::iv() const
{
  return d_iv;
}

inline uint32_t FrameWithAttachment::iv_size() const
{
  return d_iv_size;
}

inline uint64_t FrameWithAttachment::filepos() const
{
  return d_filepos;
}

inline uint32_t FrameWithAttachment::length() const
{
  return d_attachmentdata_size;
}

inline unsigned char *FrameWithAttachment::mackey() const
{
  return d_mackey;
}

inline uint64_t FrameWithAttachment::mackey_size() const
{
  return d_mackey_size;
}

inline unsigned char *FrameWithAttachment::cipherkey() const
{
  return d_cipherkey;
}

inline uint64_t FrameWithAttachment::cipherkey_size() const
{
  return d_cipherkey_size;
}

inline std::string const &FrameWithAttachment::filename() const
{
  return d_filename;
}

inline void FrameWithAttachment::setLazyData(unsigned char *iv, uint32_t iv_size, unsigned char *mackey, uint64_t mackey_size, unsigned char *cipherkey, uint64_t cipherkey_size, uint32_t attsize, std::string const &filename, uint64_t filepos/*, BaseDecryptor *dec*/)
{
  d_iv_size = iv_size;
  d_iv = new unsigned char[d_iv_size];
  std::memcpy(d_iv, iv, d_iv_size);

  d_cipherkey_size = cipherkey_size;
  d_cipherkey = new unsigned char[d_cipherkey_size];
  std::memcpy(d_cipherkey, cipherkey, d_cipherkey_size);

  d_mackey_size = mackey_size;
  d_mackey = new unsigned char[d_mackey_size];
  std::memcpy(d_mackey, mackey, d_mackey_size);

  d_attachmentdata_size = attsize;

  d_filename = filename;
  d_filepos = filepos;

  //d_dec = dec;
}

inline unsigned char *FrameWithAttachment::attachmentData()
{
  if (!d_attachmentdata)
    if (BaseDecryptor::getAttachment(this) != 0)
      return nullptr;
  return d_attachmentdata;
}

inline void FrameWithAttachment::clearData()
{
  if (d_attachmentdata) // do not use bepaald::destroyPtr, it will set size to zero
  {
    delete[] d_attachmentdata;
    d_attachmentdata = nullptr;
  }
}

#endif
