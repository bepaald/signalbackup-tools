/*
  Copyright (C) 2023-2024  Selwin van Dijk

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

#include "signalbackup.ih"
#include "dtsharedcontactstruct.h"


        // On android message.shared_contacts is a json string looking like:
        /*
          [
            {
            "name":
              {
                "displayName":"Selwin van Dijk",
                "givenName":"Selwin",
                "familyName":"van Dijk",
                "prefix":null,
                "suffix":null,
                "middleName":null,
                "empty":false
              },
            "organization":null,           <--------------------------------------------------- never seen this filled
            "phoneNumbers":[
              {
                "number":"0201234567",
                "type":"HOME",
                "label":"home"
              },
              {
                "number":"0612345678",
                "type":"MOBILE",
                "label":"cell"
              }
            ],
            "emails":[
               {
                 "email":"balabla@something.fake",
                 "type":"WORK",
                 "label":"work"
               },
               {
                 "email":"anotherfake@email.address",
                 "type":"HOME",
                 "label":"home"
               }
             ],
             "postalAddresses":[],           <--------------------------------------------------- never seen this filled
             "avatar":
               {
                 "attachmentId":null, / {"rowId":5318,"uniqueId":1660632296881,"valid":true}
                 "isProfile":false,          <--------------------------------------------------- never seen !false
                 "profile":false             <--------------------------------------------------- never seen !false
               }
             }
           ]
         */



/*
In android:

message.shared_contacts = [{"name":{"displayName":"Fake Jay Contact, Sr.","givenName":"Fake","familyName":"Contact","prefix":null,"suffix":"Sr","middleName":null,"empty":false},"organization":null,"phoneNumbers":[{"number":"0611112222","type":"MOBILE","label":"cell"},{"number":"0505411111","type":"HOME","label":"home"},{"number":"08000611","type":"WORK","label":"work"},{"number":"000000000","type":"CUSTOM","label":"customphonefield"}],"emails":[{"email":"home@home.hh","type":"HOME","label":"home"},{"email":"wor@work.ww","type":"WORK","label":"work"},{"email":"other@other.oo","type":"CUSTOM","label":null},{"email":"custom@custom.cc","type":"CUSTOM","label":"customfield"}],"postalAddresses":[],"avatar":{"attachmentId":null,"isProfile":false,"profile":false}}]


On Desktop:

json_extract(json, '$.contact') = [{"name":{"givenName":"Fake","familyName":"Contact","suffix":"Sr","displayName":"Fake Jay Contact, Sr."},"number":[{"value":"+31611112222","type":2,"label":"cell"},{"value":"+31505411111","type":1,"label":"home"},{"value":"+318000611","type":3,"label":"work"},{"value":"000000000","type":4,"label":"customphonefield"}],"email":[{"value":"home@home.hh","type":1,"label":"home"},{"value":"wor@work.ww","type":3,"label":"work"},{"value":"other@other.oo","type":4},{"value":"custom@custom.cc","type":4,"label":"customfield"}]}]

*/



