/*
  Copyright (C) 2021-2024  Selwin van Dijk

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

//#include "filedecryptor.ih"

// #include <openssl/aes.h>
// #include <openssl/evp.h>
// #include <openssl/hmac.h>
// #include <openssl/sha.h>
// void FileDecryptor::strugee(uint64_t pos)
// {
//   unsigned int offset = 0;

//   d_file.seekg(pos, std::ios_base::beg);

//   Logger::message("Getting frame at filepos: ", d_file.tellg());

//   if (static_cast<uint64_t>(d_file.tellg()) == d_filesize)
//   {
//     Logger::message("Read entire backup file...");
//     return;
//   }

//   uint32_t encryptedframelength = getNextFrameBlockSize();
//   if (encryptedframelength > 3145728/*= 3MB*/ /*115343360 / * =110MB*/ || encryptedframelength < 11)
//   {
//     Logger::error("Framesize too big to be real");
//     return;
//   }

//   std::unique_ptr<unsigned char[]> encryptedframe(new unsigned char[encryptedframelength]);
//   if (!getNextFrameBlock(encryptedframe.get(), encryptedframelength))
//     return;

//   // check hash
//   unsigned int digest_size = SHA256_DIGEST_LENGTH;
//   unsigned char hash[SHA256_DIGEST_LENGTH];
//   HMAC(EVP_sha256(), d_mackey, d_mackey_size, encryptedframe.get(), encryptedframelength - MACSIZE, hash, &digest_size);
//   if (std::memcmp(encryptedframe.get() + (encryptedframelength - MACSIZE), hash, MACSIZE) != 0)
//   {
//     Logger::error("BAD MAC!");
//     return;
//   }
//   else
//   {
//     Logger::message("\nGOT GOOD MAC AT OFFSET ", offset, " BYTES!\n"
//                     "Now let's try and find out how many frames we skipped to get here....");
//     d_badmac = false;
//   }

//   // decode
//   unsigned int skipped = 0;
//   std::unique_ptr<BackupFrame> frame(nullptr);
//   while (!frame)
//   {

//     if (skipped > 1000000) // a frame is at least 10 bytes? -> could probably safely set this higher. MAC alone is 10 bytes, there is also actual data
//     {
//       Logger::message("TESTED 1000000 frames");
//       return;
//     }

//     if (skipped % 100 == 0)
//       Logger::message_overwrite("Checking if we skipped ", skipped, " frames... ");
//       //Logger::message("\rChecking if we skipped ", skipped, " frames... ", std::flush;

//     uintToFourBytes(d_iv, d_counter + skipped);

//     // create context
//     std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), &::EVP_CIPHER_CTX_free);

//     // disable padding
//     EVP_CIPHER_CTX_set_padding(ctx.get(), 0);

//     if (EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_ctr(), nullptr, d_cipherkey, d_iv) != 1)
//     {
//       Logger::error("CTX INIT FAILED");
//       return;
//     }

//     int decodedframelength = encryptedframelength - MACSIZE;
//     unsigned char *decodedframe = new unsigned char[decodedframelength];

//     if (EVP_DecryptUpdate(ctx.get(), decodedframe, &decodedframelength, encryptedframe.get(), encryptedframelength - MACSIZE) != 1)
//     {
//       Logger::error("Failed to decrypt data");
//       delete[] decodedframe;
//       return;
//     }

//     DEBUGOUT("Decoded hex      : ", bepaald::bytesToHexString(decodedframe, decodedframelength));

//     frame.reset(initBackupFrame(decodedframe, decodedframelength, d_framecount + skipped));

//     delete[] decodedframe;

