/*
  Copyright (C) 2019-2024  Selwin van Dijk

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

#ifndef BASEDECRYPTOR_H_
#define BASEDECRYPTOR_H_

#include "../cryptbase/cryptbase.h"

class FrameWithAttachment;

class BaseDecryptor : public CryptBase
{
 protected:
  bool d_verbose;
 public:
  inline BaseDecryptor(bool verbose);
  inline BaseDecryptor(BaseDecryptor const &other);
  inline BaseDecryptor &operator=(BaseDecryptor const &other);
  inline BaseDecryptor(BaseDecryptor &&other);
  inline BaseDecryptor &operator=(BaseDecryptor &&other);
  static int getAttachment(FrameWithAttachment *frame, bool verbose = false);
};

inline BaseDecryptor::BaseDecryptor(bool verbose)
  :
  d_verbose(verbose)
{}

inline BaseDecryptor::BaseDecryptor(BaseDecryptor const &other)
  :
  CryptBase(other),
  d_verbose(other.d_verbose)
{}

inline BaseDecryptor &BaseDecryptor::operator=(BaseDecryptor const &other)
{
  if (this != &other)
  {
    CryptBase::operator=(other);
    d_verbose = other.d_verbose;
  }
  return *this;
}

inline BaseDecryptor::BaseDecryptor(BaseDecryptor &&other)
  :
  CryptBase(std::move(other)),
  d_verbose(std::move(other.d_verbose))
{}

inline BaseDecryptor &BaseDecryptor::operator=(BaseDecryptor &&other)
{
  if (this != &other)
  {
    CryptBase::operator=(std::move(other));
    d_verbose = std::move(other.d_verbose);
  }
  return *this;
}

#endif
