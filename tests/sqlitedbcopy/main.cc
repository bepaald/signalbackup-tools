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

  MemSqliteDB maindb;
  maindb.exec("CREATE TABLE messages (id, body)");
  maindb.exec("INSERT INTO messages (id, body) VALUES (1, 'MESSAGE 1'), (2, 'MESSAGE 2'), (3, 'MESSAGE 3')");
  SqliteDB::QueryResults res;
  maindb.exec("SELECT id,body FROM messages", &res);
  results &= (res.rows() == 3);
  {
    MemSqliteDB copy1(maindb);
    copy1.exec("DELETE FROM messages WHERE body = 'MESSAGE 2'");
    copy1.exec("SELECT id,body FROM messages", &res);
    results &= (res.rows() == 2);

    copy1 = maindb;
    copy1.exec("SELECT id,body FROM messages", &res);
    results &= (res.rows() == 3);
    copy1.exec("DELETE FROM messages");
    copy1.exec("SELECT id,body FROM messages", &res);
    results &= (res.rows() == 0);
  }
  maindb.exec("SELECT id,body FROM messages", &res);
  results &= (res.rows() == 3);

  std::cout << "MEMSQLITEDB COPY TESTS PASSED" << std::endl;
  return 0;
}
