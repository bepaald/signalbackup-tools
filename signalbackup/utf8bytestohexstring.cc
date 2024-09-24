#include "signalbackup.ih"

#include "../common_bytes.h"

std::string SignalBackup::utf8BytesToHexString(unsigned char const *const data, size_t data_size) const
{
  // NOTE THIS IS NOT GENERIC UTF-8 CONVERSION, THIS
  // DATA IS GUARANTEED TO HAVE ONLY SINGLE- AND TWO-BYTE
  // CHARS (NO 3 OR 4-BYTE). THE TWO-BYTE CHARS NEVER
  // CONTAIN MORE THAN TWO BITS OF DATA
  unsigned char output[16]{0};
  uint outputpos = 0;
  for (uint i = 0; i < data_size; ++i)
  {
    if (outputpos >= 16) [[unlikely]]
      return std::string();

    if ((data[i] & 0b10000000) == 0) // single byte char
      output[outputpos++] += data[i];
    else // 2 byte char
      output[outputpos++] = ((data[i] & 0b00000011) << 6) | (data[i + 1] & 0b00111111), ++i;
  }
  return bepaald::bytesToHexString(output, 16, true);
}
