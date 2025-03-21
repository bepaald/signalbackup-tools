/*
  Copyright (C) 2019-2025  Selwin van Dijk

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
#include "msgrange.h"

#include "../common_be.h"
#include "../common_filesystem.h"

void SignalBackup::handleSms(SqliteDB::QueryResults const &results, std::ofstream &outputfile, std::string const &self [[maybe_unused]], int i) const
{
  /* protocol - Protocol used by the message, its mostly 0 in case of SMS messages. */
  /* OPTIONAL */
  // removed from dbv166
  long long int protocol = getIntOr(results, i, "protocol", 0);

  /* subject - Subject of the message, its always null in case of SMS messages. */
  /* OPTIONAL */
  std::string subject = getStringOr(results, i, "subject");

  /* service_center - The service center for the received message, null in case of sent messages. */
  /* OPTIONAL */
  std::string service_center = getStringOr(results, i, "service_center");

  /* read - Read Message = 1, Unread Message = 0. */
  /* REQUIRED */
  long long int read = getIntOr(results, i, "read", 0);

  /* status - None = -1, Complete = 0, Pending = 32, Failed = 64. */
  /* REQUIRED */
  long long int status = getIntOr(results, i, "status", 0);

  /* type - 1 = Received, 2 = Sent, 3 = Draft, 4 = Outbox, 5 = Failed, 6 = Queued */
  /* REQUIRED */
  long long int type = 5;
  long long int realtype = -1;
  if (results.valueHasType<long long int>(i, "type"))
  {
    realtype = results.getValueAs<long long int>(i, "type");

    // skip status messages for now...
    if (Types::isStatusMessage(realtype))
    {
      //std::cout << "Skipping status message " << realtype << std::endl;
      return;
    }

    // skip 'This group was updated to a New Group'
    if (realtype == Types::GV1_MIGRATION_TYPE)
      return;

    switch (realtype & Types::BASE_TYPE_MASK)
    {
      case Types::INCOMING_CALL_TYPE:
      case Types::BASE_INBOX_TYPE:
        type = 1;
        break;
      case Types::OUTGOING_CALL_TYPE:
      case Types::BASE_SENT_TYPE:
        type = 2;
        break;
      case Types::MISSED_CALL_TYPE:
      case Types::BASE_DRAFT_TYPE:
        type = 3;
        break;
      case Types::JOINED_TYPE:
      case Types::BASE_OUTBOX_TYPE:
        type = 4;
        break;
      case Types::UNSUPPORTED_MESSAGE_TYPE:
      case Types::BASE_SENT_FAILED_TYPE:
        type = 5;
        break;
      case Types::INVALID_MESSAGE_TYPE:
      case Types::BASE_SENDING_TYPE:
      case Types::BASE_PENDING_SECURE_SMS_FALLBACK:
      case Types::BASE_PENDING_INSECURE_SMS_FALLBACK:
        type = 6;
        break;
    }
  }

  /* date - The Java date representation (including millisecond) of the time when the message was sent/received. */
  /* REQUIRED */
  long long int date = 0;
  if (type > 1) // we assume outgoing
  {
    if (results.valueHasType<long long int>(i, "date_sent"))
      date = results.getValueAs<long long int>(i, "date_sent");
  }
  else // incoming message
  {
    if (results.valueHasType<long long int>(i, d_sms_date_received))
      date = results.getValueAs<long long int>(i, d_sms_date_received);
  }

  /* readable_date - Optional field that has the date in a human readable format. */
  /* OPTIONAL */
  std::string readable_date;
  if (results.valueHasType<long long int>(i, d_sms_date_received))
  {
    long long int datum = results.getValueAs<long long int>(i, d_sms_date_received);
    std::time_t epoch = datum / 1000;
    readable_date = bepaald::toDateString(epoch, "%b %d, %Y %H:%M:%S");
  }

  /* address - The phone number of the sender/recipient. */
  /* REQUIRED */
  std::string address;
  if (results.valueHasType<std::string>(i, d_sms_recipient_id) || results.valueHasType<long long int>(i, d_sms_recipient_id))
  {
    std::string rid = results.valueAsString(i, d_sms_recipient_id);

    if (d_databaseversion >= 24)
    {
      SqliteDB::QueryResults r2;
      d_database.exec("SELECT " + d_recipient_e164 + " FROM recipient WHERE _id = " + rid, &r2);
      if (r2.rows() == 1 && r2.valueHasType<std::string>(0, d_recipient_e164))
        address = r2.getValueAs<std::string>(0, d_recipient_e164);
      else
        Logger::error("Failed to retrieve required field 'address' (sms database, type = ", realtype, ")");
    }
    else
      address = rid;

    escapeXmlString(&address);
  }
  else
    Logger::error("Type mismatch while retrieving required field 'address'");

  /* contact_name - Optional field that has the name of the contact. */
  /* OPTIONAL */
  std::string contact_name;
  if (results.valueHasType<std::string>(i, d_sms_recipient_id) || results.valueHasType<long long int>(i, d_sms_recipient_id))
  {
    std::string rid = results.valueAsString(i, d_sms_recipient_id);

    SqliteDB::QueryResults r2;
    if (d_database.containsTable("recipient")) // d_databaseversion >= 24)
    {
      if (!d_database.exec("SELECT COALESCE(" + (d_database.tableContainsColumn("recipient", "nickname_joined_name") ? "NULLIF(recipient.nickname_joined_name, ''),"s : ""s) +
                           "NULLIF(recipient." + d_recipient_system_joined_name + ", ''), " +
                           (d_database.tableContainsColumn("recipient", "profile_joined_name") ? "NULLIF(recipient.profile_joined_name, ''),"s : ""s) +
                           "NULLIF(recipient." + d_recipient_profile_given_name + ", ''), NULLIF(groups.title, ''), " +
                           (d_database.containsTable("distribution_list") ? "NULLIF(distribution_list.name, ''), " : "") +
                           "NULLIF(recipient." + d_recipient_e164 + ", ''), NULLIF(recipient." + d_recipient_aci + ", ''), "
                           " recipient._id) AS 'contact_name' FROM recipient "
                           "LEFT JOIN groups ON groups.recipient_id = recipient._id " +
                           (d_database.containsTable("distribution_list") ? "LEFT JOIN distribution_list ON recipient._id = distribution_list.recipient_id " : "") +
                           "WHERE recipient._id = ?", rid, &r2))
        Logger::error("Failed to get contact_name");
    }
    else
      d_database.exec("SELECT COALESCE(recipient_preferences.system_display_name, recipient_preferences.signal_profile_name) AS 'contact_name' FROM recipient_preferences WHERE recipient_ids = ?", rid, &r2);
    if (r2.rows() == 1 && r2.valueHasType<std::string>(0, "contact_name"))
      contact_name = r2.getValueAs<std::string>(0, "contact_name");
    escapeXmlString(&contact_name);
  }

  /* body - The content of the message. */
  /* REQUIRED */
  long long int expiration = getIntOr(results, i, "expires_in", -1);
  std::string body;
  if (results.valueHasType<std::string>(i, "body"))
  {
    body = results.getValueAs<std::string>(i, "body");
    body = decodeStatusMessage(body, expiration, realtype, contact_name);
    escapeXmlString(&body);
  }

  outputfile << "  <sms "
             << "protocol=\"" << protocol << "\" "
             << "address=\"" << address << "\" "
             << "contact_name=\"" << (contact_name.empty() ? std::string("null") : contact_name) << "\" "
             << "date=\"" << date << "\" "
             << "readable_date=\"" << (readable_date.empty() ? std::string("null") : readable_date) << "\" "
             << "type=\"" << type << "\" "
             << "subject=\"" << (subject.empty() ? std::string("null") : subject) << "\" "
             << "body=\"" << body << "\" "
             << "toa=\"" << "null" << "\" "
             << "sc_toa=\"" << "null" << "\" "
             << "service_center=\"" << (service_center.empty() ? std::string("null") : service_center)<< "\" "
             << "read=\"" << read << "\" "
             << "status=\"" << status << "\" "
             << "locked=\"" << 0 << "\" "
             << "/>" << '\n';
}

