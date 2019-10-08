/*
    Copyright (C) 2019  Selwin van Dijk

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

#include "sqlcipherdecryptor.ih"

bool SqlCipherDecryptor::decryptData(std::ifstream *dbfile)
{
  // decrypt data
  d_decrypteddata = new unsigned char[d_decrypteddatasize];
  unsigned int pos = 0;

  // write header
  std::memcpy(d_decrypteddata + pos, s_sqlliteheader, 16);
  pos += 16;


    /*
    PAGE

                                                         page_size
     -------------------------------------------------------------------------------------------------------------------------------
    /                                                                     real_page_size                                            \
    |                                 -----------------------------------------------------------------------------------------------|
    |                                /                                                                                               |
    |                                |                                                                                               |
    |                                |     page + (real_page_size - (digest_size + page_padding) - iv_size)                          |
    |                                |                v                                                                              |

    [salt, 16 bytes, only first page][encrypted bytes][iv, 16 bytes][mac, padded to 16 bytes (for version < 3, 20 bytes, padded to 32]


    */

  unsigned int iv_size = 16;
  unsigned int page_padding = (((d_digestsize - 1) | 15) + 1) - d_digestsize;  // pad to multiple of 16 bytes ??? (maybe 32?)
  unsigned char *page = new unsigned char[d_pagesize];
  unsigned int pagenumber = 1;
  CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption d;
  while (true)
  {
    unsigned int real_page_size = pagenumber == 1 ? d_pagesize - d_saltsize : d_pagesize;

    if (!dbfile->read(reinterpret_cast<char *>(page), real_page_size))
    {
      if (dbfile->gcount() == 0 && dbfile->eof())
        break;
      std::cout << "Unexpectedly failed to read next block" << (dbfile->eof() ? " (EOF)" : "") << std::endl;
      return false;
    }

    // these pointers all point to specific data inside 'page' (which was hust read from dbfile
    unsigned char *page_data_to_hash = page;
    unsigned int page_data_to_hash_size = real_page_size - (d_digestsize + page_padding);
    unsigned char *iv = page + page_data_to_hash_size - iv_size;
    unsigned char *page_encrypted_data = page;
    unsigned int page_encrypted_data_size = page_data_to_hash_size - iv_size;

    //std::cout << "Calling mac calc with data (" << real_page_size - (CryptoPP::HMAC<CryptoPP::SHA1>::DIGESTSIZE + page_padding) << "): "
    //          << bepaald::bytesToHexString(page, real_page_size - (CryptoPP::HMAC<CryptoPP::SHA1>::DIGESTSIZE + page_padding)) << std::endl;
    unsigned char *calculatedmac = new unsigned char[d_digestsize];
    d_hmac->Update(page_data_to_hash, page_data_to_hash_size);
    d_hmac->Update(reinterpret_cast<unsigned char *>(&pagenumber), sizeof(pagenumber));
    d_hmac->Final(calculatedmac);

    //std::cout << "HMAC DIGESTSIZE: " << CryptoPP::HMAC<CryptoPP::SHA1>::DIGESTSIZE << std::endl;
    //std::cout << "Pagenumber: " << pagenumber << std::endl;
    if (std::memcmp(page + (real_page_size - (d_digestsize + page_padding)), calculatedmac, d_digestsize) != 0)
    {
      std::cout << "MAC in file: " << bepaald::bytesToHexString(page + (real_page_size - (d_digestsize + page_padding)), d_digestsize) << std::endl;
      std::cout << "Calculated : " << bepaald::bytesToHexString(calculatedmac, d_digestsize) << std::endl;
      std::cout << "BAD MAC!" << std::endl;
      delete[] calculatedmac;
      bepaald::destroyPtr(&page, &d_pagesize);
      return false;
    }
    else
    {
      //std::cout << "MAC OK!" << std::endl;
      delete[] calculatedmac;

      int decodedframelength = d_pagesize;
      if (pagenumber == 1)
        decodedframelength -= d_saltsize;

      d.SetKeyWithIV(d_key, d_keysize, iv, iv_size);
      d.ProcessData(d_decrypteddata + pos, page_encrypted_data, page_encrypted_data_size);
      std::memset(d_decrypteddata + pos + page_encrypted_data_size, 0, decodedframelength - page_encrypted_data_size); // append zeros
      pos += decodedframelength;

      //std::cout << "Writing " << decodedframelength << " bytes to file" << std::endl;
      //outputdb.write(reinterpret_cast<char *>(decodedframe2.get()), decodedframelength);
    }
    ++pagenumber;
  }
  bepaald::destroyPtr(&page, &d_pagesize);
  return true;
}
