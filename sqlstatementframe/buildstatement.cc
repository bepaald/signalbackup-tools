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


void SqlStatementFrame::buildStatement()
{
  for (auto const &p : d_framedata)
    if (std::get<0>(p) == FIELD::STATEMENT)
    {
      d_statement = bepaald::bytesToString(std::get<1>(p), std::get<2>(p));
      if (d_parameterdata.size() == 0) // only a statement was given, no parameters to insert...
        return;
    }

  // check if number of parameters equals number of '?'
  if (std::count(d_statement.begin(), d_statement.end(), '?') != static_cast<int>(d_parameterdata.size()))
  {
    std::cout << "Bad subst count: " << "Statement: " << d_statement << " parameters: " << d_parameterdata.size() << std::endl;
    d_statement.clear();
    return;
  }

  std::string::size_type pos = 0;
  for (auto const &p : d_parameterdata)
  {
    pos = d_statement.find('?', pos);
    if (pos == std::string::npos)
    {
      DEBUGOUT("Fail to find '?'");
      d_statement.clear();
      return;
    }

    switch (std::get<0>(p))
    {
    case PARAMETER_FIELD::INT:
      {
        d_statement.replace(pos, 1, std::to_string(static_cast<int64_t>(bytesToUint64(std::get<1>(p), std::get<2>(p)))));
        break;
      }
    case PARAMETER_FIELD::STRING:
      {
        std::string rep = bepaald::bytesToString(std::get<1>(p), std::get<2>(p));
        std::string::size_type pos2 = 0;
        while ((pos2 = rep.find('\'', pos2)) != std::string::npos)
        {
          rep.replace(pos2, 1, "''");
          pos2 += 2;
        }
        rep = '\'' + rep + '\'';

        d_statement.replace(pos, 1, rep);
        pos += rep.length();
        break;
      }
    case PARAMETER_FIELD::DOUBLE:
      {
        std::stringstream ss;
        ss.imbue(std::locale(std::locale(), new Period)); // make sure we get periods as decimal indicators
        ss << std::defaultfloat << std::setprecision(17) << *reinterpret_cast<double *>(std::get<1>(p));
        d_statement.replace(pos, 1, ss.str());
        break;
      }
    case PARAMETER_FIELD::BLOB:
      {
        d_statement.replace(pos, 1, "X'" + bepaald::bytesToHexString(std::get<1>(p), std::get<2>(p), true) + '\'');
        break;
      }
    case PARAMETER_FIELD::NULLPARAMETER:
      {
        d_statement.replace(pos, 1, "NULL");
        break;
      }
    }
  }
}