//     ++skipped;
//     if (!frame)
//     {
//       Logger::message_overwrite("Checking if we skipped ", skipped, " frames... nope! :(");
//       //Logger::message("\rChecking if we skipped ", skipped, " frames... nope! :(", std::flush;
//       //if (skipped >
//     }
//     else
//     {
//       if (frame->validate() &&
//           frame->frameType() != BackupFrame::FRAMETYPE::HEADER && // it is impossible to get in this function without the headerframe, and there is only one
//           (frame->frameType() != BackupFrame::FRAMETYPE::END || static_cast<uint64_t>(d_file.tellg()) == d_filesize))
//       {
//         d_counter += skipped;
//         d_framecount += skipped;
//         Logger::message_overwrite("Checking if we skipped ", skipped, " frames... YEAH! :)");
//         //Logger::message("\rChecking if we skipped ", skipped, " frames... YEAH! :)");
//         Logger::message("Good frame: ", frame->frameNumber(), " (", frame->frameTypeString(), ")\nCOUNTER: ", d_counter);
//         frame->printInfo();
//         //delete[] encryptedframe.release();
//         frame.reset();
//         return;
//       }
//       Logger::message_overwrite("Checking if we skipped ", skipped, " frames... nope! :(");
//       frame.reset();
//     }
//   }

//   //frame->printInfo();
//   //Logger::message("HEADERTYPE: ", frame->frameType());

//   uint32_t attsize = 0;
//   if (!d_badmac && (attsize = frame->attachmentSize()) > 0 &&
//       (frame->frameType() == BackupFrame::FRAMETYPE::ATTACHMENT ||
//        frame->frameType() == BackupFrame::FRAMETYPE::AVATAR ||
//        frame->frameType() == BackupFrame::FRAMETYPE::STICKER))
//   {
//     if (d_verbose) [[unlikely]]
//       Logger::message("Trying to read attachment (bruteforce)");

//     uintToFourBytes(d_iv, d_counter++);

//     reinterpret_cast<FrameWithAttachment *>(frame.get())->setLazyData(d_iv, d_iv_size, d_mackey, d_mackey_size, d_cipherkey, d_cipherkey_size, attsize, d_filename, d_file.tellg());

//     d_file.seekg(attsize + MACSIZE, std::ios_base::cur);

//     /*
//     if (!d_lazyload) // immediately decrypt i guess...
//     {
//       if (d_verbose) [[unlikely]]
//         Logger::message("Getting attachment at file pos ", d_file.tellg(), " (size: ", attsize, ")");

//       int getatt = getAttachment(reinterpret_cast<FrameWithAttachment *>(frame.get()));
//       if (getatt != 0)
//       {
//         if (getatt < 0)
//           d_badmac = true;
//         return;
//       }
//     }
//     */
//   }

// }



// #include "../sqlstatementframe/sqlstatementframe.h"

// std::unique_ptr<BackupFrame> FileDecryptor::getFrameStrugee2()
// {
//   long long int filepos = d_file.tellg();

//   if (d_verbose) [[unlikely]]
//     Logger::message("Getting frame at filepos: ", filepos, " (COUNTER: ", d_counter, ")");

//   if (static_cast<uint64_t>(filepos) == d_filesize) [[unlikely]]
//   {
//     Logger::message("Read entire backup file...");
//     return std::unique_ptr<BackupFrame>(nullptr);
//   }

//   if (d_headerframe)
//   {
//     std::unique_ptr<BackupFrame> frame(d_headerframe.release());
//     return frame;
//   }

//   uint32_t encryptedframelength = getNextFrameBlockSize();
//   //if (encryptedframelength > 3145728/*= 3MB*/ /*115343360 / * =110MB*/ || encryptedframelength < 11)
//   //{
//   //  Logger::message("Suspicious framelength");
//   //  bruteForceFrom(filepos)???
//   //}

//   if (encryptedframelength == 0 && d_file.eof()) [[unlikely]]
//   {
//     Logger::message(bepaald::bold_on, "ERROR", bepaald::bold_off, " Unexpectedly hit end of file!");
//     return std::unique_ptr<BackupFrame>(nullptr);
//   }

//   DEBUGOUT("Framelength: ", encryptedframelength);
//   if (d_verbose) [[unlikely]]
//     Logger::message("Framelength: ", encryptedframelength);

//   std::unique_ptr<unsigned char[]> encryptedframe(new unsigned char[encryptedframelength]);
//   if (encryptedframelength > 115343360 /*110MB*/ || encryptedframelength < 11 || !getNextFrameBlock(encryptedframe.get(), encryptedframelength)) [[unlikely]]
//   {
//     Logger::message("Failed to read next frame (", encryptedframelength, " bytes at filepos ", filepos, ")");
//     return std::unique_ptr<BackupFrame>(nullptr);
//   }

