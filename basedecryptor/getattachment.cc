/*
    Copyright (C) 2019-2020  Selwin van Dijk

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


int BaseDecryptor::getAttachment(FrameWithAttachment *frame) // static
{
  std::ifstream file(frame->filename(), std::ios_base::binary | std::ios_base::in);
  if (!file.is_open())
  {
    std::cout << "Failed to open backup file for reading attachment" << std::endl;
    return 1;
  }

  //std::cout << "Getting attachment: " << frame->filepos() << " + " << frame->length() << std::endl;
  file.seekg(frame->filepos(), std::ios_base::beg);

  //uintToFourBytes(d_iv, d_counter++); // done in getFrame
  CryptoPP::CTR_Mode<CryptoPP::AES>::Decryption decryptor(frame->cipherkey(), frame->cipherkey_size(), frame->iv());

  CryptoPP::HMAC<CryptoPP::SHA256> hmac(frame->mackey(), frame->mackey_size());
  hmac.Update(frame->iv(), frame->iv_size());

  // read and process attachment data in 8MB chunks
  uint32_t const BUFFERSIZE = 8 * 1024;
  unsigned char encrypteddatabuffer[BUFFERSIZE];
  uint32_t processed = 0;
  uint32_t size = frame->length();
  unsigned char *decryptedattachmentdata = new unsigned char[size]; // to hold the data
  while (processed < size)
  {
    if (!file.read(reinterpret_cast<char *>(encrypteddatabuffer), std::min(size - processed, BUFFERSIZE)))
    {
      std::cout << " STOPPING BEFORE END OF ATTACHMENT!!!" << (file.eof() ? " (EOF) " : "") << std::endl;
      delete[] decryptedattachmentdata;
      return 1;
    }
    uint32_t read = file.gcount();

    hmac.Update(encrypteddatabuffer, read);

    decryptor.ProcessData(decryptedattachmentdata + processed, encrypteddatabuffer, read);

    processed += read;
    //return;
  }
  DEBUGOUT("Read ", processed, " bytes");

  unsigned char ourMac[CryptoPP::HMAC<CryptoPP::SHA256>::DIGESTSIZE];
  hmac.Final(ourMac);

  unsigned char theirMac[MACSIZE];
  if (!file.read(reinterpret_cast<char *>(theirMac), MACSIZE))
  {
    std::cout << " STOPPING BEFORE END OF ATTACHMENT!!! 2 " << std::endl;
    delete[] decryptedattachmentdata;
    return 1;
  }
  DEBUGOUT("theirMac         : ", bepaald::bytesToHexString(theirMac, MACSIZE));
  DEBUGOUT("ourMac           : ", bepaald::bytesToHexString(ourMac, CryptoPP::HMAC<CryptoPP::SHA256>::DIGESTSIZE));

  bool badmac = false;

  if (std::memcmp(theirMac, ourMac, 10) != 0)
  {
    std::cout << "" << std::endl;
    std::cout << "WARNING: Bad MAC in attachmentdata: theirMac: " << bepaald::bytesToHexString(theirMac, MACSIZE) << std::endl;
    std::cout << "                                      ourMac: " << bepaald::bytesToHexString(ourMac, CryptoPP::HMAC<CryptoPP::SHA256>::DIGESTSIZE) << std::endl;

    badmac = true;
  }
  else
    badmac = false;

  if (frame->setAttachmentData(decryptedattachmentdata))
  {
    if (badmac)
      return -1;
    return 0;
  }
  return 1;
}
