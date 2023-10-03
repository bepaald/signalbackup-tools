/*
  Copyright (C) 2023  Selwin van Dijk

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

#ifndef DTSHARED_CONTACTSTRUCT
#define DTSHARED_CONTACTSTRUCT

#include <string>
#include <optional>
#include <any>

struct SharedContactDataName
{
  std::any displayName = nullptr;
  std::any givenName = nullptr;
  std::any familyName = nullptr;
  std::any prefix = nullptr;
  std::any suffix = nullptr;
  std::any middleName = nullptr;
  bool empty;
};

struct SharedContactDataPhone
{
  std::any number = nullptr;
  std::any type = nullptr;
  std::any label = nullptr;
};

struct SharedContactDataEmail
{
  std::any email = nullptr;
  std::any type = nullptr;
  std::any label = nullptr;
};

struct SharedContactDataOrganization // ???
{
};

struct SharedContactDataPostalAddress // ???
{
};

struct SharedContactDataAttachmentId
{
  long long int rowId;
  long long int uniqueId;
  bool valid;
};

struct SharedContactDataAvatar
{
  std::optional<SharedContactDataAttachmentId> attachmentId;
  bool isProfile;
  bool profile;
};

struct SharedContactData {
  SharedContactDataName name;
  std::optional<SharedContactDataOrganization> organization;
  std::vector<SharedContactDataPhone> phoneNumbers;
  std::vector<SharedContactDataEmail> emails;
  std::vector<SharedContactDataPostalAddress> postalAddresses;
  SharedContactDataAvatar avatar;
};

#endif
