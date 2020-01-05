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


std::string SignalBackup::decodeStatusMessage(std::string const &body, long long int expiration, long long int type, std::string const &contactname) const
{
  if (Types::isGroupUpdate(type))
  {
    if (Types::isOutgoing(type))
      return "You updated the group.";

    std::string result = contactname + " updated the group.";

    // decode msg
    ProtoBufParser<protobuffer::optional::BYTES,
                   protobuffer::optional::ENUM,
                   protobuffer::optional::STRING,
                   protobuffer::repeated::STRING,
                   ProtoBufParser<protobuffer::optional::FIXED64,
                                  protobuffer::optional::STRING,
                                  protobuffer::optional::BYTES,
                                  protobuffer::optional::UINT32,
                                  protobuffer::optional::BYTES,
                                  protobuffer::optional::BYTES,
                                  protobuffer::optional::STRING,
                                  protobuffer::optional::UINT32,
                                  protobuffer::optional::UINT32,
                                  protobuffer::optional::UINT32>> statusmsg(body);

    std::string members;
    auto field4 = statusmsg.getField<4>();
    if (field4.size())
      for (uint k = 0; k < field4.size(); ++k)
      {
        // get name from members string
        SqliteDB::QueryResults res;
        if (d_databaseversion >= 33)
          d_database.exec("SELECT COALESCE(recipient.system_display_name, recipient.signal_profile_name) AS 'name' FROM recipient WHERE _id = " + field4[k], &res);
        else
        {
          ;//d_database.exec("SELECT COALESCE(recipient.signal_profile_name, recipient.system_display_name) AS 'name' FROM recipient_preferences WHERE _something
        }
        members += field4[k] + ", ";
      }

    if (!members.empty())
      result += "\n" + members + " joined the group.";


    std::string title = statusmsg.getField<3>().value_or(std::string());
    if (!title.empty())
    {
      result += (!members.empty() ? ' ' : '\n');
      result += "Group name is now '" + title + "'.";
    }

    return result;
  }
  if (Types::isGroupQuit(type))
  {
    if (Types::isOutgoing(type))
      return "You have left the group.";
    return contactname + "has left the group.";
  }
  if (Types::isIncomingCall(type))
    return contactname + "called you";
  if (Types::isOutgoingCall(type))
    return "You called";
  if (Types::isMissedCall(type))
    return "Missed call";
  if (Types::isJoined(type))
    return contactname + "is on Signal!";
  if (Types::isExpirationTimerUpdate(type))
  {
    if (expiration < 0)
    {
      if (Types::isOutgoing(type))
        return "You disabled disappearing messages.";
      return contactname + " disabled disappearing messages.";
    }
    std::string time = bepaald::toString(expiration) + " seconds";//ExpirationUtil.getExpirationDisplayValue(context, expiration);
    if (Types::isOutgoing(type))
      return "You set the disappearing message timer to " + time;
    return contactname + " set the disappearing message timer to " + time;
  }
  if (Types::isIdentityUpdate(type))
    return "Your safety number with " + contactname + " has changed.";
  if (Types::isIdentityVerified(type))
  {
    if (Types::isOutgoing(type))
      return "You marked your safety number with " + contactname + " verified";
    return "You marked your safety number with " + contactname + " verified from another device";
  }
  if (Types::isIdentityDefault(type))
  {
    if (Types::isOutgoing(type))
      return "You marked your safety number with " + contactname + " unverified";
    return "You marked your safety number with " + contactname + " verified from another device";
  }

  return body;
}