//   // check hash
//   unsigned int digest_size = SHA256_DIGEST_LENGTH;
//   unsigned char hash[SHA256_DIGEST_LENGTH];
//   HMAC(EVP_sha256(), d_mackey, d_mackey_size, encryptedframe.get(), encryptedframelength - MACSIZE, hash, &digest_size);
//   if (std::memcmp(encryptedframe.get() + (encryptedframelength - MACSIZE), hash, 10) != 0) [[unlikely]]
//   {
//     Logger::warning("Bad MAC in frame: theirMac: ", bepaald::bytesToHexString(encryptedframe.get() + (encryptedframelength - MACSIZE), MACSIZE));
//     Logger::warning_indent("                    ourMac: ", bepaald::bytesToHexString(hash, SHA256_DIGEST_LENGTH));
//     d_badmac = true;
//     if (d_stoponerror)
//     {
//       Logger::message("Stop reading backup. Next frame would be read at offset ", filepos + encryptedframelength);
//       return std::unique_ptr<BackupFrame>(nullptr);
//     }
//   }
//   else
//   {
//     d_badmac = false;
//     if (d_verbose) [[unlikely]]
//     {
//       Logger::message("Calculated mac: ", bepaald::bytesToHexString(hash, SHA256_DIGEST_LENGTH));
//       Logger::message("Mac in file   : ", bepaald::bytesToHexString(encryptedframe.get() + (encryptedframelength - MACSIZE), MACSIZE));
//     }
//   }

//   // decode
//   uintToFourBytes(d_iv, d_counter++);

//   // create context
//   std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), &::EVP_CIPHER_CTX_free);

//   // disable padding
//   EVP_CIPHER_CTX_set_padding(ctx.get(), 0);

//   if (EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_ctr(), nullptr, d_cipherkey, d_iv) != 1) [[unlikely]]
//   {
//     Logger::message("CTX INIT FAILED");
//     return std::unique_ptr<BackupFrame>(nullptr);
//   }

//   int decodedframelength = encryptedframelength - MACSIZE;
//   unsigned char *decodedframe = new unsigned char[decodedframelength];

//   if (EVP_DecryptUpdate(ctx.get(), decodedframe, &decodedframelength, encryptedframe.get(), encryptedframelength - MACSIZE) != 1) [[unlikely]]
//   {
//     Logger::message("Failed to decrypt data");
//     delete[] decodedframe;
//     return std::unique_ptr<BackupFrame>(nullptr);
//   }

//   delete[] encryptedframe.release(); // free up already....

//   std::unique_ptr<BackupFrame> frame(initBackupFrame(decodedframe, decodedframelength, d_framecount++));

//   if (!frame) [[unlikely]]
//   {
//     Logger::message("Failed to get valid frame from decoded data...");
//     if (d_badmac)
//     {
//       Logger::message("Encrypted data had failed verification (Bad MAC)");
//       delete[] decodedframe;
//       return std::unique_ptr<BackupFrame>(nullptr);
//     }
//     else
//     {
//       Logger::message("Data was verified ok, but does not represent a valid frame... Don't know what happened, but it's bad... :(");
//       Logger::message("Decrypted frame data: ", bepaald::bytesToHexString(decodedframe, decodedframelength));
//       delete[] decodedframe;
//       return std::make_unique<InvalidFrame>();
//     }
//     delete[] decodedframe;
//     return std::unique_ptr<BackupFrame>(nullptr);
//   }

//   delete[] decodedframe;

//   uint32_t attsize = frame->attachmentSize();
//   if (!d_badmac && attsize > 0 &&
//       (frame->frameType() == BackupFrame::FRAMETYPE::ATTACHMENT ||
//        frame->frameType() == BackupFrame::FRAMETYPE::AVATAR ||
//        frame->frameType() == BackupFrame::FRAMETYPE::STICKER))
//   {

//     if ((d_file.tellg() < 0 && d_file.eof()) || (attsize + static_cast<uint64_t>(d_file.tellg()) > d_filesize)) [[unlikely]]
//       if (!d_assumebadframesize)
//       {
//         Logger::error("Unexpectedly hit end of file while reading attachment!");
//         return std::unique_ptr<BackupFrame>(nullptr);
//       }

