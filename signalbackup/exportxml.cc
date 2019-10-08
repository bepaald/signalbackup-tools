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


/*
void SignalBackup::exportXml(std::string const &filename) const
{
  if (checkFileExists(filename))
  {
    std::cout << "File " << filename << " exists. Refusing to overwrite" << std::endl;
    return;
  }

  SqliteDB::QueryResults results;
  d_database.exec("SELECT count(*) FROM sms", &results);
  long long int smscount = 0;
  if (results.valueHasType<long long int>(0, 0))
    smscount = results.getValueAs<long long int>(0, 0);
  d_database.exec("SELECT count(*) FROM mms", &results);
  long long int mmscount = 0;
  if (results.valueHasType<long long int>(0, 0))
    mmscount = results.getValueAs<long long int>(0, 0);

  std::ofstream outputfile(filename, std::ios_base::binary);
  outputfile << "<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>";
  outputfile << "<?xml-stylesheet type=\"text/xsl\" href=\"sms.xsl\"?>";
  if (smscount)
  {
    outputfile << "<smses count=" << bepaald::toString(smscount) << ">";
    for (uint i = 0; i < smscount; ++i)
    {
      long long int protocol = 0; // optional, ubyte
      std::string address = "31647474974"; // required, string
      long long int date = 0; // req, ulong
      std::string readable_date = ""; // opt, string
      long long int type = 0; // req, ubyte
      std::string body = ""; // required, string
      std::string service_center = "null"; // opt, string // null for sent... ?? for received
      long long int read = 0; // req, ubyte
      long long int status = 0; // req, byte
      std::string contact_name = ""; // opt, string
      long long int locked = 0; // opt, ubyte;

      outputfile << "  <sms "
                 << "protocol=\"" << protocol << "\" "
                 << "address=\"" << address << "\" "
                 << "date=\"" << date << "\" "
                 << "type=\"" << type << "\" "
                 << "subject=\"" << "null" << "\" "
                 << "body=\"" << body << "\" "
                 << "toa=\"" << "null" << "\" "
                 << "sc_toa=\"" << "null" << "\" "
                 << "service_center=\"" << service_center << "\" "
                 << "read=\"" << read << "\" "
                 << "status=\"" << status << "\" "
                 << "locked=\"" << locked << "\" "
                 << "readable_date=\"" << readable_date << "\" "
                 << "contact_name=\"" << contact_name << "\" "
                 << "/>";
    }
    outputfile << "</smses>";
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
*/
