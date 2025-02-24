/*
  Copyright (C) 2025  Selwin van Dijk

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

#ifndef SIGNALPLAINTEXTBACKUPATTACHMENTREADER_H_
#define SIGNALPLAINTEXTBACKUPATTACHMENTREADER_H_

#include "../baseattachmentreader/baseattachmentreader.h"
#include "../framewithattachment/framewithattachment.h"
#include "../base64/base64.h"

#include <tuple>

class SignalPlainTextBackupAttachmentReader : public AttachmentReader<RawFileAttachmentReader>
{
  std::string d_base64data;
  std::string d_filename;
  long long int d_pos;
  long long int d_size; // size of the base64 string data
  long long int d_truesize;
 public:
  inline explicit SignalPlainTextBackupAttachmentReader(std::string const &b64data, std::string const &filename = std::string(),
                                                        long long int pos = -1, long long int size = -1);
  SignalPlainTextBackupAttachmentReader(SignalPlainTextBackupAttachmentReader const &other) = default;
  SignalPlainTextBackupAttachmentReader(SignalPlainTextBackupAttachmentReader &&other) = default;
  SignalPlainTextBackupAttachmentReader &operator=(SignalPlainTextBackupAttachmentReader const &other) = default;
  SignalPlainTextBackupAttachmentReader &operator=(SignalPlainTextBackupAttachmentReader &&other) = default;
  virtual ~SignalPlainTextBackupAttachmentReader() override = default;

  inline virtual int getAttachment(FrameWithAttachment *frame, bool verbose) override;
  inline int getAttachmentData(unsigned char **data, bool verbose);
  inline long long int dataSize();
};

SignalPlainTextBackupAttachmentReader::SignalPlainTextBackupAttachmentReader(std::string const &b64data, std::string const &filename,
                                                                             long long int pos, long long int size)
  :
  d_base64data(b64data),
  d_filename(filename),
  d_pos(pos),
  d_size(size),
  d_truesize(-1)
{}

inline int SignalPlainTextBackupAttachmentReader::getAttachment(FrameWithAttachment *frame, bool verbose) // virtual
{
  unsigned char *data = nullptr;
  int ret = getAttachmentData(&data, verbose);
  if (ret == 0)
    frame->setAttachmentDataBacked(data, d_truesize); // NOTE: test this when d_filename.empty()
  return ret;
}

inline int SignalPlainTextBackupAttachmentReader::getAttachmentData(unsigned char **data, bool verbose)
{
  // read the data if needed
  if (d_base64data.empty() && !d_filename.empty())
  {
    std::ifstream file(std::filesystem::path(d_filename), std::ios_base::binary | std::ios_base::in);
    if (!file.is_open())
    {
      Logger::error("Failed to open file '", d_filename, "' for reading attachment");
      return 1;
    }
    if (!file.seekg(d_pos))
    {
      Logger::error("Failed to seek to correct offset in file '", d_filename, " (", d_pos, ")");
      return 1;
    }

    if (verbose) [[unlikely]]
      Logger::message("Reading attachment data, length: ", d_size);

    d_base64data.reserve(d_size + 1);
    std::istreambuf_iterator<char> file_it(file);
    std::copy_n(file_it, d_size, std::back_inserter<std::string>(d_base64data));
    if (file.tellg() != (d_pos + d_size - 1))
    {
      Logger::error("Failed to read base64-encoded attachment from \"", d_filename, "\"");
      return 1;
    }
  }

  if (d_base64data.empty())
  {
    Logger::error("SignalPlainTextBackupAttachmentReader has no base64 encoded data");
    return 1;
  }

  unsigned char *attdata;
  std::tie(attdata, d_truesize) = Base64::base64StringToBytes(d_base64data);
  if (d_truesize == 0 || !attdata)
  {
    Logger::error("Failed to decode base64-encoded attachment from \"", d_filename, "\"");
    return 1;
  }

  *data = attdata;
  return 0;
}

inline long long int SignalPlainTextBackupAttachmentReader::dataSize()
{
  if (d_truesize == -1)
  {
    if (d_base64data.empty() && !d_filename.empty())
    {
      std::ifstream file(std::filesystem::path(d_filename), std::ios_base::binary | std::ios_base::in);
      if (!file.is_open())
      {
        Logger::error("Failed to open file '", d_filename, "' for reading attachment");
        return 1;
      }
      if (!file.seekg(d_pos + d_size - 2)) // we want the last two characters to check if they are padding
      {
        Logger::error("Failed to seek to correct offset in file '", d_filename, " (", d_pos, ")");
        return 1;
      }
      int numpadding = 0;
      char ch = file.get();
      if (ch == '=')
        ++numpadding;
      ch = file.get();
      if (ch == '=')
        ++numpadding;
      d_truesize = ((d_size / 4) * 3) - numpadding;
    }
    else
    {
      int numpadding = 0;
      if (d_base64data[d_base64data.size() - 1] == '=')
      {
        ++numpadding;
        if (d_base64data[d_base64data.size() - 2] == '=')
          ++numpadding;
      }
      d_truesize = ((d_base64data.size() / 4) * 3) - numpadding;
    }
  }
  return d_truesize;
}

#endif