//     uintToFourBytes(d_iv, d_counter++);

//     reinterpret_cast<FrameWithAttachment *>(frame.get())->setLazyData(d_iv, d_iv_size, d_mackey, d_mackey_size, d_cipherkey, d_cipherkey_size, attsize, d_filename, d_file.tellg());

//     d_file.seekg(attsize + MACSIZE, std::ios_base::cur);

//     /*
//     if (!d_lazyload) // immediately decrypt i guess...
//     {
//       if (d_verbose) [[unlikely]]
//         Logger::message("Getting attachment at file pos ", d_file.tellg(), " (size: ", attsize, ")");

//       int getatt = getAttachment(reinterpret_cast<FrameWithAttachment *>(frame.get())); // 0 == good, >0 == bad, <0 == bad+badmac
//       if (getatt > 0)
//       {
//         Logger::message("Failed to get attachment data for FrameWithAttachment... info:");
//         frame->printInfo();
//         return std::unique_ptr<BackupFrame>(nullptr);
//       }
//       if (getatt < 0)
//       {
//         d_badmac = true;
//         if (d_stoponerror)
//         {
//           Logger::message("Stop reading backup. Next frame would be read at offset ", filepos + encryptedframelength);
//           return std::unique_ptr<BackupFrame>(nullptr);
//         }
//         if (d_assumebadframesize)
//         {
//           std::unique_ptr<BackupFrame> f = bruteForceFrom(filepos, encryptedframelength);
//           //long long int curfilepos = d_file.tellg();
//           //Logger::message("curpso: ", curfilepos);
//           //Logger::message("ATTACHMENT LENGTH SHOULD HAVE BEEN: ", curfilepos - filepos - encryptedframelength - MACSIZE);
//           return f;
//         }
//       }
//     }
//     */

//   }

//   //Logger::message("FILEPOS: ", d_file.tellg());

//   //delete frame;

//   return frame;
// }

// void FileDecryptor::strugee2()
// {

//   d_stoponerror = true;

//   std::vector<std::string> tables;
//   std::string lastmsg;
//   bool endfound = false;

//   std::unique_ptr<BackupFrame> frame(nullptr);
//   while ((frame = getFrameStrugee2()))
//   {
//     if (frame->frameType() == BackupFrame::FRAMETYPE::SQLSTATEMENT)
//     {
//       SqlStatementFrame *s = reinterpret_cast<SqlStatementFrame *>(frame.get());
//       if (s->statement().find("INSERT INTO ") == 0)
//       {
//         // parse table name
//         std::string::size_type pos = s->statement().find(' ', 12);
//         std::string tablename = s->statement().substr(12, pos - 12);

//         if (std::find(tables.begin(), tables.end(), tablename) == tables.end())
//           tables.push_back(tablename);

//         if (tablename == "mms" || tablename == "message" || tablename == "sms")
//           lastmsg = s->statement();
//       }
//     }
//     if (frame->frameType() == BackupFrame::FRAMETYPE::END)
//       endfound = true;
//   }

//   Logger::message("Tables present in backup:");
//   for (unsigned int i = 0; i < tables.size(); ++i)
//     Logger::message(tables[i], ((i == tables.size() - 1) && !endfound ? " (probably incomplete)" : ""));

//   Logger::message("Last message: ", (lastmsg.empty() ? "(none)" : lastmsg));

// }

// void FileDecryptor::strugee3Helper(std::vector<std::pair<std::unique_ptr<unsigned char[]>, uint64_t>> *macs_and_positions)
// {

//   while (true)
//   {
//   //d_file.seekg(0, std::ios_base::beg);
//   long long int filepos = d_file.tellg();

//   //Logger::message("FILEPOS: ", filepos);


//   if (d_verbose) [[unlikely]]
//     Logger::message("Getting frame at filepos: ", filepos, " (COUNTER: ", d_counter, ")");

//   if (static_cast<uint64_t>(filepos) == d_filesize) [[unlikely]]
//   {
//     Logger::message("Read entire backup file...");
//     return;
//   }