std::string SignalBackup::dtSetSharedContactsJsonString(SqliteDB const &ddb, long long int rowid) const
{

  //*** gather data from Desktop database ***//
  SqliteDB::QueryResults dtsc;
  ddb.exec("SELECT "
           "json_extract(json, '$.contact[0].name.givenName') AS givenName, "
           "json_extract(json, '$.contact[0].name.familyName') AS familyName, "
           "json_extract(json, '$.contact[0].name.suffix') AS suffix, "
           "json_extract(json, '$.contact[0].name.displayName') AS displayName, "
           "json_extract(json, '$.contact[0].name.prefix') AS prefix, "
           "json_extract(json, '$.contact[0].name.middleName') AS middleName, "
           "IFNULL(json_array_length(json, '$.contact[0].number'), 0) AS numphones, "
           "IFNULL(json_array_length(json, '$.contact[0].email'), 0) AS numemails "
           "FROM messages WHERE rowid = ?", rowid, &dtsc);
  //dtsc.prettyPrint();



  //*** save data to structure ***//
  SharedContactData scd;

  if (!dtsc.isNull(0, "givenName"))
    scd.name.givenName = dtsc("givenName");
  if (!dtsc.isNull(0, "familyName"))
    scd.name.familyName = dtsc("familyName");
  if (!dtsc.isNull(0, "suffix"))
    scd.name.suffix = dtsc("suffix");
  if (!dtsc.isNull(0, "prefix"))
    scd.name.prefix = dtsc("prefix");
  if (!dtsc.isNull(0, "displayName"))
    scd.name.displayName = dtsc("displayName");
  if (!dtsc.isNull(0, "middleName"))
    scd.name.middleName = dtsc("middleName");

  scd.name.empty = (!scd.name.givenName.has_value() && !scd.name.familyName.has_value() &&
                    !scd.name.displayName.has_value() && !scd.name.middleName.has_value());

  for (unsigned int i = 0; i < dtsc.valueAsInt(0, "numphones", 0); ++i)
  {
    SqliteDB::QueryResults dtsc_array;
    ddb.exec("SELECT "
             "json_extract(json, '$.contact[0].number[" + bepaald::toString(i) + "].value') AS number, "
             "json_extract(json, '$.contact[0].number[" + bepaald::toString(i) + "].type') AS type, "
             "json_extract(json, '$.contact[0].number[" + bepaald::toString(i) + "].label') AS label "
             "FROM messages WHERE rowid = ?", rowid, &dtsc_array);

    SharedContactDataPhone tmp;
    if (!dtsc_array.isNull(0, "number"))
      tmp.number = dtsc_array("number");
    if (!dtsc_array.isNull(0, "label"))
      tmp.label = dtsc_array("label");
    int type = dtsc_array.valueAsInt(0, "type", -1);
    switch (type)
    {
      case 1:
        tmp.type = "HOME"s;
        break;
      case 2:
        tmp.type = "MOBILE"s;
        break;
      case 3:
        tmp.type = "WORK"s;
        break;
      case 4:
        tmp.type = "CUSTOM"s;
        break;
      default:
        tmp.type = "CUSTOM"s;
        break;
    }
    scd.phoneNumbers.emplace_back(std::move(tmp));
  }


  for (unsigned int i = 0; i < dtsc.valueAsInt(0, "numemails", 0); ++i)
  {
    SqliteDB::QueryResults dtsc_array;
    ddb.exec("SELECT "
             "json_extract(json, '$.contact[0].email[" + bepaald::toString(i) + "].value') AS email, "
             "json_extract(json, '$.contact[0].email[" + bepaald::toString(i) + "].type') AS type, "
             "json_extract(json, '$.contact[0].email[" + bepaald::toString(i) + "].label') AS label "
             "FROM messages WHERE rowid = ?", rowid, &dtsc_array);

    SharedContactDataEmail tmp;
    if (!dtsc_array.isNull(0, "email"))
      tmp.email = dtsc_array("email");
    if (!dtsc_array.isNull(0, "label"))
      tmp.label = dtsc_array("label");
    int type = dtsc_array.valueAsInt(0, "type", -1);
    switch (type)
    {
      case 1:
        tmp.type = "HOME"s;
        break;
      case 2:
        tmp.type = "MOBILE"s; // ????
        break;
      case 3:
        tmp.type = "WORK"s;
        break;
      case 4:
        tmp.type = "CUSTOM"s;
        break;
      default:
        tmp.type = "CUSTOM"s;
        break;
    }
    scd.emails.emplace_back(std::move(tmp));
  }


  //*** set data in new json string for insertion into android db ***//
  std::string jsonstring =
    ddb.getSingleResultAs<std::string>("SELECT json_array("
                                              "json_object('name', json_object('displayName', ?, "
                                                                              "'givenName', ?, "
                                                                              "'familyName', ?, "
                                                                              "'prefix', ?, "
                                                                              "'suffix', ?, "
                                                                              "'middleName', ?, "
                                                                              "'empty', "s + (scd.name.empty ? "json('true')" : "json('false')") + "), "
                                                          "'organization', ?, "
                                                          "'phoneNumbers', json_array(), "
                                                          "'emails', json_array(), "
                                                          "'postalAddresses', json_array(), "
                                                          //"'avatar', json_object('attachmentId', json_object('rowId', '', 'uniqueId', '', 'valid', json(true)), " // not yet, this requires attachment stuff..
                                                                                                                                                                    // plus never seen in Desktop db
                                                          "'avatar', json_object('attachmentId', json('null'), "                                                      // so for now, leave empty
                                                                                "'isProfile', json('false'), "
                                                                                "'profile', json('false'))"
                                              "))",
                                       {scd.name.displayName,
                                        scd.name.givenName,
                                        scd.name.familyName,
                                        scd.name.prefix,
                                        scd.name.suffix,
                                        scd.name.middleName,
                                        nullptr // scd.organization
                                       },
                                       std::string());
  //std::cout << jsonstring << std::endl;
  if (jsonstring.empty()) // above failed, the rest will also fail...
    return jsonstring;

  // add phonenumbers
  for (unsigned int p = 0; p < scd.phoneNumbers.size(); ++p)
    jsonstring = ddb.getSingleResultAs<std::string>("SELECT json_insert(?, '$[0].phoneNumbers[#]', json_object('number', ?, 'type', ?, 'label', ?))",
                                                    {jsonstring,
                                                     scd.phoneNumbers[p].number,
                                                     scd.phoneNumbers[p].type,
                                                     scd.phoneNumbers[p].label},
                                                    std::string());
  if (jsonstring.empty()) // above failed, the rest will also fail...
    return jsonstring;

  // add emails
  for (unsigned int p = 0; p < scd.emails.size(); ++p)
    jsonstring = ddb.getSingleResultAs<std::string>("SELECT json_insert(?, '$[0].emails[#]', json_object('email', ?, 'type', ?, 'label', ?))",
                                                    {jsonstring,
                                                     scd.emails[p].email,
                                                     scd.emails[p].type,
                                                     scd.emails[p].label},
                                                    std::string());

  return jsonstring;
}
