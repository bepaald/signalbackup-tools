#include "signalbackup.ih"

void SignalBackup::updateThreadsEntries(long long int thread)
{
  SqliteDB::QueryResults results;
  std::string query = "SELECT DISTINCT _id FROM thread"; // gets all threads
  if (thread > -1)
    query += " WHERE _id = " + bepaald::toString(thread);
  d_database.exec(query, &results);
  for (uint i = 0; i < results.rows(); ++i)
  {
    if (results.valueHasType<long long int>(i, 0))
    {
      // set message count
      std::string threadid = bepaald::toString(results.getValueAs<long long int>(i, 0));
      std::cout << "Dealing with thread id: " << threadid << std::endl;
      std::cout << "  Updating msgcount" << std::endl;
      d_database.exec("UPDATE thread SET message_count = (SELECT (SELECT count(*) FROM sms WHERE thread_id = " + threadid +
                      ") + (SELECT count(*) FROM mms WHERE thread_id = " + threadid + ")) WHERE _id = " + threadid);

      // not sure if i need mms.date(_received)...
      SqliteDB::QueryResults results2;
      d_database.exec("SELECT sms.date_sent AS union_date, sms.type AS union_type, sms.body AS union_body, sms._id AS [sms._id], '' AS [mms._id] FROM 'sms' WHERE sms.thread_id = "
                      + threadid
                      + " UNION SELECT mms.date AS union_display_date, mms.msg_box AS union_type, mms.body AS union_body, '' AS [sms._id], mms._id AS [mms._id] FROM mms WHERE mms.thread_id = "
                      + threadid + " ORDER BY union_date DESC LIMIT 1", &results2);
      //results2.prettyPrint();

      std::any date = results2.value(0, "union_date");
      if (date.type() == typeid(long long int))
      {
        long long int roundeddate = std::any_cast<long long int>(date) - (std::any_cast<long long int>(date) % 1000);
        std::cout << "  Setting last msg date" << std::endl;
        d_database.exec("UPDATE thread SET date = ? WHERE _id = ?", {roundeddate, threadid});
      }

      std::any body = results2.value(0, "union_body");
      if (body.type() == typeid(std::string))
      {
        std::cout << "  Updating snippet" << std::endl;
        d_database.exec("UPDATE thread SET snippet = ? WHERE _id = ?", {std::any_cast<std::string>(body), threadid});
      }

      std::any type = results2.value(0, "union_type");
      if (type.type() == typeid(long long int))
      {
        std::cout << "  Updating snippet type" << std::endl;
        d_database.exec("UPDATE thread SET snippet_type = ? WHERE _id = ?", {std::any_cast<long long int>(type), threadid});
      }

      std::any mid = results2.value(0, "mms._id");
      if (mid.type() == typeid(long long int))
      {
        SqliteDB::QueryResults results3;
        d_database.exec("SELECT unique_id, _id FROM part WHERE mid = ?", {mid}, &results3);

        std::any uniqueid = results3.value(0, "unique_id");
        std::any id = results3.value(0, "_id");

        // snippet_uri = content://org.thoughtcrime.securesms/part/ + part.unique_id + '/' + part._id
        if (id.type() == typeid(long long int) && uniqueid.type() == typeid(long long int))
        {
          std::cout << "  Updating snippet_uri" << std::endl;
          d_database.exec("UPDATE thread SET snippet_uri = 'content://org.thoughtcrime.securesms/part/" +
                          bepaald::toString(std::any_cast<long long int>(uniqueid)) + "/" +
                          bepaald::toString(std::any_cast<long long int>(id)) + "' WHERE _id = " + threadid);
        }
      }

    }
  }
}