//   if (d_headerframe)
//   {
//     std::unique_ptr<BackupFrame> frame(d_headerframe.release());
//     Logger::message("Headerframe");
//     continue;
//   }

//   uint32_t encryptedframelength = getNextFrameBlockSize();
//   //if (encryptedframelength > 3145728/*= 3MB*/ /*115343360 / * =110MB*/ || encryptedframelength < 11)
//   //{
//   //  Logger::message("Suspicious framelength");
//   //  bruteForceFrom(filepos)???
//   //}

//   if (encryptedframelength == 0 && d_file.eof()) [[unlikely]]
//   {
//     Logger::error("Unexpectedly hit end of file!");
//     return;
//   }

//   DEBUGOUT("Framelength: ", encryptedframelength);
//   if (d_verbose) [[unlikely]]
//     Logger::message("Framelength: ", encryptedframelength);

//   std::unique_ptr<unsigned char[]> encryptedframe(new unsigned char[encryptedframelength]);
//   if (encryptedframelength > 115343360 /*110MB*/ || encryptedframelength < 11 || !getNextFrameBlock(encryptedframe.get(), encryptedframelength)) [[unlikely]]
//   {
//     Logger::message("Failed to read next frame (", encryptedframelength, " bytes at filepos ", filepos, ")");
//     return;
//   }

//   // check hash
//   unsigned int digest_size = SHA256_DIGEST_LENGTH;
//   unsigned char hash[SHA256_DIGEST_LENGTH];
//   HMAC(EVP_sha256(), d_mackey, d_mackey_size, encryptedframe.get(), encryptedframelength - MACSIZE, hash, &digest_size);
//   if (std::memcmp(encryptedframe.get() + (encryptedframelength - MACSIZE), hash, 10) != 0) [[unlikely]]
//   {
//     Logger::warning("Bad MAC in frame: theirMac: ", bepaald::bytesToHexString(encryptedframe.get() + (encryptedframelength - MACSIZE), MACSIZE));
//     Logger::warning_indent("                    ourMac: ", bepaald::bytesToHexString(hash, SHA256_DIGEST_LENGTH));
//     d_badmac = true;
//   }
//   else
//   {

//     macs_and_positions->emplace_back(std::make_pair(new unsigned char[SHA256_DIGEST_LENGTH], filepos));
//     std::memcpy(macs_and_positions->back().first.get(), hash, SHA256_DIGEST_LENGTH);

//     d_badmac = false;
//     if (d_verbose) [[unlikely]]
//     {
//       Logger::message("Calculated mac: ", bepaald::bytesToHexString(hash, SHA256_DIGEST_LENGTH));
//       Logger::message("Mac in file   : ", bepaald::bytesToHexString(encryptedframe.get() + (encryptedframelength - MACSIZE), MACSIZE));
//     }
//   }


//   // decode
//   uintToFourBytes(d_iv, d_counter++);

//   // create context
//   std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), &::EVP_CIPHER_CTX_free);

//   // disable padding
//   EVP_CIPHER_CTX_set_padding(ctx.get(), 0);

//   if (EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_ctr(), nullptr, d_cipherkey, d_iv) != 1) [[unlikely]]
//   {
//     Logger::message("CTX INIT FAILED");
//     return;
//   }

//   int decodedframelength = encryptedframelength - MACSIZE;
//   unsigned char *decodedframe = new unsigned char[decodedframelength];

//   if (EVP_DecryptUpdate(ctx.get(), decodedframe, &decodedframelength, encryptedframe.get(), encryptedframelength - MACSIZE) != 1) [[unlikely]]
//   {
//     Logger::message("Failed to decrypt data");
//     delete[] decodedframe;
//     return;
//   }

//   delete[] encryptedframe.release(); // free up already....

//   std::unique_ptr<BackupFrame> frame(initBackupFrame(decodedframe, decodedframelength, d_framecount++));

//   if (!frame) [[unlikely]]
//   {
//     Logger::message("Failed to get valid frame from decoded data...");
//     if (d_badmac)
//     {
//       Logger::message("Encrypted data had failed verification (Bad MAC)");
//       delete[] decodedframe;
//       return;
//     }
//     else
//     {
//       Logger::message("Data was verified ok, but does not represent a valid frame... Don't know what happened, but it's bad... :(");
//       Logger::message("Decrypted frame data: ", bepaald::bytesToHexString(decodedframe, decodedframelength));
//       delete[] decodedframe;
//       return;
//     }
//     delete[] decodedframe;
//     return;
//   }

