/*
  Copyright (C) 2026  Selwin van Dijk

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

#ifdef BEPAALD_BV2_ENABLED

#include "backupv2reader.h"

#include "../common_bytes.h"

std::pair<unsigned char *, size_t> BackupV2Reader::getFrame()
{
  //std::unique_ptr<unsigned char []> medianame_bytes;
  while (true)
  {
    // now we repeatedly inflate into out output buffer (gzip_output)
    // whenever the zstream has made some gunzipped data available in
    // the output stream, we read frames (protobuf messages) from it
    // until we cant anymore. Reading a frame means:
    // 1: read the size of the frame (a varint)
    // 2: read this size of data and pass to handleFrame()
    // whenever we have read a frame, and want to read the next, there
    // are FOUR possible scenarios:
    // OPTION (1): We read the size (S), S bytes are still available so
    //             we read those...
    // OPTION (2): We read the size S, but have less than S bytes in
    //             the output stream: we move the unconsumed bytes +
    //             the size S to the start of the buffer, and tell the
    //             zstream where it can put new data. then break to call
    //             inflate again.
    // OPTION (2a): Size S is larger than the complete output buffer. In
    //              this case, we increase the size of the buffer and proceed
    //              as in (2)
    // OPTION (3a): We can not read the full size of the next frame.
    //              The hack here is we set the size to a size larger that the
    //              available space, to trigger (2)
    // OPTION (3b): We can not read the size of the next frame at all, the previous
    //              frame ended exactly at the end of the buffer, here we just
    //              reset the buffer pointers and call inflate again
    // OPTION (4):  We are at the end of the available data, and the zstream
    //              has indicated the end of the stream: We are done!

    if (d_gzip_reset) [[unlikely]]
    {
      d_gzip_previous_blocks_in = d_gunzip_stream.total_in;

      d_gzip_res = inflate(&d_gunzip_stream, Z_NO_FLUSH);
      if (d_gzip_res != Z_OK &&
          d_gzip_res != Z_STREAM_END) [[unlikely]]
      {
        Logger::error("Error gunzipping main data. (", d_gzip_res, ")");
        return {nullptr, 0};
      }

      d_gzip_current_block_in = d_gunzip_stream.total_in - d_gzip_previous_blocks_in;

      d_gzip_pos = 0;

      d_gzip_reset = false;
    }
    while (true)
    {
      // get size of frame, check if we have that available
      size_t available = ((d_gunzip_buffer - d_gunzip_stream.avail_out)/* + avail_from_previous*/) - d_gzip_pos;
      uint64_t size = 0;
      unsigned int iter = 0;

      //std::cout << "avail: " << available << std::endl;
      if (available == 0)
      {
        if (d_gzip_res != Z_STREAM_END) [[unlikely]]
          Logger::error("No bytes available, but not at ZSTREAM_END");
        else
        {
          //std::cout << "Used data size: " << d_main_data_gzipped_size << std::endl
          //          << "Corrected size: " << d_gunzip_stream.total_in << std::endl;

          // we now have the _real_ gzipped data size...
          d_main_data_gzipped_size = d_gunzip_stream.total_in;
        }
        return {nullptr, 0};
      }

      while ((iter < available - 1) &&
             (d_gzip_output.get()[d_gzip_pos + iter]) & 0b10000000)
        size += ((d_gzip_output.get()[d_gzip_pos + iter] & 0b01111111) << (7 * iter++));
      // get last bit
      if (d_gzip_output.get()[d_gzip_pos + iter] & 0b10000000) [[unlikely]]               // OPTION (3a)
      {
        //std::cout << "OPT 3a" << std::endl;

        size = d_gunzip_buffer - 10; // force buffer reset
        d_gzip_reset = true;
      }
      else
        size += (d_gzip_output.get()[d_gzip_pos + iter] << (7 * iter++));

      if (available - iter >= size) [[likely]]                                     // OPTION (1)
      {
        //std::cout << "Got size:  " << size << " (available: " << available << ")" << std::endl;

        if (!d_have_backupinfo) [[unlikely]] // first frame is BackupInfo
        {
          // BackupV2::BackupInfo bi(d_gzip_output.get() + pos + iter, size);
          // bi.print();
          d_have_backupinfo = true;
          d_gzip_pos += size + iter;
          if (d_gzip_pos == d_gunzip_buffer) [[unlikely]]                                     // OPTION (3b)
          {
            //std::cout << "OPT 3b(*)" << std::endl;
            d_gunzip_stream.next_out = d_gzip_output.get();
            d_gunzip_stream.avail_out = d_gunzip_buffer;
            d_gzip_reset = true;
            break;
          }
          continue;
        }

        d_gzip_pos += size + iter;

        if (d_gzip_pos == d_gunzip_buffer) [[unlikely]]                                     // OPTION (3b)
        {
          //std::cout << "OPT 3b" << std::endl;
          d_gunzip_stream.next_out = d_gzip_output.get();
          d_gunzip_stream.avail_out = d_gunzip_buffer;
          d_gzip_reset = true;
        }

        // RETURN FRAME DATA!
        return {d_gzip_output.get() + d_gzip_pos - size, size};
      }
      else // the next frame is not fully available in the output                    // OPTION (2)
      {    // buffer, so lets make some room and continue;
        //std::cout << "OPT 2" << std::endl;

        d_gzip_reset = true;

        // increase size of buffer if it is too small for frame
        if (size > d_gunzip_buffer - 10) [[unlikely]]                                // OPTION (2a)
        {
          //std::cout << "OPT 2a" << std::endl;

          d_gunzip_buffer += size;
          std::unique_ptr<unsigned char []> d_gzip_output_temp(new unsigned char[d_gunzip_buffer]);
          std::memcpy(d_gzip_output_temp.get(), d_gzip_output.get() + d_gzip_pos, available);
          d_gzip_output = std::move(d_gzip_output_temp);
        }
        else
          std::memcpy(d_gzip_output.get(), d_gzip_output.get() + d_gzip_pos, available);

        d_gunzip_stream.next_out = d_gzip_output.get() + available;
        d_gunzip_stream.avail_out = d_gunzip_buffer - available;
        break;
      }

      // if (d_gzip_pos >= d_gunzip_buffer - d_gunzip_stream.avail_out && d_gzip_res == Z_STREAM_END)       // OPTION (4)
      // {
      //   std::cout << "OPT 4" << std::endl;

      //   // we now have the _real_ gzipped data size...
      //   d_main_data_gzipped_size = d_gunzip_stream.total_in;

      //   Logger::message("Percentage: ",
      //                   ((d_gzip_previous_blocks_in / d_main_data_gzipped_size) +
      //                    ((static_cast<float>(d_gzip_pos) / (d_gunzip_buffer - d_gunzip_stream.avail_out)) *
      //                     (d_gzip_current_block_in / d_main_data_gzipped_size))) * 100, "%");
      //   d_gzip_reset = true;
      //   break;
      // }
    }

    if (d_gzip_res == Z_STREAM_END)
      break;
  }

  return {nullptr, 0};
}

#endif
