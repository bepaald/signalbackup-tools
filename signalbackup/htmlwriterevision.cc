/*
  Copyright (C) 2023-2024  Selwin van Dijk

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

void SignalBackup::HTMLwriteRevision(long long int msg_id, std::ofstream &filt, HTMLMessageInfo const &parent_info,
                                     std::map<long long int, RecipientInfo> *recipient_info) const
{
  SqliteDB::QueryResults revision;
  if (!d_database.exec("SELECT " +
                       d_mms_recipient_id + ", " +
                       d_mms_date_sent + ", " +
                       d_mms_type + ", "
                       "body, quote_missing, quote_author, quote_body, " + d_mms_delivery_receipts + ", " + d_mms_read_receipts + ", "
                       "json_extract(link_previews, '$[0].title') AS link_preview_title, "
                       "json_extract(link_previews, '$[0].description') AS link_preview_description, " +
                       (d_database.tableContainsColumn(d_mms_table, "receipt_timestamp") ? "receipt_timestamp, " : "-1 AS receipt_timestamp, ") +
                       (d_database.tableContainsColumn(d_mms_table, "message_extras") ? "message_extras, " : "") +
                       "shared_contacts, quote_id, expires_in, " + d_mms_ranges + ", quote_mentions"
                       " FROM message WHERE _id = ?", msg_id, &revision) ||
      revision.rows() != 1)
    return;

  long long int msg_recipient_id = revision.valueAsInt(0, d_mms_recipient_id);
  std::string readable_date =
    bepaald::toDateString(revision.getValueAs<long long int>(0, d_mms_date_sent) / 1000, "%b %d, %Y %H:%M:%S");

  bool incoming = !Types::isOutgoing(revision.getValueAs<long long int>(0, d_mms_type));
  std::string body = revision.valueAsString(0, "body");
  std::string shared_contacts = revision.valueAsString(0, "shared_contacts");
  std::string quote_body = revision.valueAsString(0, "quote_body");
  long long int type = revision.getValueAs<long long int>(0, d_mms_type);
  long long int expires_in = revision.getValueAs<long long int>(0, "expires_in");
  bool hasquote = !revision.isNull(0, "quote_id") && revision.getValueAs<long long int>(0, "quote_id");
  bool quote_missing = revision.valueAsInt(0, "quote_missing", 0) != 0;

  SqliteDB::QueryResults attachment_results;
  d_database.exec("SELECT "
                  "_id, " +
                  (d_database.tableContainsColumn(d_part_table, "unique_id") ? "unique_id"s : "-1 AS unique_id") + ", " +
                  d_part_ct + ", "
                  "file_name, "
                  + d_part_pending + ", " +
                  (d_database.tableContainsColumn(d_part_table, "caption") ? "caption, "s : std::string()) +
                  "sticker_pack_id "
                  "FROM " + d_part_table + " WHERE " + d_part_mid + " IS ? AND quote IS 0", msg_id, &attachment_results);
  // check attachments for long message body -> replace cropped body & remove from attachment results
  setLongMessageBody(&body, &attachment_results);

  SqliteDB::QueryResults quote_attachment_results;
        d_database.exec("SELECT "
                        "_id," +
                        (d_database.tableContainsColumn(d_part_table, "unique_id") ? "unique_id"s : "-1 AS unique_id") + ", " +
                        d_part_ct + ", "
                        "file_name,"
                        + d_part_pending + ", "
                        "sticker_pack_id "
                        "FROM " + d_part_table + " WHERE " + d_part_mid + " IS ? AND quote IS 1", msg_id, &quote_attachment_results);

  SqliteDB::QueryResults mention_results;
  d_database.exec("SELECT recipient_id, range_start, range_length FROM mention WHERE message_id IS ?", msg_id, &mention_results);

  SqliteDB::QueryResults reaction_results;
  d_database.exec("SELECT emoji, author_id, DATETIME(ROUND(date_sent / 1000), 'unixepoch', 'localtime') AS 'date_sent', DATETIME(ROUND(date_received / 1000), 'unixepoch', 'localtime') AS 'date_received'"
                  "FROM reaction WHERE message_id IS ?", msg_id, &reaction_results);

  SqliteDB::QueryResults edit_revisions; // leave empty

  bool issticker = (attachment_results.rows() == 1 && !attachment_results.isNull(0, "sticker_pack_id"));

  IconType icon = IconType::NONE;
  if (Types::isStatusMessage(type))
  {
    if (!body.empty())
      body = decodeStatusMessage(body, revision.getValueAs<long long int>(0, "expires_in"), type, getRecipientInfoFromMap(recipient_info, msg_recipient_id).display_name, &icon);
    else if (d_database.tableContainsColumn(d_mms_table, "message_extras") &&
             revision.valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(0, "message_extras"))
      body = decodeStatusMessage(revision.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(0, "message_extras"),
                                 revision.getValueAs<long long int>(0, "expires_in"), type, getRecipientInfoFromMap(recipient_info, msg_recipient_id).display_name, &icon);
  }

  // prep body (scan emoji? -> in <span>) and handle mentions...
  // if (prepbody)
  std::vector<std::tuple<long long int, long long int, long long int>> mentions;
  for (unsigned int mi = 0; mi < mention_results.rows(); ++mi)
    mentions.emplace_back(std::make_tuple(mention_results.getValueAs<long long int>(mi, "recipient_id"),
                                          mention_results.getValueAs<long long int>(mi, "range_start"),
                                          mention_results.getValueAs<long long int>(mi, "range_length")));
  std::pair<std::shared_ptr<unsigned char []>, size_t> brdata(nullptr, 0);
  if (!revision.isNull(0, d_mms_ranges))
    brdata = revision.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(0, d_mms_ranges);

  bool only_emoji = HTMLprepMsgBody(&body, mentions, recipient_info, incoming, brdata, false /*isquote*/);

  bool nobackground = false;
  if ((only_emoji && !hasquote && !attachment_results.rows()) ||  // if no quote etc
      issticker) // or sticker
    nobackground = true;

  // same for quote_body!
  mentions.clear();
  std::pair<std::shared_ptr<unsigned char []>, size_t> quote_mentions{nullptr, 0};
  if (!revision.isNull(0, "quote_mentions"))
    quote_mentions = revision.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(0, "quote_mentions");
  HTMLprepMsgBody(&quote_body, mentions, recipient_info, incoming, quote_mentions, true);

  HTMLMessageInfo msg_info({only_emoji,
                            false, //is_deleted,
                            false, //is_viewonce,
                            parent_info.isgroup,
                            incoming,
                            nobackground,
                            hasquote,
                            quote_missing,
                            parent_info.overwrite, // ?
                            parent_info.append,    // ?
                            parent_info.story_reply,
                            type,
                            expires_in,
                            msg_id,
                            msg_recipient_id,
                            -1, //original_message_id,
                            0, // messagecount, // idx of current message in &messages

                            &revision,
                            &quote_attachment_results,
                            &attachment_results,
                            &reaction_results,
                            &edit_revisions,

                            body,
                            quote_body,
                            readable_date,
                            parent_info.directory,
                            parent_info.threaddir,
                            parent_info.filename,
                            revision(0, "link_preview_title"),
                            revision(0, "link_preview_description"),
                            shared_contacts,

                            icon});

  HTMLwriteMessage(filt, msg_info, recipient_info, false /*searchpage*/, false /*writereceipts*/);
}