//   delete[] decodedframe;

//   uint32_t attsize = frame->attachmentSize();
//   if (!d_badmac && attsize > 0 &&
//       (frame->frameType() == BackupFrame::FRAMETYPE::ATTACHMENT ||
//        frame->frameType() == BackupFrame::FRAMETYPE::AVATAR ||
//        frame->frameType() == BackupFrame::FRAMETYPE::STICKER))
//   {

//     if ((d_file.tellg() < 0 && d_file.eof()) || (attsize + static_cast<uint64_t>(d_file.tellg()) > d_filesize)) [[ unlikely ]]
//       if (!d_assumebadframesize)
//       {
//         Logger::error("Unexpectedly hit end of file while reading attachment!");
//         return;
//       }

//     uintToFourBytes(d_iv, d_counter++);

//     reinterpret_cast<FrameWithAttachment *>(frame.get())->setLazyData(d_iv, d_iv_size, d_mackey, d_mackey_size, d_cipherkey, d_cipherkey_size, attsize, d_filename, d_file.tellg());

//     d_file.seekg(attsize + MACSIZE, std::ios_base::cur);

//     /*
//     if (!d_lazyload) // immediately decrypt i guess...
//     {
//       if (d_verbose) [[unlikely]]
//         Logger::message("Getting attachment at file pos ", d_file.tellg(), " (size: ", attsize, ")");

//       int getatt = getAttachment(reinterpret_cast<FrameWithAttachment *>(frame.get())); // 0 == good, >0 == bad, <0 == bad+badmac
//       if (getatt > 0)
//       {
//         Logger::message("Failed to get attachment data for FrameWithAttachment... info:");
//         frame->printInfo();
//         return;
//       }
//       if (getatt < 0)
//       {
//         d_badmac = true;
//         if (d_stoponerror)
//         {
//           Logger::message("Stop reading backup. Next frame would be read at offset ", filepos + encryptedframelength);
//           return;
//         }
//         if (d_assumebadframesize)
//         {
//           std::unique_ptr<BackupFrame> f = bruteForceFrom(filepos, encryptedframelength);
//           //long long int curfilepos = d_file.tellg();
//           //Logger::message("curpso: ", curfilepos);
//           //Logger::message("ATTACHMENT LENGTH SHOULD HAVE BEEN: ", curfilepos - filepos - encryptedframelength - MACSIZE);
//           return;
//         }
//       }
//     }
//     */
//   }







//   }
// }

// void FileDecryptor::strugee3(uint64_t pos)
// {

//   std::vector<std::pair<std::unique_ptr<unsigned char[]>, uint64_t>> macs_and_positions;
//   strugee3Helper(&macs_and_positions);
//   Logger::message("Got macs: ");
//   //for (unsigned int i = 0; i < macs_and_positions.size(); ++i)
//   //  Logger::message(macs_and_positions[i].second, " : ", bepaald::bytesToHexString(macs_and_positions[i].first.get(), SHA256_DIGEST_LENGTH));

//   unsigned int offset = 0;

//   d_file.seekg(pos, std::ios_base::beg);

//   Logger::message("Getting frame at filepos: ", d_file.tellg());

//   if (static_cast<uint64_t>(d_file.tellg()) == d_filesize)
//   {
//     Logger::message("Read entire backup file...");
//     return;
//   }

//   uint32_t encryptedframelength = getNextFrameBlockSize();
//   if (encryptedframelength > 3145728/*= 3MB*/ /*115343360 / * =110MB*/ || encryptedframelength < 11)
//   {
//     Logger::message("Framesize too big to be real");
//     return;
//   }

//   std::unique_ptr<unsigned char[]> encryptedframe(new unsigned char[encryptedframelength]);
//   if (!getNextFrameBlock(encryptedframe.get(), encryptedframelength))
//     return;