void SignalBackup::escapeXmlString(std::string *str) const
{
  size_t pos = 0;
  while (pos != str->size())
  {
    if (str->at(pos) == '&')
    {
      str->replace(pos, 1, "&amp;");
      pos += STRLEN("&amp;");
      continue;
    }

    if (str->at(pos) == '<')
    {
      str->replace(pos, 1, "&lt;");
      pos += STRLEN("&lt;");
      continue;
    }

    if (str->at(pos) == '>')
    {
      str->replace(pos, 1, "&gt;");
      pos += STRLEN("&gt;");
      continue;
    }

    if (str->at(pos) == '"')
    {
      str->replace(pos, 1, "&quot;");
      pos += STRLEN("&quot;");
      continue;
    }

    if (str->at(pos) == '\'')
    {
      str->replace(pos, 1, "&apos;");
      pos += STRLEN("&apos;");
      continue;
    }

    // [^\u0020-\uD7FF]
    /*
      under 0020 = control chars (escape, linebreak, etc...)
     */
    if ((static_cast<unsigned int>(str->at(pos)) & 0xFF) < 0x20)
    {
      std::string rep = "&#" + bepaald::toString(static_cast<unsigned int>(str->at(pos)) & 0xFF) + ";";
      str->replace(pos, 1, rep);
      pos += rep.length();
      continue;
    }

    /*
      If you know that the data is UTF-8, then you just have to check the high bit:

      0xxxxxxx = single-byte ASCII character
      1xxxxxxx = part of multi-byte character

      Or, if you need to distinguish lead/trail bytes:

      10xxxxxx = 2nd, 3rd, or 4th byte of multi-byte character
      110xxxxx = 1st byte of 2-byte character
      1110xxxx = 1st byte of 3-byte character
      11110xxx = 1st byte of 4-byte character
    */
    /*
      Over 0xd7ff.
      0x800 - 0xffff is only 3 byte utf chars, and are represented by utf16 points directly?
    */
    if ((str->at(pos) & 0b11110000) == 0b11100000) // or 0b11110000
    {
      if (pos + 2 >= str->size())
      {
        ++pos;
        continue;
      }
      uint32_t unicode = 0;
      unicode += (static_cast<uint32_t>(str->at(pos) & 0b0001111) << 12);
      unicode += (static_cast<uint32_t>(str->at(pos + 1) & 0b0111111) << 6);
      unicode += (static_cast<uint32_t>(str->at(pos + 2) & 0b0111111));

      std::string rep = "&#" + bepaald::toString(unicode) + ";";

      str->replace(pos, 3, rep);
      pos += rep.length();
      continue;

    }

    /*
      Over 0xffff is only 4 byte utf chars
    */
    if ((str->at(pos) & 0b11111000) == 0b11110000) // or 0b11110000
    {

      if (pos + 3 >= str->size())
      {
        ++pos;
        continue;
      }

      /*
        bytes of unicode char:
        UTF8:    11110XXX 10XXXXXX 10XXXXXX 10XXXXXX
        UNICODE: ........ ...XXX_XX XXXX_XXXX XX_XXXXXX {21}
        U' (UNICODE - 0x10000): yyyyyyyyyzzzzzzzzzz
        UTF16 LOW: 110110yyyyyyyyyy
        UTF16 HI : 110111zzzzzzzzzz
      */

      uint32_t unicode = 0;
      unicode += (static_cast<uint32_t>(str->at(pos) & 0b0000111) << 18);
      unicode += (static_cast<uint32_t>(str->at(pos + 1) & 0b0111111) << 12);
      unicode += (static_cast<uint32_t>(str->at(pos + 2) & 0b0111111) << 6);
      unicode += (static_cast<uint32_t>(str->at(pos + 3) & 0b0111111));
      unicode -= 0x10000;

      std::string rep = "&#" + bepaald::toString(0xd800 + (unicode >> 10)) + ";&#" + bepaald::toString(0xdc00 + (unicode & 0x3FF)) + ";";

      str->replace(pos, 4, rep);
      pos += rep.length();
      continue;
    }

    ++pos;

  }
}

