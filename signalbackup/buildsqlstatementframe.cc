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

#include "signalbackup.ih"

SqlStatementFrame SignalBackup::buildSqlStatementFrame(std::string const &table, std::vector<std::string> const &headers, std::vector<std::any> const &result) const
{
  //std::cout << "Building new frame:" << std::endl;

  SqlStatementFrame NEWFRAME;
  std::string newstatement = "INSERT INTO " + table + " (";

  for (uint i = 0; i < headers.size(); ++i)
  {
    newstatement.append(headers[i]);
    if (i < headers.size() - 1)
      newstatement.append(",");
    else
      newstatement.append(")");
  }
  newstatement += " VALUES (";

  for (uint j = 0; j < result.size(); ++j)
  {
    if (j < result.size() - 1)
      newstatement.append("?,");
    else
      newstatement.append("?)");
    if (result[j].type() == typeid(nullptr))
      NEWFRAME.addNullParameter();
    else if (result[j].type() == typeid(long long int))
      NEWFRAME.addIntParameter(std::any_cast<long long int>(result[j]));
    else if (result[j].type() == typeid(std::string))
      NEWFRAME.addStringParameter(std::any_cast<std::string>(result[j]));
    else if (result[j].type() == typeid(std::pair<std::shared_ptr<unsigned char []>, size_t>))
      NEWFRAME.addBlobParameter(std::any_cast<std::pair<std::shared_ptr<unsigned char []>, size_t>>(result[j]));
    else if (result[j].type() == typeid(double))
      NEWFRAME.addDoubleParameter(std::any_cast<double>(result[j]));
    else
      std::cout << "WARNING : UNHANDLED PARAMETER TYPE = " << result[j].type().name() << std::endl;
  }
  NEWFRAME.setStatementField(newstatement);

  //NEWFRAME.printInfo();

  //std::exit(0);

  return NEWFRAME;
}

SqlStatementFrame SignalBackup::buildSqlStatementFrame(std::string const &table, std::vector<std::any> const &result) const
{
  //std::cout << "Building new frame:" << std::endl;

  SqlStatementFrame NEWFRAME;
  std::string newstatement = "INSERT INTO " + table + " VALUES (";
  for (uint j = 0; j < result.size(); ++j)
  {
    if (j < result.size() - 1)
      newstatement.append("?,");
    else
      newstatement.append("?)");
    if (result[j].type() == typeid(nullptr))
      NEWFRAME.addNullParameter();
    else if (result[j].type() == typeid(long long int))
      NEWFRAME.addIntParameter(std::any_cast<long long int>(result[j]));
    else if (result[j].type() == typeid(std::string))
      NEWFRAME.addStringParameter(std::any_cast<std::string>(result[j]));
    else if (result[j].type() == typeid(std::pair<std::shared_ptr<unsigned char []>, size_t>))
      NEWFRAME.addBlobParameter(std::any_cast<std::pair<std::shared_ptr<unsigned char []>, size_t>>(result[j]));
    else if (result[j].type() == typeid(double))
      NEWFRAME.addDoubleParameter(std::any_cast<double>(result[j]));
    else
      std::cout << "WARNING : UNHANDLED PARAMETER TYPE = " << result[j].type().name() << std::endl;
  }
  NEWFRAME.setStatementField(newstatement);

  //NEWFRAME.printInfo();

  //std::exit(0);

  return NEWFRAME;
}
