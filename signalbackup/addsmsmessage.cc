/*
  Copyright (C) 2019-2023  Selwin van Dijk

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

void SignalBackup::addSMSMessage(std::string const &body, std::string const &address, long long int timestamp, long long int thread, bool incoming)
{

  /*
    DEPRECATED, unused and almost useless anyway...
  */

  if (d_databaseversion >= 168)
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Unsupported database version" << std::endl;
    return;
  }

  //get address automatically -> msg partner for normal thread, sender for incoming group, groupid (__textsecure__!xxxxx) for outgoing
  // maybe do something with 'notified'? it is almost always 0, but a few times it is 1 on incoming msgs in my db


  // INSERT INTO sms(_id, thread_id, address, address_device_id, person, date, date_sent, protocol, read, status, type, reply_path_present, delivery_receipt_count, subject, body, mismatched_identities, service_center, subscription_id, expires_in, expire_started, notified, read_receipt_count, unidentified);

  if (incoming)
  {
    if (d_database.tableContainsColumn("sms", "protocol") &&
        d_database.tableContainsColumn("sms", "reply_path_present") &&
        d_database.tableContainsColumn("sms", "service_center")) // removed in dbv166
      d_database.exec("INSERT INTO sms(thread_id, body, " + d_sms_date_received + ", date_sent, " + d_sms_recipient_id + ", type, protocol, read, reply_path_present, service_center) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
                      {thread, body, timestamp, timestamp, address, 10485780ll, 31337ll, 1ll, 1ll, std::string("GCM")});
    else
      d_database.exec("INSERT INTO sms(thread_id, body, " + d_sms_date_received + ", date_sent, " + d_sms_recipient_id + ", type, read) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
                      {thread, body, timestamp, timestamp, address, 10485780ll, 1ll});
  }
  else
  {
    d_database.exec("INSERT INTO sms(thread_id, body, " + d_sms_date_received + ", date_sent, " + d_sms_recipient_id + ", type, read, delivery_receipt_count) VALUES(?, ?, ?, ?, ?, ?, ?, ?)",
                    {thread, body, timestamp, timestamp, address, 10485783ll, 1ll, 1ll});
  }

  // update message count
  updateThreadsEntries(thread);

  /*
    protocol on incoming is 31337? on outgoing always null

    what i know about types quickly...
    type
    10485784 = outgoing, send failed
    2097684 = incoming, safety number changed?
    10551319 = outgoing, updated group
    3 = call
    2 = call
    10485783 = outgoing normal (secure) message, properly received and such
    10458780 = incoming normal (secure) message, properly received and such?
    10747924 = incoming, disabled disappearing msgs
  */
}

/*
void SignalBackup::addMMSMessage()
{

MMS TABLE:
CREATE TABLE mms (
_id INTEGER PRIMARY KEY,
** thread_id INTEGER,
** date INTEGER,
** date_received INTEGER,
** msg_box INTEGER,
read INTEGER DEFAULT 0,
m_id TEXT,
sub TEXT,
sub_cs INTEGER,
** body TEXT,
part_count INTEGER,
ct_t TEXT,
ct_l TEXT,
** address TEXT,  -> for group messages, this is '__textsecure_group__!xxxxx...' for outgoing, +316xxxxxxxx for incoming
address_device_id INTEGER,
exp INTEGER,
m_cls TEXT,
m_type INTEGER,  --> in my database either 132 OR 128, always 128 for outgoing, 132 for incoming
v INTEGER,
m_size INTEGER,
pri INTEGER,
rr INTEGER,
rpt_a INTEGER,
resp_st INTEGER,
st INTEGER,       --> null, or '1' in my database, always null for outgoing, 1 for incoming
tr_id TEXT,
retr_st INTEGER,
retr_txt TEXT,
retr_txt_cs INTEGER,
read_status INTEGER,
ct_cls INTEGER,
resp_txt TEXT,
d_tm INTEGER,
delivery_receipt_count INTEGER DEFAULT 0, --> set auto to number of recipients
mismatched_identities TEXT DEFAULT NULL,
network_failures TEXT DEFAULT NULL,
d_rpt INTEGER,
subscription_id INTEGER DEFAULT -1,
expires_in INTEGER DEFAULT 0,
expire_started INTEGER DEFAULT 0,
notified INTEGER DEFAULT 0,              --> both 0 and 1 present, most often '0' (32553 vs 199), only 1 on incoming types
read_receipt_count INTEGER DEFAULT 0,    --> always zero for me... but...
quote_id INTEGER DEFAULT 0,
quote_author TEXT,
quote_body TEXT,
quote_attachment INTEGER DEFAULT -1,     --> always -1 in my db??
shared_contacts TEXT,
quote_missing INTEGER DEFAULT 0,
unidentified INTEGER DEFAULT 0,           --> both 0 and 1 in my db
previews TEXT)                           -->



CREATE TABLE part (
_id INTEGER PRIMARY KEY,
mid INTEGER,
seq INTEGER DEFAULT 0,
ct TEXT,
name TEXT,
chset INTEGER,
cd TEXT,
fn TEXT,
cid TEXT,
cl TEXT,c
tt_s INTEGER,
ctt_t TEXT,
encrypted INTEGER,
pending_push INTEGER,
_data TEXT,
data_size INTEGER,
file_name TEXT,
thumbnail TEXT,
aspect_ratio REAL,
unique_id INTEGER NOT NULL,
digest BLOB,
fast_preflight_id TEXT,
voice_note INTEGER DEFAULT 0,
data_random BLOB,
thumbnail_random BLOB,
width INTEGER DEFAULT 0,
height INTEGER DEFAULT 0,
quote INTEGER DEFAULT 0,
caption TEXT DEFAULT NULL)
}
*/

/*
replaceAttachment()
*/