void SignalBackup::exportXml(std::string const &filename) const
{

  /*
  if (checkFileExists(filename))
  {
    std::cout << "File " << filename << " exists. Refusing to overwrite" << std::endl;
    return;
  }
  */

  // output header
  std::ofstream outputfile(filename, std::ios_base::binary);
  outputfile << "<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>" << std::endl;
  outputfile << "<?xml-stylesheet type=\"text/xsl\" href=\"sms.xsl\"?>" << std::endl;

  SqliteDB::QueryResults results;
  d_database.exec("SELECT protocol,subject,service_center,read,status,date_sent,date,address,type,body,expires_in FROM sms", &results);
  if (results.rows())
  {
    outputfile << "<smses count=" << bepaald::toString(results.rows()) << ">" << std::endl;;
    for (uint i = 0; i < results.rows(); ++i)
    {

      // probably need to escape certain chars in this output (at least in body, but for safety probably everywhere)
      // like " and &

      /* protocol - Protocol used by the message, its mostly 0 in case of SMS messages. */
      /* OPTIONAL */
      long long int protocol = 0;
      if (results.valueHasType<long long int>(i, "protocol"))
        protocol = results.getValueAs<long long int>(i, "protocol");

      /* subject - Subject of the message, its always null in case of SMS messages. */
      /* OPTIONAL */
      std::string subject;
      if (results.valueHasType<std::string>(i, "subject"))
      {
        subject = results.getValueAs<std::string>(i, "subject");
        escapeXmlString(&subject);
      }

      /* service_center - The service center for the received message, null in case of sent messages. */
      /* OPTIONAL */
      std::string service_center;
      if (results.valueHasType<std::string>(i, "service_center"))
      {
        service_center = results.getValueAs<std::string>(i, "service_center");
        escapeXmlString(&service_center);
      }

      /* read - Read Message = 1, Unread Message = 0. */
      /* REQUIRED */
      long long int read = 0;
      if (results.valueHasType<long long int>(i, "read"))
        read = results.getValueAs<long long int>(i, "read");

      /* status - None = -1, Complete = 0, Pending = 32, Failed = 64. */
      /* REQUIRED */
      long long int status = 0;
      if (results.valueHasType<long long int>(i, "status"))
        status = results.getValueAs<long long int>(i, "status");

      /* type - 1 = Received, 2 = Sent, 3 = Draft, 4 = Outbox, 5 = Failed, 6 = Queued */
      /* REQUIRED */
      long long int type = 5;
      long long int realtype = -1;
      if (results.valueHasType<long long int>(i, "type"))
      {
        realtype = results.getValueAs<long long int>(i, "type");

        switch (realtype & Types::BASE_TYPE_MASK)
        {
        case 1:
        case 20:
          type = 1;
          break;
        case 2:
        case 23:
          type = 2;
          break;
        case 3:
        case 27:
          type = 3;
          break;
        case 4:
        case 21:
          type = 4;
          break;
        case 5:
        case 24:
          type = 5;
          break;
        case 6:
        case 22:
        case 25:
        case 26:
          type = 7;
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
        if (results.valueHasType<long long int>(i, "date"))
          date = results.getValueAs<long long int>(i, "date");
      }

      /* readable_date - Optional field that has the date in a human readable format. */
      /* OPTIONAL */
      std::string readable_date;
      if (results.valueHasType<long long int>(i, "date"))
      {
        long long int datum = results.getValueAs<long long int>(i, "date");
        std::time_t epoch = datum / 1000;
        std::ostringstream tmp;
        tmp << std::put_time(std::localtime(&epoch), "%b %d, %Y %T");
        readable_date = tmp.str();
      }

      /* address - The phone number of the sender/recipient. */
      /* REQUIRED */
      std::string address;
      if (results.valueHasType<std::string>(i, "address"))
      {
        std::string rid = results.getValueAs<std::string>(i, "address");
        SqliteDB::QueryResults r2;
        d_database.exec("SELECT phone FROM recipient WHERE _id = " + rid, &r2);
        if (r2.rows() == 1 && r2.valueHasType<std::string>(0, "phone"))
          address = r2.getValueAs<std::string>(0, "phone");
        escapeXmlString(&address);
      }

      /* contact_name - Optional field that has the name of the contact. */
      /* OPTIONAL */
      std::string contact_name;
      if (results.valueHasType<std::string>(i, "address"))
      {
        std::string rid = results.getValueAs<std::string>(i, "address");
        SqliteDB::QueryResults r2;
        d_database.exec("SELECT COALESCE(recipient.system_display_name, recipient.signal_profile_name) AS 'contact_name' FROM recipient WHERE _id = " + rid, &r2);
        if (r2.rows() == 1 && r2.valueHasType<std::string>(0, "contact_name"))
          contact_name = r2.getValueAs<std::string>(0, "contact_name");
        escapeXmlString(&contact_name);
      }


      long long int expiration = -1;
      if (results.valueHasType<long long int>(i, "expires_in"))
        expiration = results.getValueAs<long long int>(i, "expires_in");

      /* body - The content of the message. */
      /* REQUIRED */
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
                 << "/>" << std::endl;
    }
    outputfile << "</smses>" << std::endl;
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
  // date - The Java date representation (including millisecond) of the time when the message was sent/received. Check out www.epochconverter.com for information on how to do the conversion from other languages to Java.
  // ct_t - The Content-Type of the message, usually "application/vnd.wap.multipart.related"
  // msg_box - The type of message, 1 = Received, 2 = Sent, 3 = Draft, 4 = Outbox
  // rr - The read-report of the message.
  // sub - The subject of the message, if present.
  // read_status - The read-status of the message.
  // address - The phone number of the sender/recipient.
  // m_id - The Message-ID of the message
  // read - Has the message been read
  // m_size - The size of the message.
  // m_type - The type of the message defined by MMS spec.
  // readable_date - Optional field that has the date in a human readable format.
  // contact_name - Optional field that has the name of the contact.
  // part
  // seq - The order of the part.
  // ct - The content type of the part.
  // name - The name of the part.
  // chset - The charset of the part.
  // cl - The content location of the part.
  // text - The text content of the part.
  // data - The base64 encoded binary content of the part.
  // addr
  // address - The phone number of the sender/recipient.
  // type - The type of address, 129 = BCC, 130 = CC, 151 = To, 137 = From
  // charset - Character set of this entry



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
}
