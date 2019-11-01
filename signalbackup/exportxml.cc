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
  d_database.exec("SELECT protocol,subject,service_center,read,status,date_sent,date,address,type,body FROM sms", &results);
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
        subject = results.getValueAs<std::string>(i, "subject");

      /* service_center - The service center for the received message, null in case of sent messages. */
      /* OPTIONAL */
      std::string service_center;
      if (results.valueHasType<std::string>(i, "service_center"))
        service_center = results.getValueAs<std::string>(i, "service_center");

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
      if (results.valueHasType<long long int>(i, "type"))
      {
        long long int t = results.getValueAs<long long int>(i, "type");

        switch (t & Types::BASE_TYPE_MASK)
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
      }

      /* body - The content of the message. */
      /* REQUIRED */
      std::string body;
      if (results.valueHasType<std::string>(i, "body"))
        body = results.getValueAs<std::string>(i, "body");

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
      }

      outputfile << "  <sms "
                 << "protocol=\"" << protocol << "\" "
                 << "subject=\"" << (subject.empty() ? std::string("null") : subject) << "\" "
                 << "address=\"" << address << "\" "
                 << "date=\"" << date << "\" "
                 << "type=\"" << type << "\" "
                 << "body=\"" << body << "\" "
                 << "toa=\"" << "null" << "\" "
                 << "sc_toa=\"" << "null" << "\" "
                 << "service_center=\"" << (service_center.empty() ? std::string("null") : service_center)<< "\" "
                 << "read=\"" << read << "\" "
                 << "status=\"" << status << "\" "
                 << "readable_date=\"" << (readable_date.empty() ? std::string("null") : readable_date) << "\" "
                 << "contact_name=\"" << (contact_name.empty() ? std::string("null") : contact_name) << "\" "
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