void SignalBackup::handleMms(SqliteDB::QueryResults const &results, std::ofstream &outputfile, std::string const &self, int i, bool keepattachmentdatainmemory) const
{
  // msg_box - The type of message, 1 = Received, 2 = Sent, 3 = Draft, 4 = Outbox
  long long int msg_box = 5;
  long long int realtype = -1;
  if (results.valueHasType<long long int>(i, d_mms_type))
  {
    realtype = results.getValueAs<long long int>(i, d_mms_type);

    // skip status messages for now...
    if (Types::isStatusMessage(realtype))
    {
      //std::cout << "Skipping status message " << realtype << std::endl;
      return;
    }

    switch (realtype & Types::BASE_TYPE_MASK)
    {
      case 1:
      case 20:
        msg_box = 1;
        break;
      case 2:
      case 23:
        msg_box = 2;
        break;
      case 3:
      case 27:
        msg_box = 3;
        break;
      case 4:
      case 21:
        msg_box = 4;
        break;
      case 5:
      case 24:
        msg_box = 5; // INVALID?
        break;
      case 6:
      case 22:
      case 25:
      case 26:
        msg_box = 7; // INVALID?
        break;
    }
  }

  // date - The Java date representation (including millisecond) of the time when the message was sent/received. Check out www.epochconverter.com for information on how to do the conversion from other languages to Java.
  long long int date = getIntOr(results, i, "date_received", 0);

  long long int date_sent = getIntOr(results, i, d_mms_date_sent, 0) / 1000;

  // readable_date - Optional field that has the date in a human readable format.
  std::string readable_date;
  if (results.valueHasType<long long int>(i, "date_received"))
  {
    long long int datum = results.getValueAs<long long int>(i, "date_received");
    std::time_t epoch = datum / 1000;
    readable_date = bepaald::toDateString(epoch, "%b %d, %Y %H:%M:%S");
  }

  // this needs to be redone:
  // get thread.thread_recipient_id from thread where _id = mms.thread_id
  // -> then get address/group_id from recipient table

  /* address - The phone number of the sender/recipient. */
  /* for (outgoing) group messages, address is all phone numbers concatenated with ~'s in between */
  bool isgroup = false;
  std::set<std::string> memberphones;
  std::string thread_address;
  std::string address;

  {
    SqliteDB::QueryResults r2;
    if (d_database.exec("SELECT " + d_thread_recipient_id + " FROM thread WHERE _id = ?", results.value(i, "thread_id"), &r2) &&
        r2.rows() == 1)
    {
      //r2.prettyPrint();
      thread_address = r2.valueAsString(0, d_thread_recipient_id);

      SqliteDB::QueryResults r3;
      d_database.exec("SELECT " + d_recipient_e164 + ",group_id FROM recipient WHERE _id = " + thread_address, &r3);
      //r3.prettyPrint();

      if (r3.rows() == 1 && r3.valueHasType<std::string>(0, "group_id"))
      {
        isgroup = true;
        std::vector<long long int> members;
        if (!getGroupMembersOld(&members, r3.getValueAs<std::string>(0, "group_id")))
        {
          Logger::error("Failed to get group members");
          return;
        }
        for (auto const &id : members)
        {
          if (!d_database.exec("SELECT " + d_recipient_e164 + " FROM recipient WHERE _id = ?", id, &r3) ||
              r3.rows() != 1)
          {
            Logger::error("Failed to get phone number for recipient: ", id);
            r3.prettyPrint(d_truncate);
            return;
          }
          memberphones.insert(r3.valueAsString(0, d_recipient_e164));
        }
#if __cplusplus > 201703L
        for (int count = memberphones.size(); auto const &p : memberphones)
#else
        int count = memberphones.size();
        for (auto const &p : memberphones)
#endif
        {
          --count;
          address += p + (count ? "~" : "");
        }
        //std::cout << "Got address: " << address << std::endl;
      }
      else if (r3.rows() == 1 && r3.valueHasType<std::string>(0, d_recipient_e164))
      {
        address = r3.getValueAs<std::string>(0, d_recipient_e164);
      }
      else
      {
        Logger::error("Failed to retrieve required field 'address' (mms database, type = ", realtype, ")");
        return;
      }
    }
    else
    {
      Logger::error("Failed to set field 'address'");
      return;
    }
  }
  escapeXmlString(&address);

  // contact_name - Optional field that has the name of the contact.
  std::string contact_name;
  if (!isgroup)
  {
    std::string rid;
    if (!d_database.tableContainsColumn(d_mms_table, "to_recipient_id") || !Types::isOutgoing(realtype)) // this is the way for older dbs, or when message is not outgoing
    {
      if (results.valueHasType<std::string>(i, d_mms_recipient_id) || results.valueHasType<long long int>(i, d_mms_recipient_id))
        rid = results.valueAsString(i, d_mms_recipient_id);
    }
    else if (d_database.tableContainsColumn(d_mms_table, "to_recipient_id") && Types::isOutgoing(realtype)) // on newer dbs, when message is outgoing, check the to_recipient
      if (results.valueHasType<std::string>(i, "to_recipient_id") || results.valueHasType<long long int>(i, "to_recipient_id"))
        rid = results.valueAsString(i, "to_recipient_id");


    if (!rid.empty())
    {
      /*
      SqliteDB::QueryResults r2;
      if (d_database.containsTable("recipient")) // d_databaseversion >= 24)
        d_database.exec("SELECT COALESCE(" + (d_database.tableContainsColumn("recipient", "nickname_joined_name") ? "NULLIF(recipient.nickname_joined_name, ''),"s : ""s) +
                        "NULLIF(recipient." + d_recipient_system_joined_name + ", ''), " +
                        (d_database.tableContainsColumn("recipient", "profile_joined_name") ? "NULLIF(recipient.profile_joined_name, ''),"s : ""s) +
                        "NULLIF(recipient." + d_recipient_profile_given_name + ", ''), NULLIF(groups.title, ''), " +
                        (d_database.containsTable("distribution_list") ? "NULLIF(distribution_list.name, ''), " : "") +
                        "NULLIF(recipient." + d_recipient_e164 + ", ''), NULLIF(recipient." + d_recipient_aci + ", ''), "
                        " recipient._id) AS 'contact_name' FROM recipient "
                        "LEFT JOIN groups ON groups.recipient_id = recipient._id " +
                        (d_database.containsTable("distribution_list") ? "LEFT JOIN distribution_list ON recipient._id = distribution_list.recipient_id " : "") +
                        "WHERE recipient._id = ?", rid, &r2);
      else
        d_database.exec("SELECT COALESCE(recipient_preferences.system_display_name, recipient_preferences.signal_profile_name) AS 'contact_name' FROM recipient_preferences WHERE recipient_ids = ?", rid, &r2);
      if (r2.rows() == 1 && r2.valueHasType<std::string>(0, "contact_name"))
        contact_name = r2.getValueAs<std::string>(0, "contact_name");
      */
      contact_name = getNameFromRecipientId(bepaald::toNumber<long long int>(rid));
    }
  }
  else
  {
    /*
    SqliteDB::QueryResults r2;
    if (d_database.exec("SELECT title FROM groups WHERE group_id IS (SELECT group_id FROM recipient WHERE _id = ?)", thread_address, &r2) &&
        r2.rows() == 1)
      contact_name = r2.valueAsString(0, "title");
    */
    contact_name = getNameFromRecipientId(bepaald::toNumber<long long int>(thread_address));
  }
  escapeXmlString(&contact_name);

  // sub - The subject of the message, if present.
  std::string sub = getStringOr(results, i, "sub");

  // read - Has the message been read
  long long int read = getIntOr(results, i, "read", 0);

  // read_status - The read-status of the message.
  std::string read_status = getStringOr(results, i, "read_status");

  // rr - The read-report of the message.
  long long int rr = getIntOr(results, i, "rr", 0);

  // ct_t - The Content-Type of the message, usually "application/vnd.wap.multipart.related"
  std::string ct_t = getStringOr(results, i, "ct_t", "application/vnd.wap.multipart.related");

  std::string ct_cls = getStringOr(results, i, "ct_cls");
  std::string sub_cs = getStringOr(results, i, "sub_cs");
  long long int pri = getIntOr(results, i, "pri", 0);
  long long int v = getIntOr(results, i, "v", 0);   // v = (msg_box == 1 // m_type == 132) ? 16 : (== 2 // == 128) 18  ????

  // m_id - The Message-ID of the message
  std::string m_id = getStringOr(results, i, "m_id");

  // m_size - The size of the message.
  std::string m_size = getStringOr(results, i, "m_size");

  // m_type - The type of the message defined by MMS spec.
  long long int m_type = getIntOr(results, i, "m_type", 0);
  if (m_type == 0) // let's set this to 128/132 for outgoing/incoming to appease Google Messenger (#216)
    m_type = Types::isOutgoing(realtype) ? 128 : 132;

  // m_cls
  std::string m_cls = getStringOr(results, i, "m_cls");  // if address == __textsecuregroup_ -> "personal" ???

  std::string retr_st = getStringOr(results, i, "retr_st");
  std::string retr_txt = getStringOr(results, i, "retr_txt");
  std::string retr_txt_cs = getStringOr(results, i, "retr_txt_cs");
  std::string ct_l = getStringOr(results, i, "ct_l");
  std::string d_tm = getStringOr(results, i, "d_tm");
  std::string d_rpt = getStringOr(results, i, "d_rpt");
  std::string exp = getStringOr(results, i, "exp");
  std::string resp_txt = getStringOr(results, i, "resp_txt");
  std::string rpt_a = getStringOr(results, i, "rpt_a");
  std::string resp_st = getStringOr(results, i, "resp_st");
  std::string st = getStringOr(results, i, "st");
  std::string tr_id = getStringOr(results, i, "tr_id");

  /*
    REQUIRED
    ints:
    seen|locked
  */

  long long int seen = 0;
  long long int locked = 0;

  long long int text_only = 0;

  // attachment data
  long long int mid = getIntOr(results, i, "_id", -1);
  SqliteDB::QueryResults part_results;
  if (mid >= 0)
    d_database.exec("SELECT _id," +
                    (d_database.tableContainsColumn(d_part_table, "unique_id") ? "unique_id"s : "-1 AS unique_id"s) + ", " +
                    (d_database.tableContainsColumn(d_part_table, "seq") ? "seq"s : "0 AS seq"s) + ", " +   // seq was removed in dbv215
                    d_part_ct + ", file_name, " +
                    d_part_cd + ", " +
                    //"chset, " +   // chset was removed in dbv215 (always null in earlier dbs)
                    //"fn, " +      // idem
                    //"cid, " +     // idem
                    //"ctt_s, " +   // idem
                    //"ctt_t "      // idem
                    d_part_cl +
                    " FROM " + d_part_table + " WHERE " + d_part_table + "." + d_part_mid + " = ?", mid, &part_results);
  if (part_results.rows() == 0)
    text_only = 1;

  outputfile << "  <mms "
             << "msg_box=\"" << msg_box << "\" "
             << "date=\"" << date << "\" "
             << "date_sent=\"" << date_sent << "\" "
             << "seen=\"" << seen << "\" "
             << "readable_date=\"" << readable_date << "\" "
             << "address=\"" << address << "\" "
             << "contact_name=\"" << contact_name << "\" "
             << "sub=\"" << sub << "\" "
             << "read=\"" << read << "\" "
             << "read_status=\"" << read_status << "\" "
             << "rr=\"" << rr << "\" "
             << "ct_t=\"" << ct_t << "\" "
             << "ct_l=\"" << ct_l << "\" "
             << "ct_cls=\"" << ct_cls << "\" "
             << "sub_cs=\"" << sub_cs << "\" "
             << "pri=\"" << pri << "\" "
             << "v=\"" << v << "\" "
             << "exp=\"" << exp << "\" "
             << "d_tm=\"" << d_tm << "\" "
             << "d_rpt=\"" << d_rpt << "\" "
             << "rpt_a=\"" << rpt_a << "\" "
             << "st=\"" << st << "\" "
             << "locked=\"" << locked << "\" "
             << "m_id=\"" << m_id << "\" "
             << "m_size=\"" << m_size << "\" "
             << "m_type=\"" << m_type << "\" "
             << "m_cls=\"" << m_cls << "\" "
             << "tr_id=\"" << tr_id << "\" "
             << "retr_st=\"" << retr_st << "\" "
             << "retr_txt=\"" << retr_txt << "\" "
             << "retr_txt_cs=\"" << retr_txt_cs << "\" "
             << "resp_txt=\"" << resp_txt << "\" "
             << "resp_st=\"" << resp_st << "\" "
             << "text_only=\"" << text_only << "\">"
             << '\n';
  // << "=\"" <<  << "\" "

  /* PART */
  /* text - The content of the message. */
  long long int expiration = getIntOr(results, i, "expires_in", -1);
  std::string text;
  if (results.valueHasType<std::string>(i, "body"))
  {
    text = results.getValueAs<std::string>(i, "body");
    SqliteDB::QueryResults mention_results;
    if (d_database.containsTable("mention"))
      d_database.exec("SELECT recipient_id, range_start, range_length FROM mention WHERE message_id = ?", mid, &mention_results);

    std::vector<Range> ranges;
    for (unsigned int m = 0; m < mention_results.rows(); ++m)
    {
      std::string displayname = getNameFromRecipientId(mention_results.getValueAs<long long int>(m, "recipient_id"));
      if (displayname.empty())
        continue;
      ranges.emplace_back(Range{mention_results.getValueAs<long long int>(m, "range_start"),
                                mention_results.getValueAs<long long int>(m, "range_length"),
                                "",
                                "@" + displayname,
                                "",
                                false});
    }
    applyRanges(&text, &ranges, nullptr);

    if (Types::isStatusMessage(realtype))
    {
      if (!text.empty())
        text = decodeStatusMessage(text, expiration, realtype, contact_name);
      else if (d_database.tableContainsColumn(d_mms_table, "message_extras") &&
               results.valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "message_extras"))
        text = decodeStatusMessage(results.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "message_extras"),
                                   expiration, realtype, contact_name);
    }
    escapeXmlString(&text);
  }
  else if (results.isNull(i, "body"))
  {
    if (Types::isStatusMessage(realtype) &&
        d_database.tableContainsColumn(d_mms_table, "message_extras") &&
        results.valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "message_extras"))
      text = decodeStatusMessage(results.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "message_extras"),
                                 expiration, realtype, contact_name);
    escapeXmlString(&text);
  }

  outputfile << "    <parts>" << '\n';

  if (!text.empty())
  {
    outputfile << "      <part "
               << "seq=\"0\" "
               << "ct=\"text/plain\" "
               << "name=\"null\" "
               << "chset=\"106\" "
               << "cd=\"null\" "
               << "fn=\"null\" "
      //       << "cl=\"&lt;text" << std::setw(6) << std::setfill('0') << mid << std::setw(0) << "&gt;\" "
               << "cl=\"text" << std::setw(6) << std::setfill('0') << mid << std::setw(0) << ".txt\" "
               << "ctt_s=\"null\" "
               << "ctt_t=\"null\" "
               << "text=\"" << text << "\" " // maybe this string needs to be escaped?
               << "></part>" << '\n';
  }

  // attachments...
  for (unsigned int j = 0; j < part_results.rows(); ++j)
  {
    long long int seq = getIntOr(part_results, j, "seq", 0);
    std::string ct = getStringOr(part_results, j, d_part_ct, "null");
    std::string name = getStringOr(part_results, j, "name", "null");
    std::string chset = getStringOr(part_results, j, "chset", "null");
    std::string cd = getStringOr(part_results, j, d_part_cd, "null");
    std::string fn = getStringOr(part_results, j, "fn", "null");
    std::string cid = getStringOr(part_results, j, "cid", "null");
    std::string cl = getStringOr(part_results, j, d_part_cl, "null");
    std::string ctt_s = getStringOr(part_results, j, "ctt_s", "null");
    std::string ctt_t = getStringOr(part_results, j, "ctt_t", "null");

    //   seq - The order of the part.
    //   ct - The content type of the part.
    //   name - The name of the part.
    //   chset - The charset of the part.
    //   cl - The content location of the part.
    //   data - The base64 encoded binary content of the part.
    //                         <xs:attribute name="seq" type="xs:byte" use="required" />
    //                         <xs:attribute name="ct" type="xs:string" use="required" />
    //                         <xs:attribute name="name" type="xs:string" use="required" />
    //                         <xs:attribute name="chset" type="xs:string" use="required" />
    //                         <xs:attribute name="cd" type="xs:string" use="required" />
    //                         <xs:attribute name="fn" type="xs:string" use="required" />
    //                         <xs:attribute name="cid" type="xs:string" use="required" />
    //                         <xs:attribute name="cl" type="xs:string" use="required" />
    //                         <xs:attribute name="ctt_s" type="xs:string" use="required" />
    //                         <xs:attribute name="ctt_t" type="xs:string" use="required" />
    //                         <xs:attribute name="text" type="xs:string" use="required" />
    //                         <xs:attribute name="data" type="xs:string" use="optional" />

    long long int rowid = getIntOr(part_results, j, "_id", -1);
    long long int uniqueid = getIntOr(part_results, j, "unique_id", -1);
    auto attachment = d_attachments.find({rowid, uniqueid});
    if (attachment != d_attachments.end())
    {
      outputfile << "      <part "
                 << "seq=\"" << seq << "\" "
                 << "ct=\"" << ct << "\" "
                 << "name=\"" << name << "\" "
                 << "chset=\"" << chset << "\" "
                 << "cd=\"" << cd << "\" "
                 << "fn=\"" << fn << "\" "
                 << "cid=\"" << cid << "\" "
                 << "cl=\"" << cl << "\" "
                 << "ctt_s=\"" << ctt_s << "\" "
                 << "ctt_t=\"" << ctt_t << "\" "
                 << "text=\"" << "null" << "\"";
      // add this for testing, or your xml file will be huge
      outputfile << " data=\"" << Base64::bytesToBase64String(attachment->second->attachmentData(), attachment->second->attachmentSize())/*.substr(0, 50)*/ << "\" ";
      if (!keepattachmentdatainmemory)
        attachment->second.get()->clearData();
    }
    outputfile << "></part>" << '\n';
  }
  outputfile << "    </parts>" << '\n';

  // ADDR

  outputfile << "    <addrs>" << '\n';
  // 1-on-1 chat -> get conversation partners phone: <addr address="[PHONE]" type="151(outgoing)/137(incoming)" charset="106" />
  if (!isgroup)
    outputfile << "      <addr address=\"" << address << "\" type=\"" << ((msg_box == 1) ? "137" : "151") << "\" charset=\"106\" />" << '\n';
  // group chat -> for each phone number: above. Mark sender with 137, all others 151
  else
  {
    for (auto const &mp : memberphones)
    {

      // get message originator
      // incoming message: mms.address
      // outgoing message: self
      std::string sender = self;
      if (msg_box == 1) // incoming message
      {
        SqliteDB::QueryResults r2;
        if (d_database.exec("SELECT " + d_recipient_e164 + " FROM recipient WHERE _id = ?", results.valueAsString(i, d_mms_recipient_id), &r2) && // should be ok to use d_mms_recipient_id, since msg_box = incoming
            r2.rows() == 1)
          sender = r2.valueAsString(0, d_recipient_e164);
      }

      outputfile << "      <addr address=\"" << mp << "\" type=\""
                 << ((mp == sender) ? "137" : "151") << "\" charset=\"106\" />" << '\n';
    }
  }
  outputfile << "    </addrs>" << '\n';

  outputfile << "  </mms>" << '\n';

}

