/*
  Copyright (C) 2021-2022  Selwin van Dijk

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

/*
 * Things to deal with:
 * get proper thread from id (phone number)
 * deal with type, in what way do we assume incoming/outgoing is specified in the csv
 * should imported messages be marked as received / read
 * should they be imported as secured or unsecured messages?
 */

bool SignalBackup::importCSV(std::string const &file, std::map<std::string, std::string> const &fieldmap)
{
  CSVReader csvfile(file);
  if (!csvfile.ok())
    return false;

  std::string statementstub("INSERT INTO sms SET (");

  int64_t idx_of_address = -1;
  //int64_t idx_of_type = -1;
  std::vector<unsigned int> date_indeces;

  // get columns to set
  for (uint i = 0; i < csvfile.fields(); ++i)
  {
    std::string fieldname = csvfile.getFieldName(i);
    if (fieldmap.find(fieldname) != fieldmap.end())// (fieldmap.contains(fieldname))
      fieldname = fieldmap.at(fieldname);

    if (fieldname == "address")
      idx_of_address = i;
    else if (fieldname == "type")
      ;//idx_of_type = i;
    else if (fieldname.find("date") != std::string::npos)
      date_indeces.push_back(i);

    statementstub += fieldname + ',';
  }
  statementstub += "thread_id) VALUES (";

  // build statement from each row
  for (uint msg = 0; msg < csvfile.rows(); ++msg)
  {
    std::string statement = statementstub;
    for (uint f = 0; f < csvfile.fields(); ++f)
    {
      //if (f == idx_of_type)
      //  translate type?

      //if (date_indeces.contains(f))
      //{
      //  std::string date = csvfile.get(f, msg);
      //  if (date.find_first_not_of("0123456789") != std::string::npos)
      //    translate(date);
      //}

      statement += csvfile.get(f, msg) + ',';
    }

    // determine thread_id
    long long int tid = getThreadIdFromRecipient(csvfile.get(idx_of_address, msg));
    if (tid == -1)
    {
      std::cout << bepaald::bold_on << "ERROR" << bepaald::bold_off
                << " Unable to determine thread_id for message." << std::endl;
      return false;
    }
    statement += bepaald::toString(tid);



    statement += ')';

    if (!d_database.exec(statement))
      return false;

  }
  return true;
}
