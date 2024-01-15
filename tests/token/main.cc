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

#include "../../memsqlitedb/memsqlitedb.h"

int main()
{

  bool results = true;

  MemSqliteDB temp;
  if (!temp.exec("CREATE TABLE conversations (rowid INTEGER PRIMARY KEY, members)"))
  {
    std::cout << "FAILED TEST TOKEN." << std::endl;
    return 1;
  }

  if (!temp.exec("INSERT INTO conversations (rowid, members) VALUES (3, '24d3386d-70b1-44de-87c1-c62d0c6e0b99 93722273-78e3-4136-8640-c8261969714c 0d70b7f4-fe4a-41af-9fd5-e74268d13f6e')"))
  {
    std::cout << "FAILED TEST TOKEN." << std::endl;
    return 1;
  }

  SqliteDB::QueryResults res;
  if (!temp.exec("SELECT members, TOKENCOUNT(members), TOKEN(members, 0), TOKEN(members, 1), TOKEN(members, 2), TOKEN(members, 3) FROM conversations WHERE rowid == 3", &res))
  {
    std::cout << "FAILED TEST TOKEN." << std::endl;
    return 1;
  }
  //res.printLineMode();

  results &= (res(0, "members") == "24d3386d-70b1-44de-87c1-c62d0c6e0b99 93722273-78e3-4136-8640-c8261969714c 0d70b7f4-fe4a-41af-9fd5-e74268d13f6e");

  results &= (res.valueAsInt(0, "TOKENCOUNT(members)") == 3);
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }
  results &= (res(0, "TOKEN(members, 0)") == "24d3386d-70b1-44de-87c1-c62d0c6e0b99");
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }
  results &= (res(0, "TOKEN(members, 1)") == "93722273-78e3-4136-8640-c8261969714c");
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }
  results &= (res(0, "TOKEN(members, 2)") == "0d70b7f4-fe4a-41af-9fd5-e74268d13f6e");
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }
  results &= (res.isNull(0, "TOKEN(members, 3)"));
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }

  if (!temp.exec("SELECT members, "
            "TOKENCOUNT(members, '-'), "
            "TOKEN(members, 0, '-'), TOKEN(members, 1, '-'), TOKEN(members, 2, '-'), TOKEN(members, 3, '-'), "
            "TOKEN(members, 4, '-'), TOKEN(members, 5, '-'), TOKEN(members, 6, '-'), TOKEN(members, 7, '-'), "
            "TOKEN(members, 8, '-'), TOKEN(members, 9, '-'), TOKEN(members, 10, '-'), TOKEN(members, 11, '-'), "
            "TOKEN(members, 12, '-'), TOKEN(members, 13, '-'), TOKEN(members, 14, '-'), TOKEN(members, 15, '-') "
            " FROM conversations WHERE rowid == 3", &res))
  {
    std::cout << "FAILED TEST TOKEN." << std::endl;
    return 1;
  }
  //res.printLineMode();

  results &= (res.valueAsInt(0, "TOKENCOUNT(members, '-')") == 13);
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }
  results &= (res(0, "TOKEN(members, 0, '-')") == "24d3386d");
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }
  results &= (res(0, "TOKEN(members, 1, '-')") == "70b1");
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }
  results &= (res(0, "TOKEN(members, 2, '-')") == "44de");
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }
  results &= (res(0, "TOKEN(members, 3, '-')") == "87c1");
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }
  results &= (res(0, "TOKEN(members, 4, '-')") == "c62d0c6e0b99 93722273");
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }
  results &= (res(0, "TOKEN(members, 5, '-')") == "78e3");
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }
  results &= (res(0, "TOKEN(members, 6, '-')") == "4136");
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }
  results &= (res(0, "TOKEN(members, 7, '-')") == "8640");
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }
  results &= (res(0, "TOKEN(members, 8, '-')") == "c8261969714c 0d70b7f4");
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }
  results &= (res(0, "TOKEN(members, 9, '-')") == "fe4a");
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }
  results &= (res(0, "TOKEN(members, 10, '-')") == "41af");
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }
  results &= (res(0, "TOKEN(members, 11, '-')") == "9fd5");
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }
  results &= (res(0, "TOKEN(members, 12, '-')") == "e74268d13f6e");
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }
  results &= (res.isNull(0, "TOKEN(members, 13, '-')"));
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }
  results &= (res.isNull(0, "TOKEN(members, 14, '-')"));
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }
  results &= (res.isNull(0, "TOKEN(members, 15, '-')"));
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }

  if (!temp.exec("SELECT members, TOKENCOUNT(members, '3'), TOKEN(members, 0, '3'), TOKEN(members, 1, '3'), TOKEN(members, 2, '3'), TOKEN(members, 3, '3') FROM conversations WHERE rowid == 3", &res))
  {
    std::cout << "FAILED TEST TOKEN." << std::endl;
    return 1;
  }

  results &= (res.valueAsInt(0, "TOKENCOUNT(members, '3')") == 7);
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }
  results &= (res(0, "TOKEN(members, 0, '3')") == "24d");
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }
  results &= (res(0, "TOKEN(members, 1, '3')") == "86d-70b1-44de-87c1-c62d0c6e0b99 9");
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }
  results &= (res(0, "TOKEN(members, 2, '3')") == "72227");
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }
  results &= (res(0, "TOKEN(members, 3, '3')") == "-78e");
  if (!results)
  {
    std::cout << "TOKEN TEST FAILED: UNEXPECTED RESULT" << std::endl;
    return 1;
  }

  //std::cout << "TOKEN TESTS PASSED" << std::endl;
  return 0;
}