bool SignalBackup::exportXml(std::string const &filename, bool overwrite, std::string self, bool includemms, bool keepattachmentdatainmemory)
{
  if (d_databaseversion < 24)
  {
    Logger::error("Unsupported database version (", d_databaseversion, "). Please upgrade first.");
    return false;
  }

  // get own E164
  if (self.empty())
  {
    d_selfid = scanSelf();
    if (d_selfid != -1)
    {
      SqliteDB::QueryResults r;
      if (d_database.exec("SELECT " + d_recipient_e164 + " FROM recipient WHERE _id = ?", d_selfid, &r) && r.rows() == 1)
        self = r.valueAsString(0, d_recipient_e164);
    }

    if (self.empty())
    {
      Logger::error("Failed to determine own phone number. Please add it on the command line with `--setselfid`.");
      return false;
    }
  }
  if (d_selfid != -1)
    d_selfuuid = bepaald::toLower(d_database.getSingleResultAs<std::string>("SELECT " + d_recipient_aci + " FROM recipient WHERE _id = ?", d_selfid, std::string()));

  Logger::message("\nExporting backup to '", filename, "'");

  if (!overwrite && (bepaald::fileOrDirExists(filename) && !bepaald::isDir(filename)))
  {
    Logger::error("File ", filename, " exists, use --overwrite to overwrite");
    return false;
  }

  // output header
  std::ofstream outputfile(filename, std::ios_base::binary);
  outputfile << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>" << '\n';
  outputfile << "<?xml-stylesheet type=\"text/xsl\" href=\"sms.xsl\" ?>" << '\n';

  SqliteDB::QueryResults sms_results;
  if (d_database.containsTable("sms"))
  {
    if (d_database.tableContainsColumn("sms", "protocol") &&
        d_database.tableContainsColumn("sms", "service_center") &&
        d_database.tableContainsColumn("sms", "subject")) // removed in dbv166
      d_database.exec("SELECT _id,thread_id,protocol,subject,service_center,read,status,date_sent," + d_sms_date_received + "," + d_sms_recipient_id + ",type,body,expires_in FROM sms "
                      "WHERE "
                      + d_sms_recipient_id + " IN (SELECT _id FROM recipient WHERE " + d_recipient_e164 + " IS NOT NULL OR group_id IS NOT NULL) "
                      "AND "
                      "(type & ?) == 0 AND ((type & 0x1F) == ? OR (type & 0x1F) == ? OR (type & 0x1F) == ? OR (type & 0x1F) == ? OR (type & 0x1F) == ? OR (type & 0x1F) == ? OR (type & 0x1F) == ? OR (type & 0x1F) == ?)",
                      {Types::GROUP_UPDATE_BIT, Types::BASE_INBOX_TYPE, Types::BASE_OUTBOX_TYPE, Types::BASE_SENDING_TYPE, Types::BASE_SENT_TYPE, Types::BASE_SENT_FAILED_TYPE,
                       Types::BASE_PENDING_SECURE_SMS_FALLBACK, Types::BASE_PENDING_INSECURE_SMS_FALLBACK,  Types::BASE_DRAFT_TYPE}, &sms_results);
    else
      d_database.exec("SELECT _id,thread_id,read,status,date_sent," + d_sms_date_received + "," + d_sms_recipient_id + ",type,body,expires_in FROM sms "
                      "WHERE "
                      + d_sms_recipient_id + " IN (SELECT _id FROM recipient WHERE " + d_recipient_e164 + " IS NOT NULL OR group_id IS NOT NULL) "
                      "AND "
                      "(type & ?) == 0 AND ((type & 0x1F) == ? OR (type & 0x1F) == ? OR (type & 0x1F) == ? OR (type & 0x1F) == ? OR (type & 0x1F) == ? OR (type & 0x1F) == ? OR (type & 0x1F) == ? OR (type & 0x1F) == ?)",
                      {Types::GROUP_UPDATE_BIT, Types::BASE_INBOX_TYPE, Types::BASE_OUTBOX_TYPE, Types::BASE_SENDING_TYPE, Types::BASE_SENT_TYPE, Types::BASE_SENT_FAILED_TYPE,
                       Types::BASE_PENDING_SECURE_SMS_FALLBACK, Types::BASE_PENDING_INSECURE_SMS_FALLBACK,  Types::BASE_DRAFT_TYPE}, &sms_results);
  }

  SqliteDB::QueryResults mms_results;
  if (includemms)
  {
    // at dbv 109 many columns were removed from the mms table.
    if (d_databaseversion >= 109)
      d_database.exec("SELECT _id,thread_id,date_received," + d_mms_date_sent + "," + d_mms_recipient_id + (d_database.tableContainsColumn(d_mms_table, "to_recipient_id") ? ",to_recipient_id" : "") +
                      "," + d_mms_type + ","
                      "(" + d_mms_type + " & " + bepaald::toString(Types::BASE_TYPE_MASK) + ") AS base_type,body,expires_in," +
                      (d_database.tableContainsColumn(d_mms_table, "message_extras") ? "message_extras, " : "") +
                      "read, ct_l, m_type, m_size, exp, tr_id, st FROM " + d_mms_table +
                      " WHERE "
                      + d_mms_recipient_id + " IN (SELECT _id FROM recipient WHERE " + d_recipient_e164 + " IS NOT NULL OR group_id IS NOT NULL) "
                      "AND " +
                      (d_database.tableContainsColumn(d_mms_table, "to_recipient_id") ? "to_recipient_id IN (SELECT _id FROM recipient WHERE " + d_recipient_e164 + " IS NOT NULL OR group_id IS NOT NULL) AND " : "") +
                      "(" + d_mms_type + " & ?) == 0 AND (" + d_mms_type + " & ?) == 0 AND (" + d_mms_type + " & ?) == 0  AND (" + d_mms_type + " & ?) == 0 AND (" + d_mms_type + " & ?) == 0 AND "
                      "(base_type IN (?, ?, ?, ?, ?, ?, ?, ?)) AND "
                      "((" + d_mms_type + " & ?) NOT IN (?, ?, ?)) AND "
                      "((" + d_mms_type + " & ?) NOT IN (?)) AND "
                      //"((" + d_mms_type + " & ?) != 0) AND "
                      "(base_type NOT IN (?, ?)) AND "
                      "(" + d_mms_type + " NOT IN (?, ?, ?, ?, ?, ?, ?, ?, ?, ?))",
                      {Types::GROUP_UPDATE_BIT, Types::GROUP_V2_BIT, Types::GROUP_QUIT_BIT, Types::END_SESSION_BIT, Types::EXPIRATION_TIMER_UPDATE_BIT,
                       Types::BASE_INBOX_TYPE, Types::BASE_OUTBOX_TYPE, Types::BASE_SENDING_TYPE, Types::BASE_SENT_TYPE, Types::BASE_SENT_FAILED_TYPE, Types::BASE_PENDING_SECURE_SMS_FALLBACK, Types::BASE_PENDING_INSECURE_SMS_FALLBACK,  Types::BASE_DRAFT_TYPE,
                       Types::KEY_EXCHANGE_MASK, Types::KEY_EXCHANGE_IDENTITY_UPDATE_BIT, Types::KEY_EXCHANGE_IDENTITY_VERIFIED_BIT, Types::KEY_EXCHANGE_IDENTITY_DEFAULT_BIT,
                       Types::SPECIAL_TYPES_MASK, Types::SPECIAL_TYPE_MESSAGE_REQUEST_ACCEPTED,
                       Types::PROFILE_CHANGE_TYPE, Types::JOINED_TYPE,
                       Types::GROUP_CALL_TYPE, Types::INCOMING_CALL_TYPE, Types::OUTGOING_CALL_TYPE, Types::MISSED_CALL_TYPE, Types::INCOMING_VIDEO_CALL_TYPE, Types::OUTGOING_VIDEO_CALL_TYPE, Types::MISSED_VIDEO_CALL_TYPE, Types::GV1_MIGRATION_TYPE, Types::CHANGE_NUMBER_TYPE, Types::BOOST_REQUEST_TYPE},
                      &mms_results);
    else
      d_database.exec("SELECT _id,thread_id,date_received," + d_mms_date_sent + "," + d_mms_recipient_id + "," + d_mms_type + ","
                      "(" + d_mms_type + " & " + bepaald::toString(Types::BASE_TYPE_MASK) + ") AS base_type,body,expires_in,read,m_id,sub,ct_t,ct_l,m_type,m_size,rr,read_status,"
                      "m_cls, sub_cs, ct_cls, v, pri, retr_st, retr_txt, retr_txt_cs, d_tm, d_rpt, exp, resp_txt, tr_id, st, resp_st, rpt_a FROM " + d_mms_table + " WHERE "
                      + d_mms_recipient_id + " IN (SELECT _id FROM recipient WHERE " + d_recipient_e164 + " IS NOT NULL OR group_id IS NOT NULL) "
                      "AND "
                      "(" + d_mms_type + " & ?) == 0 AND (" + d_mms_type + " & ?) == 0  AND (" + d_mms_type + " & ?) == 0 AND (" + d_mms_type + " & ?) == 0 AND (" + d_mms_type + " & ?) == 0 AND "
                      "(base_type IN (?, ?, ?, ?, ?, ?, ?, ?)) AND "
                      "((" + d_mms_type + " & ?) NOT IN (?, ?, ?)) AND "
                      "((" + d_mms_type + " & ?) NOT IN (?)) AND "
                      //"((" + d_mms_type + " & ?) != 0) AND "
                      "(base_type NOT IN (?, ?)) AND "
                      "(" + d_mms_type + " NOT IN (?, ?, ?, ?, ?, ?, ?, ?, ?, ?))",
                      {Types::GROUP_UPDATE_BIT, Types::GROUP_V2_BIT, Types::GROUP_QUIT_BIT, Types::END_SESSION_BIT, Types::EXPIRATION_TIMER_UPDATE_BIT,
                       Types::BASE_INBOX_TYPE, Types::BASE_OUTBOX_TYPE, Types::BASE_SENDING_TYPE, Types::BASE_SENT_TYPE, Types::BASE_SENT_FAILED_TYPE, Types::BASE_PENDING_SECURE_SMS_FALLBACK, Types::BASE_PENDING_INSECURE_SMS_FALLBACK,  Types::BASE_DRAFT_TYPE,
                       Types::KEY_EXCHANGE_MASK, Types::KEY_EXCHANGE_IDENTITY_UPDATE_BIT, Types::KEY_EXCHANGE_IDENTITY_VERIFIED_BIT, Types::KEY_EXCHANGE_IDENTITY_DEFAULT_BIT,
                       Types::SPECIAL_TYPES_MASK, Types::SPECIAL_TYPE_MESSAGE_REQUEST_ACCEPTED,
                       Types::PROFILE_CHANGE_TYPE, Types::JOINED_TYPE,
                       Types::GROUP_CALL_TYPE, Types::INCOMING_CALL_TYPE, Types::OUTGOING_CALL_TYPE, Types::MISSED_CALL_TYPE, Types::INCOMING_VIDEO_CALL_TYPE, Types::OUTGOING_VIDEO_CALL_TYPE, Types::MISSED_VIDEO_CALL_TYPE, Types::GV1_MIGRATION_TYPE, Types::CHANGE_NUMBER_TYPE, Types::BOOST_REQUEST_TYPE},
                      &mms_results);
  }

  //std::string date;
  outputfile << "<smses count=\"" << bepaald::toString(sms_results.rows() + mms_results.rows())
  //         << "\" backup_date=\"" << date << "\" type=\"full\">" << '\n';
             << "\">\n";
  unsigned int sms_row = 0;
  unsigned int mms_row = 0;
  while (sms_row < sms_results.rows() ||
         mms_row < mms_results.rows())
  {
    if (mms_row >= mms_results.rows() ||
        (sms_row < sms_results.rows() &&
         (sms_results.getValueAs<long long int>(sms_row, d_sms_date_received) <
          mms_results.getValueAs<long long int>(mms_row, "date_received"))))
      handleSms(sms_results, outputfile, self, sms_row++);
    else if (mms_row < mms_results.rows())
      handleMms(mms_results, outputfile, self, mms_row++, keepattachmentdatainmemory);

    //std::cout << "Handled row! Indices now: " << sms_row << "/" << sms_results.rows() << " " << mms_row << "/" << mms_results.rows() << std::endl;
  }

  outputfile << "</smses>" << '\n';

  return true;
}

    // protocol - Protocol used by the message, its mostly 0 in case of SMS messages.
    // address - The phone number of the sender/recipient.
    // date - The Java date representation (including millisecond) of the time when the message was sent/received. Check out www.epochconverter.com for information on how to do the conversion from other languages to Java.
    // type - 1 = Received, 2 = Sent, 3 = Draft, 4 = Outbox, 5 = Failed, 6 = Queued
    // subject - Subject of the message, its always null in case of SMS messages.
    // body - The content of the message.
    // toa - n/a, defaults to null.
    // sc_toa - n/a, defaults to null.
    // service_center - The service center for the received message, null in case of sent messages.
    // read - Read Message = 1, Unread Message = 0.
    // status - None = -1, Complete = 0, Pending = 32, Failed = 64.
    // readable_date - Optional field that has the date in a human readable format.
    // contact_name - Optional field that has the name of the contact.


    // mms
    //* date - The Java date representation (including millisecond) of the time when the message was sent/received. Check out www.epochconverter.com for information on how to do the conversion from other languages to Java.
    //* ct_t - The Content-Type of the message, usually "application/vnd.wap.multipart.related"
    //* msg_box - The type of message, 1 = Received, 2 = Sent, 3 = Draft, 4 = Outbox
    //* rr - The read-report of the message.
    //* sub - The subject of the message, if present.
    //* read_status - The read-status of the message.
    //* address - The phone number of the sender/recipient.
    //* m_id - The Message-ID of the message
    //* read - Has the message been read
    //* m_size - The size of the message.
    //* m_type - The type of the message defined by MMS spec.
    //* readable_date - Optional field that has the date in a human readable format.
    //* contact_name - Optional field that has the name of the contact.
    //  part
    //   seq - The order of the part.
    //   ct - The content type of the part.
    //   name - The name of the part.
    //   chset - The charset of the part.
    //   cl - The content location of the part.
    //   text - The text content of the part.
    //   data - The base64 encoded binary content of the part.
    //  addr
    //   address - The phone number of the sender/recipient.
    //   type - The type of address, 129 = BCC, 130 = CC, 151 = To, 137 = From
    //   charset - Character set of this entry



    // Call Logs

    // number - The phone number of the call.
    // duration - The duration of the call in seconds.
    // date - The Java date representation (including millisecond) of the time when the message was sent/received. Check out www.epochconverter.com for information on how to do the conversion from other languages to Java.
    // type - 1 = Incoming, 2 = Outgoing, 3 = Missed, 4 = Voicemail, 5 = Rejected, 6 = Refused List.
    // presentation - caller id presentation info. 1 = Allowed, 2 = Restricted, 3 = Unknown, 4 = Payphone.
    // readable_date - Optional field that has the date in a human readable format.
    // contact_name - Optional field that has the name of the contact.

    //           <xs:complexType>
    //             <xs:sequence>
    //               <xs:element name="parts">
    //                 <xs:complexType>
    //                   <xs:sequence>
    //                     <xs:element maxOccurs="unbounded" name="part">
    //                       <xs:complexType>
    //                         <xs:attribute name="seq" type="xs:byte" use="required" />
    //                         <xs:attribute name="ct" type="xs:string" use="required" />
    //                         <xs:attribute name="name" type="xs:string" use="required" />
    //                         <xs:attribute name="chset" type="xs:string" use="required" />
    //                         <xs:attribute name="cd" type="xs:string" use="required" />
    //                         <xs:attribute name="fn" type="xs:string" use="required" />
    //                         <xs:attribute name="cid" type="xs:string" use="required" />
    //                         <xs:attribute name="cl" type="xs:string" use="required" />
    //                         <xs:attribute name="ctt_s" type="xs:string" use="required" />
    //                         <xs:attribute name="ctt_t" type="xs:string" use="required" />
    //                         <xs:attribute name="text" type="xs:string" use="required" />
    //                         <xs:attribute name="data" type="xs:string" use="optional" />
    //                       </xs:complexType>
    //                     </xs:element>
    //                   </xs:sequence>
    //                 </xs:complexType>
    //               </xs:element>
    //             </xs:sequence>
    //             <xs:attribute name="text_only" type="xs:unsignedByte" use="optional" />
    //             <xs:attribute name="sub" type="xs:string" use="optional" />
    //             <xs:attribute name="retr_st" type="xs:string" use="required" />
    //             <xs:attribute name="date" type="xs:unsignedLong" use="required" />
    //             <xs:attribute name="ct_cls" type="xs:string" use="required" />
    //             <xs:attribute name="sub_cs" type="xs:string" use="required" />
    //             <xs:attribute name="read" type="xs:unsignedByte" use="required" />
    //             <xs:attribute name="ct_l" type="xs:string" use="required" />
    //             <xs:attribute name="tr_id" type="xs:string" use="required" />
    //             <xs:attribute name="st" type="xs:string" use="required" />
    //             <xs:attribute name="msg_box" type="xs:unsignedByte" use="required" />
    //             <xs:attribute name="address" type="xs:long" use="required" />
    //             <xs:attribute name="m_cls" type="xs:string" use="required" />
    //             <xs:attribute name="d_tm" type="xs:string" use="required" />
    //             <xs:attribute name="read_status" type="xs:string" use="required" />
    //             <xs:attribute name="ct_t" type="xs:string" use="required" />
    //             <xs:attribute name="retr_txt_cs" type="xs:string" use="required" />
    //             <xs:attribute name="d_rpt" type="xs:unsignedByte" use="required" />
    //             <xs:attribute name="m_id" type="xs:string" use="required" />
    //             <xs:attribute name="date_sent" type="xs:unsignedByte" use="required" />
    //             <xs:attribute name="seen" type="xs:unsignedByte" use="required" />
    //             <xs:attribute name="m_type" type="xs:unsignedByte" use="required" />
    //             <xs:attribute name="v" type="xs:unsignedByte" use="required" />
    //             <xs:attribute name="exp" type="xs:string" use="required" />
    //             <xs:attribute name="pri" type="xs:unsignedByte" use="required" />
    //             <xs:attribute name="rr" type="xs:unsignedByte" use="required" />
    //             <xs:attribute name="resp_txt" type="xs:string" use="required" />
    //             <xs:attribute name="rpt_a" type="xs:string" use="required" />
    //             <xs:attribute name="locked" type="xs:unsignedByte" use="required" />
    //             <xs:attribute name="retr_txt" type="xs:string" use="required" />
    //             <xs:attribute name="resp_st" type="xs:string" use="required" />
    //             <xs:attribute name="m_size" type="xs:string" use="required" />
    //             <xs:attribute name="readable_date" type="xs:string" use="optional" />
    //             <xs:attribute name="contact_name" type="xs:string" use="optional" />
