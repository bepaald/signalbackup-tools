/*
  Copyright (C) 2024  Selwin van Dijk

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

#include "../../filedecryptor/filedecryptor.h"
#include "../../backupframe/backupframe.h"

#include <fstream>
#include <memory>

int main()
{

  {
    FileDecryptor fd("DEVsignal-2023-12-31-11-13-42.backup", "000000000000000000000000000000",
                     false, false, false, std::vector<long long int>());
    if (!fd.ok())
    {
      std::cout << "ERROR CREATING DECRYPTOR" << std::endl;
      return 1;
    }

    std::ifstream file("DEVsignal-2023-12-31-11-13-42.backup", std::ios_base::binary | std::ios_base::in);
    std::unique_ptr<BackupFrame> frame;
    frame = fd.getFrame(file);
    frame = fd.getFrame(file);
    auto pos_fd = file.tellg();
    if (!frame)
    {
      std::cout << "ERROR GETTING FRAME!" << std::endl;
      return 1;
    }
    std::cout << frame->frameNumber() << std::endl;
    std::cout << frame->dataSize() << std::endl;

    //std::cout << fd.frameCount() << std::endl;
    {
      // copy
      FileDecryptor fd2(fd);
      if (!fd2.ok())
      {
        std::cout << "ERROR CREATING DECRYPTOR" << std::endl;
        return 1;
      }
      frame = fd2.getFrame(file);
      frame = fd2.getFrame(file);
      if (!frame)
      {
        std::cout << "ERROR GETTING FRAME!" << std::endl;
        return 1;
      }
      std::cout << frame->frameNumber() << std::endl;
      std::cout << frame->dataSize() << std::endl;
      auto pos_fd2 = file.tellg();

      // assign
      FileDecryptor fd3 = fd2;
      if (!fd3.ok())
      {
        std::cout << "ERROR CREATING DECRYPTOR" << std::endl;
        return 1;
      }
      frame = fd3.getFrame(file);
      frame = fd3.getFrame(file);
      frame = fd3.getFrame(file);
      if (!frame)
      {
        std::cout << "ERROR GETTING FRAME!" << std::endl;
        return 1;
      }
      std::cout << frame->frameNumber() << std::endl;
      std::cout << frame->dataSize() << std::endl;
      auto pos_fd3 = file.tellg();

      // move
      FileDecryptor fd4(std::move(fd2));
      if (!fd4.ok())
      {
        std::cout << "ERROR CREATING DECRYPTOR" << std::endl;
        return 1;
      }
      file.seekg(pos_fd2);
      frame = fd4.getFrame(file);
      if (!frame)
      {
        std::cout << "ERROR GETTING FRAME!" << std::endl;
        return 1;
      }
      std::cout << frame->frameNumber() << std::endl;
      std::cout << frame->dataSize() << std::endl;

      fd4 = std::move(fd3);
      if (!fd4.ok())
      {
        std::cout << "ERROR CREATING DECRYPTOR" << std::endl;
        return 1;
      }
      file.seekg(pos_fd3);
      frame = fd4.getFrame(file);
      if (!frame)
      {
        std::cout << "ERROR GETTING FRAME!" << std::endl;
        return 1;
      }
      std::cout << frame->frameNumber() << std::endl;
      std::cout << frame->dataSize() << std::endl;

    }
    file.seekg(pos_fd);
    frame = fd.getFrame(file);
    if (!frame)
    {
      std::cout << "ERROR GETTING FRAME!" << std::endl;
      return 1;
    }
    std::cout << frame->frameNumber() << std::endl;
    std::cout << frame->dataSize() << std::endl;
  }




  // test before taking headerframe: same thing without calling getFrame on fd initially
  {
    FileDecryptor fd("DEVsignal-2023-12-31-11-13-42.backup", "000000000000000000000000000000",
                     false, false, false, std::vector<long long int>());
    if (!fd.ok())
    {
      std::cout << "ERROR CREATING DECRYPTOR" << std::endl;
      return 1;
    }

    std::ifstream file("DEVsignal-2023-12-31-11-13-42.backup", std::ios_base::binary | std::ios_base::in);
    std::unique_ptr<BackupFrame> frame;
    auto pos_fd = file.tellg();
    {
      // copy
      FileDecryptor fd2(fd);
      if (!fd2.ok())
      {
        std::cout << "ERROR CREATING DECRYPTOR" << std::endl;
        return 1;
      }
      frame = fd2.getFrame(file);
      frame = fd2.getFrame(file);
      if (!frame)
      {
        std::cout << "ERROR GETTING FRAME!" << std::endl;
        return 1;
      }
      std::cout << frame->frameNumber() << std::endl;
      std::cout << frame->dataSize() << std::endl;
      auto pos_fd2 = file.tellg();

      // assign
      FileDecryptor fd3 = fd2;
      if (!fd3.ok())
      {
        std::cout << "ERROR CREATING DECRYPTOR" << std::endl;
        return 1;
      }
      frame = fd3.getFrame(file);
      frame = fd3.getFrame(file);
      frame = fd3.getFrame(file);
      if (!frame)
      {
        std::cout << "ERROR GETTING FRAME!" << std::endl;
        return 1;
      }
      std::cout << frame->frameNumber() << std::endl;
      std::cout << frame->dataSize() << std::endl;
      auto pos_fd3 = file.tellg();

      // move
      FileDecryptor fd4(std::move(fd2));
      if (!fd4.ok())
      {
        std::cout << "ERROR CREATING DECRYPTOR" << std::endl;
        return 1;
      }
      file.seekg(pos_fd2);
      frame = fd4.getFrame(file);
      if (!frame)
      {
        std::cout << "ERROR GETTING FRAME!" << std::endl;
        return 1;
      }
      std::cout << frame->frameNumber() << std::endl;
      std::cout << frame->dataSize() << std::endl;

      fd4 = std::move(fd3);
      if (!fd4.ok())
      {
        std::cout << "ERROR CREATING DECRYPTOR" << std::endl;
        return 1;
      }
      file.seekg(pos_fd3);
      frame = fd4.getFrame(file);
      if (!frame)
      {
        std::cout << "ERROR GETTING FRAME!" << std::endl;
        return 1;
      }
      std::cout << frame->frameNumber() << std::endl;
      std::cout << frame->dataSize() << std::endl;

    }
    file.seekg(pos_fd);
    frame = fd.getFrame(file);
    if (!frame)
    {
      std::cout << "ERROR GETTING FRAME!" << std::endl;
      return 1;
    }
    std::cout << frame->frameNumber() << std::endl;
    std::cout << frame->dataSize() << std::endl;
  }

  return 0;
}
