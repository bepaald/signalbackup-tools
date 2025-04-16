/*
  Copyright (C) 2020-2025  Selwin van Dijk

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

#ifndef INVALIDFRAME_H_
#define INVALIDFRAME_H_

#include "../backupframe/backupframe.h"

class InvalidFrame : public BackupFrame
{
  static Registrar s_registrar;
 public:
  inline explicit InvalidFrame(uint64_t count = 0);
  inline InvalidFrame(InvalidFrame &&other) noexcept = default;
  inline InvalidFrame &operator=(InvalidFrame &&other) noexcept = default;
  inline InvalidFrame(InvalidFrame const &other) = default;
  inline InvalidFrame &operator=(InvalidFrame const &other) = default;
  inline virtual ~InvalidFrame() override = default;
  inline virtual InvalidFrame *clone() const override;
  inline virtual InvalidFrame *move_clone() override;
  inline virtual FRAMETYPE frameType() const override;
  inline virtual void printInfo() const override;
};

inline InvalidFrame::InvalidFrame(uint64_t count)
  :
  BackupFrame(count)
{}

inline InvalidFrame *InvalidFrame::clone() const
{
  return new InvalidFrame(*this);
}

inline InvalidFrame *InvalidFrame::move_clone()
{
  return new InvalidFrame(std::move(*this));
}

inline BackupFrame::FRAMETYPE InvalidFrame::frameType() const
{
  return FRAMETYPE::INVALID;
}

inline void InvalidFrame::printInfo() const
{
  Logger::message("Frame number: ", d_count);
  Logger::message("        Type: INVALID");
}

#endif
