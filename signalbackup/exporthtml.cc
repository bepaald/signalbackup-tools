/*
  Copyright (C) 2023  Selwin van Dijk

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

/*
  Many thanks to Gertjan van den Burg (https://github.com/GjjvdBurg) for his
  original project (used with permission) without which this function would
  not have come together so quickly (if at all).
*/

#include "signalbackup.ih"

bool SignalBackup::exportHtml(std::string const &directory, std::vector<int> const &limittothreads, bool overwrite, bool append) const
{
  using namespace std::string_literals;

  if (d_databaseversion < 170)
  {
    std::cout << "Error, currently unsupported database version. upgrade your database" << std::endl;
    return false;
  }

  // check if dir exists, create if not
  if (!bepaald::fileOrDirExists(directory))
  {
    // try to create
    if (!bepaald::createDir(directory))
    {
      std::cout << "error Failed to create directory `" << directory << "'" << std::endl;
      return false;
    }
  }

  // directory exists, but
  // is it a dir?
  if (!bepaald::isDir(directory))
  {
    std::cout << "error" << std::endl;
    return false;
  }

  // and is it empty?
  if (!bepaald::isEmpty(directory) && !append)
  {
    if (!overwrite)
    {
      std::cout << "Directory '" << directory << "' is not empty. Use --overwrite to clear directory before export" << std::endl;
      return false;
    }
    std::cout << "Clearing contents of directory '" << directory << "'..." << std::endl;
    if (!bepaald::clearDirectory(directory))
    {
      std::cout << "Failed to empty directory '" << directory << "'" << std::endl;
      return false;
    }
  }

  std::vector<int> threads = ((limittothreads.empty() || (limittothreads.size() == 1 && limittothreads[0] == -1)) ?
                              threadIds() : limittothreads);

  std::map<long long int, RecipientInfo> recipient_info;

  // // get releasechannel thread, to skip
  // int releasechannel = -1;
  // for (auto const &skv : d_keyvalueframes)
  //   if (skv->key() == "releasechannel.recipient_id")
  //     releasechannel = bepaald::toNumber<int>(skv->value());

  for (int t : threads)
  {
    // if (t == releasechannel)
    // {
    //   std::cout << "INFO: Skipping releasechannel thread..." << std::endl;
    //   continue;
    // }

    std::cout << "Dealing with thread " << t << std::endl;

    // get recipient_id for thread;
    SqliteDB::QueryResults recid;
    if (!d_database.exec("SELECT _id," + d_thread_recipient_id + " FROM thread WHERE _id = ?", t, &recid) ||
        recid.rows() != 1 || !recid.valueHasType<long long int>(0, d_thread_recipient_id))
    {
      std::cout << "ERROR recid" << std::endl;
      return false;
    }
    long long int thread_id = recid.getValueAs<long long int>(0, "_id");
    long long int thread_recipient_id = recid.getValueAs<long long int>(0, d_thread_recipient_id);

    bool isgroup = false;
    SqliteDB::QueryResults groupcheck;
    d_database.exec("SELECT group_id FROM recipient WHERE _id = ? AND group_id IS NOT NULL", thread_recipient_id, &groupcheck);
    if (groupcheck.rows())
      isgroup = true;

    // get all recipients in thread (group member (past and present), quote/reaction authors, mentions)
    std::set<long long int> all_recipients_ids = getAllThreadRecipients(t);
    //try to set any missing info on recipients
    setRecipientInfo(all_recipients_ids, &recipient_info);
    //for (auto const &ri : recipient_info)
    //  std::cout << ri.first << ": " << ri.second.display_name << std::endl;

    // get conversation name, sanitize it and create dir
    if (recipient_info.find(thread_recipient_id) == recipient_info.end())
    {
      std::cout << "ERROR recid" << std::endl;
      return false;
    }

    std::string threaddir = sanitizeFilename(recipient_info[thread_recipient_id].display_name) + " (_id" + bepaald::toString(thread_id) + ")";
    //if (!append)
    //  makeFilenameUnique(directory, &threaddir);

    if (bepaald::fileOrDirExists(directory + "/" + threaddir))
    {
      if (!bepaald::isDir(directory + "/" + threaddir))
      {
        std::cout << "Error dir is regular file" << std::endl;
        return false;
      }
      if (!append && !overwrite)
      {
        std::cout << "Refusing to overwrite existing directory" << std::endl;
        return false;
      }
    }
    else if (!bepaald::createDir(directory + "/" + threaddir)) // try to create it
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Failed to create directory `" << directory << "/" << threaddir << "'" << std::endl;
      return false;
    }

    // create output-file
    std::ofstream htmloutput(directory + "/" + threaddir + "/" + sanitizeFilename(recipient_info[thread_recipient_id].display_name) + ".html", std::ios_base::binary);
    if (!htmloutput.is_open())
    {
      std::cout << bepaald::bold_on << "ERROR" << bepaald::bold_off
                << ": Failed to open '" << directory << "/" << threaddir << "/" << sanitizeFilename(recipient_info[thread_recipient_id].display_name) << ".html' for writing." << std::endl;
      return false;
    }

    // create start of html (css, head, start of body
    HTMLwriteStart(htmloutput, thread_recipient_id, directory, threaddir, isgroup, all_recipients_ids, recipient_info, overwrite, append);

    // now get all messages, and append them to html
    SqliteDB::QueryResults messages;
    d_database.exec("SELECT "s
                    "_id, recipient_id, body, "
                    "date_received, quote_id, quote_author, quote_body, quote_mentions, " + d_mms_type + ", "
                    "delivery_receipt_count, read_receipt_count, remote_deleted, expires_in "
                    "FROM mms WHERE thread_id = ? ORDER BY date_received ASC", t, &messages);

    std::string previous_day_change;

    for (uint i = 0; i < messages.rows(); ++i)
    {
      long long int msg_id = messages.getValueAs<long long int>(i, "_id");
      long long int msg_recipient_id = messages.getValueAs<long long int>(i, "recipient_id"); // for groups, this != thread_recipient_id on incoming messages
      std::string readable_date = bepaald::toDateString(messages.getValueAs<long long int>(i, "date_received") / 1000,
                                                        "%b %d, %Y %T");
      std::string readable_date_day = bepaald::toDateString(messages.getValueAs<long long int>(i, "date_received") / 1000,
                                                            "%b %d, %Y");
      bool incoming = !Types::isOutgoing(messages.getValueAs<long long int>(i, d_mms_type));
      bool is_deleted = messages.getValueAs<long long int>(i, "remote_deleted") == 1;
      std::string body = messages.valueAsString(i, "body");
      std::string quote_body = messages.valueAsString(i, "quote_body");
      long long int type = messages.getValueAs<long long int>(i, d_mms_type);
      bool isgroupupdatev1 = false;
      bool hasquote = !messages.isNull(i, "quote_id") && messages.getValueAs<long long int>(i, "quote_id");

      SqliteDB::QueryResults attachment_results;
      d_database.exec("SELECT _id,unique_id,ct,sticker_pack_id FROM part WHERE mid IS ? AND quote IS 0", msg_id, &attachment_results);

      SqliteDB::QueryResults quote_attachment_results;
      d_database.exec("SELECT _id, unique_id, ct FROM part WHERE mid IS ? AND quote IS 1", msg_id, &quote_attachment_results);

      SqliteDB::QueryResults mention_results;
      d_database.exec("SELECT recipient_id, range_start, range_length FROM mention WHERE message_id IS ?", msg_id, &mention_results);

      SqliteDB::QueryResults reaction_results;
      d_database.exec("SELECT emoji, author_id, DATETIME(ROUND(date_sent / 1000), 'unixepoch', 'localtime') AS 'date_sent', DATETIME(ROUND(date_received / 1000), 'unixepoch', 'localtime') AS 'date_received'"
                      "FROM reaction WHERE message_id IS ?", msg_id, &reaction_results);

      bool issticker = (attachment_results.rows() == 1 && !attachment_results.isNull(0, "sticker_pack_id"));

      if (Types::isIncomingVideoCall(type))
        body = "Incoming video call";
      else if (Types::isOutgoingVideoCall(type))
        body = "Outgoing video call";
      else if (Types::isMissedVideoCall(type))
        body = "Missed video call";
      else if (Types::isIncomingCall(type))
        body = "Incoming voice call";
      else if (Types::isOutgoingCall(type))
        body = "Outgoing voice call";
      else if (Types::isMissedCall(type))
        body = "Missed voice call";
      else if (Types::isGroupCall(type))
        body = "Group call";
      else if (Types::isGroupUpdate(type)) // group v2: to do...
      {
        if (!Types::isGroupV2(type))
        {
          isgroupupdatev1 = true;
          body = decodeStatusMessage(body, messages.getValueAs<long long int>(i, "expires_in"),
                                     type, recipient_info[msg_recipient_id].display_name);
        }
        else if (Types::isGroupQuit(type)) // done here because groupquit can also be groupv2
          body = decodeStatusMessage(body, messages.getValueAs<long long int>(i, "expires_in"),
                                     type, recipient_info[msg_recipient_id].display_name);
        else
          body = "(group V2 update)";
      }
      else if (Types::isProfileChange(type))
        body = decodeProfileChangeMessage(body, recipient_info.at(msg_recipient_id).display_name);
      else if (Types::isIdentityUpdate(type) || Types::isIdentityVerified(type) || Types::isIdentityDefault(type) ||
               Types::isExpirationTimerUpdate(type) || Types::isJoined(type) || Types::isProfileChange(type))
        body = decodeStatusMessage(body, messages.getValueAs<long long int>(i, "expires_in"), type, recipient_info.at(msg_recipient_id).display_name);

      // prep body (scan emoji? -> in <span>) and handle mentions...
      // if (prepbody)
      std::vector<std::tuple<long long int, long long int, long long int>> mentions;
      for (uint mi = 0; mi < mention_results.rows(); ++mi)
        mentions.emplace_back(std::make_tuple(mention_results.getValueAs<long long int>(mi, "recipient_id"),
                                              mention_results.getValueAs<long long int>(mi, "range_start"),
                                              mention_results.getValueAs<long long int>(mi, "range_length")));

      bool only_emoji = HTMLprepMsgBody(&body, mentions, recipient_info, incoming, false /*isquote*/);

      bool nobackground = false;
      if ((only_emoji && !hasquote && !attachment_results.rows()) ||  // if no quote etc
          issticker) // or sticker
        nobackground = true;

      // same for quote_body!
      mentions.clear();
      // protospec (app/src/main/proto/Database.proto):
      // message BodyRangeList {
      //     message BodyRange {
      //         enum Style {
      //             BOLD   = 0;
      //             ITALIC = 1;
      //         }
      //
      //         message Button {
      //             string label  = 1;
      //             string action = 2;
      //         }
      //
      //         int32 start  = 1;
      //         int32 length = 2;
      //
      //         oneof associatedValue {
      //             string mentionUuid = 3;
      //             Style  style       = 4;
      //             string link        = 5;
      //             Button button      = 6;
      //         }
      //     }
      //     repeated BodyRange ranges = 1;
      // }
      if (!messages.isNull(i, "quote_mentions"))
      {
        std::pair<std::shared_ptr<unsigned char []>, size_t> quote_mentions = messages.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "quote_mentions");
        ProtoBufParser<std::vector<ProtoBufParser<protobuffer::optional::INT32, // int32 start
                                                  protobuffer::optional::INT32, // int32 length
                                                  protobuffer::optional::STRING // in place of the oneof?
                                                  >>> bodyrangelist(quote_mentions.first.get(), quote_mentions.second);
        for (uint qm = 0; qm < bodyrangelist.getField<1>().size(); ++qm)
        {
          long long int start = bodyrangelist.getField<1>()[qm].getField<1>().value_or(-1);
          long long int length = bodyrangelist.getField<1>()[qm].getField<2>().value_or(-1);
          std::string uuid = bodyrangelist.getField<1>()[qm].getField<3>().value_or("");
          long long int mrid = getRecipientIdFromUuid(uuid, nullptr);
          //std::cout << start << " " << length << " " << mrid << std::endl;
          if (start > -1 && length > -1 && mrid > -1)
            mentions.emplace_back(std::make_tuple(mrid, start, length));
        }
      }
      HTMLprepMsgBody(&quote_body, mentions, recipient_info, incoming, true);

      // insert date-change message
      if (readable_date_day != previous_day_change)
      {
        htmloutput << R"(      <div class="msg msg-date-change">
        <p>
          )" << readable_date_day << R"(
        </p>
      </div>)" << std::endl << std::endl;
      }
      previous_day_change = readable_date_day;

      // collect data needed by writeMessage()
      HTMLMessageInfo msg_info({only_emoji,
                                is_deleted,
                                isgroup,
                                incoming,
                                isgroupupdatev1,
                                nobackground,
                                hasquote,
                                overwrite,
                                append,
                                type,
                                msg_id,
                                msg_recipient_id,
                                i,
                                &messages,
                                &quote_attachment_results,
                                &attachment_results,
                                &reaction_results,
                                body,
                                quote_body,
                                readable_date,
                                directory,
                                threaddir});
      HTMLwriteMessage(htmloutput, msg_info, &recipient_info);
    }

    htmloutput << "    </div>" << std::endl;
    htmloutput << "  </body>" << std::endl;
    htmloutput << "</html>" << std::endl;

  }
  return false;
}
