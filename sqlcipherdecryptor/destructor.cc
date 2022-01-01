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

#include "sqlcipherdecryptor.ih"

SqlCipherDecryptor::~SqlCipherDecryptor()
{
  bepaald::destroyPtr(&d_key, &d_keysize);
  bepaald::destroyPtr(&d_hmackey, &d_hmackeysize);
  bepaald::destroyPtr(&d_salt, &d_saltsize);
  bepaald::destroyPtr(&d_decrypteddata, &d_decrypteddatasize);
#ifdef USE_CRYPTOPP
  if (d_hmac)
    delete d_hmac;
  if (d_pbkdf)
    delete d_pbkdf;
#endif
}
