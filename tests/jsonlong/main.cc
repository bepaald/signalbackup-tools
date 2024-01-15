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

#include <iostream>

#include "../../memsqlitedb/memsqlitedb.h"

int main()
{
  bool results = true;

  MemSqliteDB temp;

  std::string json("{\"high\":350,\"low\":-829903741,\"unsigned\":true}");
  results &= temp.getSingleResultAs<long long int>("SELECT JSONLONG(?)", json, -1) == 1506703617155;

  temp.exec("CREATE TABLE test (col1, col2, col3)");
  temp.exec("INSERT INTO test (col1, col2, col3) VALUES (1503238553601, 2, 3)");
  temp.exec("INSERT INTO test (col2, col3) VALUES (1503238553601, '{\"high\":350,\"low\":-829903741,\"unsigned\":true}')");
  temp.exec("INSERT INTO test (col1, col2, col3) VALUES (1503238553601, '{\"high\":350,\"low\":-829903741,\"unsigned\":true}', 3)");
  temp.exec("INSERT INTO test (col1, col2, col3) VALUES ('{\"high\":350,\"low\":-829903741,\"unsigned\":true}', 2, 3)");
  temp.exec("INSERT INTO test (col3) VALUES ('{\"high\":350,\"low\":-829903741,\"unsigned\":true}')");
  temp.exec("INSERT INTO test (col2, col3) VALUES ('{\"high\":350,\"low\":-829903741,\"unsigned\":true}', 3)");
  temp.exec("INSERT INTO test (col1) VALUES ('{\"high\":350,\"low\":-829903741,\"unsigned\":true}')");

  SqliteDB::QueryResults res;
  temp.exec("SELECT JSONLONG(COALESCE(col1, col2, col3)) FROM test", &res);

  // res.prettyPrint()
  for (uint i = 0; i < res.rows(); ++i)
  {
    results &= (res.valueAsInt(i, 0) == (i < 3 ? 1503238553601 : 1506703617155));
    if (!results)
    {
      std::cout << "JSONLONG FAILED TEST:" << " Unexpected value at i = " << i << std::endl;
      return 1;
    }
  }

  //std::cout << "JSONLONG TESTS PASSED" << std::endl;
  return 0;
}