//   // check hash
//   unsigned int digest_size = SHA256_DIGEST_LENGTH;
//   unsigned char hash[SHA256_DIGEST_LENGTH];
//   HMAC(EVP_sha256(), d_mackey, d_mackey_size, encryptedframe.get(), encryptedframelength - MACSIZE, hash, &digest_size);
//   if (std::memcmp(encryptedframe.get() + (encryptedframelength - MACSIZE), hash, MACSIZE) != 0)
//   {
//     Logger::message("BAD MAC!");
//     return;
//   }
//   else
//   {
//     Logger::message("");
//     Logger::message("GOT GOOD MAC AT OFFSET ", offset, " BYTES!");
//     Logger::message("Now let's try and find out how many frames we skipped to get here....");
//     d_badmac = false;
//   }




//   Logger::message("Got GOOD MAC : ", bepaald::bytesToHexString(hash, SHA256_DIGEST_LENGTH));
//   for (unsigned int i = 0; i < macs_and_positions.size(); ++i)
//   {
//     if (std::memcmp(macs_and_positions[i].first.get(), hash, SHA256_DIGEST_LENGTH) == 0)
//     {
//       Logger::message("SAME MAC AT POS: ", macs_and_positions[i].second);

//       int const size = 200;
//       unsigned char bytes[size];

//       d_file.seekg(macs_and_positions[i].second);
//       d_file.read(reinterpret_cast<char *>(bytes), size);
//       Logger::message("200 bytes at file position ", macs_and_positions[i].second, ": \n", bepaald::bytesToHexString(bytes, size));

//       d_file.seekg(pos);
//       d_file.read(reinterpret_cast<char *>(bytes), size);
//       Logger::message("200 bytes at file position ", pos, ": \n", bepaald::bytesToHexString(bytes, size));
//     }
//   }
// }

// void FileDecryptor::ashmorgan()
// {
//   unsigned int offset = 0;


//   d_file.seekg(d_filesize - 100, std::ios_base::beg);
//   std::unique_ptr<unsigned char[]> cbytes(new unsigned char[100]);
//   d_file.read(reinterpret_cast<char *>(cbytes.get()), 100);
//   Logger::message("\nLast 100 bytes: ", bepaald::bytesToHexString(cbytes.get(), 100), "\n");
//   cbytes.release();



//   d_file.seekg(d_filesize - 16, std::ios_base::beg);

//   Logger::message("Getting frame at filepos: ", d_file.tellg());

//   if (static_cast<uint64_t>(d_file.tellg()) == d_filesize)
//   {
//     Logger::message("Read entire backup file...");
//     return;
//   }

//   uint32_t encryptedframelength = getNextFrameBlockSize();
//   if (encryptedframelength > 3145728/*= 3MB*/ /*115343360 / * =110MB*/ || encryptedframelength < 11)
//   {
//     Logger::message("Framesize too big to be real");
//     return;
//   }

//   std::unique_ptr<unsigned char[]> encryptedframe(new unsigned char[encryptedframelength]);
//   if (!getNextFrameBlock(encryptedframe.get(), encryptedframelength))
//     return;

//   Logger::message("FRAME: ", bepaald::bytesToHexString(encryptedframe.get(), encryptedframelength));

//   // check hash
//   unsigned int digest_size = SHA256_DIGEST_LENGTH;
//   unsigned char hash[SHA256_DIGEST_LENGTH];
//   HMAC(EVP_sha256(), d_mackey, d_mackey_size, encryptedframe.get(), encryptedframelength - MACSIZE, hash, &digest_size);
//   if (std::memcmp(encryptedframe.get() + (encryptedframelength - MACSIZE), hash, MACSIZE) != 0)
//   {
//     Logger::message("BAD MAC!");
//     return;
//   }
//   else
//   {
//     Logger::message("");
//     Logger::message("GOT GOOD MAC AT OFFSET ", offset, " BYTES! ", bepaald::bytesToHexString(hash, digest_size));
//     Logger::message("Now let's try and find out how many frames we skipped to get here....");
//     d_badmac = false;
//   }

//   // decode
//   unsigned int skipped = 0;
//   std::unique_ptr<BackupFrame> frame(nullptr);
//   while (true)
//   {
//     if (frame)
//       frame.reset();

//     if (skipped > 1000000) // a frame is at least 10 bytes? -> could probably safely set this higher. MAC alone is 10 bytes, there is also actual data
//     {
//       Logger::message("TESTED 1000000 frames");
//       return;
//     }

//     if (skipped % 100 == 0)
//       Logger::message_overwrite("\rChecking if we skipped ", skipped, " frames... ");

//     uintToFourBytes(d_iv, d_counter + skipped);

//     // create context
//     std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), &::EVP_CIPHER_CTX_free);

//     // disable padding
//     EVP_CIPHER_CTX_set_padding(ctx.get(), 0);

//     if (EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_ctr(), nullptr, d_cipherkey, d_iv) != 1)
//     {
//       Logger::message("CTX INIT FAILED");
//       return;
//     }

//     int decodedframelength = encryptedframelength - MACSIZE;
//     unsigned char *decodedframe = new unsigned char[decodedframelength];

//     if (EVP_DecryptUpdate(ctx.get(), decodedframe, &decodedframelength, encryptedframe.get(), encryptedframelength - MACSIZE) != 1)
//     {
//       Logger::message("Failed to decrypt data");
//       delete[] decodedframe;
//       return;
//     }

//     DEBUGOUT("Decoded hex      : ", bepaald::bytesToHexString(decodedframe, decodedframelength));

//     frame.reset(initBackupFrame(decodedframe, decodedframelength, d_framecount + skipped));

//     ++skipped;
//     if (!frame)
//     {
//       Logger::message_overwrite("\rChecking if we skipped ", skipped, " frames... nope! :(");
//       //if (skipped >
//     }
//     else
//     {
//       if (frame->validate() &&
//           frame->frameType() != BackupFrame::FRAMETYPE::HEADER && // it is impossible to get in this function without the headerframe, and there is only one
//           (frame->frameType() != BackupFrame::FRAMETYPE::END || static_cast<uint64_t>(d_file.tellg()) == d_filesize))
//       {
//         d_counter += skipped;
//         d_framecount += skipped;
//         Logger::message_overwrite("\rChecking if we skipped ", skipped, " frames... YEAH! :)", Logger::Control::ENDOVERWRITE);
//         Logger::message("Good frame: ", frame->frameNumber(), " (", frame->frameTypeString(), ")");
//         Logger::message("COUNTER: ", d_counter);
//         Logger::message("Decoded hex      : ", bepaald::bytesToHexString(decodedframe, decodedframelength));
//         frame->printInfo();
//         //delete[] encryptedframe.release();
//         frame.reset();

//         delete[] decodedframe;
//         return;
//       }
//       Logger::message_overwrite("\rChecking if we skipped ", skipped, " frames... nope! :(");
//       frame.reset();
//     }
//     delete[] decodedframe;
//   }

//   //frame->printInfo();
//   //Logger::message("HEADERTYPE: ", frame->frameType());

//   uint32_t attsize = 0;
//   if (!d_badmac && (attsize = frame->attachmentSize()) > 0 &&
//       (frame->frameType() == BackupFrame::FRAMETYPE::ATTACHMENT ||
//        frame->frameType() == BackupFrame::FRAMETYPE::AVATAR ||
//        frame->frameType() == BackupFrame::FRAMETYPE::STICKER))
//   {
//     if (d_verbose) [[unlikely]]
//       Logger::message("Trying to read attachment (bruteforce)");

//     uintToFourBytes(d_iv, d_counter++);

//     reinterpret_cast<FrameWithAttachment *>(frame.get())->setLazyData(d_iv, d_iv_size, d_mackey, d_mackey_size, d_cipherkey, d_cipherkey_size, attsize, d_filename, d_file.tellg());

//     d_file.seekg(attsize + MACSIZE, std::ios_base::cur);

//     /*
//     if (!d_lazyload) // immediately decrypt i guess...
//     {
//       if (d_verbose) [[unlikely]]
//         Logger::message("Getting attachment at file pos ", d_file.tellg(), " (size: ", attsize, ")");

//       int getatt = getAttachment(reinterpret_cast<FrameWithAttachment *>(frame.get()));
//       if (getatt != 0)
//       {
//         if (getatt < 0)
//           d_badmac = true;
//         return;
//       }
//     }
//     */
//   }

// }
