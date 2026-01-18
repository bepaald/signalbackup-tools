/*
  Copyright (C) 2021-2026  Selwin van Dijk

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
//#include <chrono>

bool SignalBackup::reorderMmsSmsIds() const
{
  //auto t1 = std::chrono::high_resolution_clock::now();

  Logger::message_start(__FUNCTION__);

  bool adjustmention = d_database.containsTable("mention");
  bool adjustmsl_message = d_database.containsTable("msl_message");
  bool msl_has_is_mms = adjustmsl_message && d_database.tableContainsColumn("msl_message", "is_mms");
  bool adjustreaction = d_database.containsTable("reaction");
  bool reaction_has_is_mms = adjustreaction && d_database.tableContainsColumn("reaction", "is_mms");
  bool adjuststorysends = d_database.containsTable("story_sends");
  bool adjustcall = d_database.containsTable("call");
  bool adjustoriginal_message_id = d_database.tableContainsColumn(d_mms_table, "original_message_id");
  bool adjustlatest_revision_id = d_database.tableContainsColumn(d_mms_table, "latest_revision_id");
  bool adjustpinning_message_id = d_database.tableContainsColumn(d_mms_table, "pinning_message_id"); // column has default 0, but only refers to a message when != 0
  bool adjustpoll = d_database.containsTable("poll");

  // get all mms in the correct order
  SqliteDB::QueryResults res;
  if (!d_database.exec(bepaald::concat("SELECT "
                                       "_id"                                          // idx = 0
                                       ", attcount",                                  // idx = 1
                                       (adjustreaction ? ", reactioncount" : ""),
                                       (adjustmention ? ", mentioncount" : ""),
                                       ", groupreceiptcount",
                                       (adjuststorysends ? ", storysendscount" : ""),
                                       (adjustcall ? ", callcount" : ""),
                                       (adjustmsl_message ? ", mslcount" : ""),
                                       (adjustoriginal_message_id ? ", og_msgcount" : ""),
                                       (adjustlatest_revision_id ? ", revisioncount" : ""),
                                       (adjustpinning_message_id ? ", pincount" : ""),
                                       (adjustpoll ? ", pollcount" : ""),
                                       " FROM ", d_mms_table, " ",
                                       // get attachment count for message:
                                       "LEFT JOIN (SELECT ", d_part_mid, " AS message_id, COUNT(*) AS attcount FROM ", d_part_table, " GROUP BY message_id) AS attmnts ON ", d_mms_table, "._id = attmnts.message_id ",
                                       // get reaction count for message:
                                       (adjustreaction ? "LEFT JOIN (SELECT message_id, COUNT(*) AS reactioncount FROM reaction GROUP BY message_id) AS rctns ON " + d_mms_table + "._id = rctns.message_id " : ""),
                                       // get mention count for message:
                                       (adjustmention ? "LEFT JOIN (SELECT message_id, COUNT(*) AS mentioncount FROM mention GROUP BY message_id) AS mntns ON " + d_mms_table + "._id = mntns.message_id " : ""),
                                       // get poll count for message:
                                       (adjustmention ? "LEFT JOIN (SELECT message_id, COUNT(*) AS pollcount FROM poll GROUP BY message_id) AS plls ON " + d_mms_table + "._id = plls.message_id " : ""),
                                       // get group receipt count for message:
                                       "LEFT JOIN (SELECT mms_id, COUNT(*) AS groupreceiptcount FROM group_receipts GROUP BY mms_id) AS grprct ON ", d_mms_table, "._id = grprct.mms_id ",
                                       // get story_sends count for message:
                                       (adjuststorysends ? "LEFT JOIN (SELECT message_id, COUNT(*) AS storysendscount FROM story_sends GROUP BY message_id) AS ss ON " + d_mms_table + "._id = ss.message_id " : ""),
                                       // get call count for message:
                                       (adjustcall ? "LEFT JOIN (SELECT message_id, COUNT(*) AS callcount FROM call GROUP BY message_id) AS cc ON " + d_mms_table + "._id = cc.message_id " : ""),
                                       // get msl count for message:
                                       (adjustmsl_message ? "LEFT JOIN (SELECT message_id, COUNT(*) AS mslcount FROM msl_message GROUP BY message_id) AS msl ON " + d_mms_table + "._id = msl.message_id " : ""),
                                       // get original_message count for message:
                                       (adjustoriginal_message_id ? "LEFT JOIN (SELECT original_message_id, COUNT(*) AS og_msgcount FROM " + d_mms_table + " GROUP BY original_message_id) AS ogmsg ON " + d_mms_table + "._id = ogmsg.original_message_id " : ""),
                                       // get latest_revision count for message:
                                       (adjustlatest_revision_id ? "LEFT JOIN (SELECT latest_revision_id, COUNT(*) AS revisioncount FROM " + d_mms_table + " GROUP BY latest_revision_id) AS lr ON " + d_mms_table + "._id = lr.latest_revision_id " : ""),
                                       // get latest_revision count for message:
                                       (adjustpinning_message_id ? "LEFT JOIN (SELECT pinning_message_id, COUNT(*) AS pincount FROM " + d_mms_table + " GROUP BY pinning_message_id) AS pm ON " + d_mms_table + "._id = pm.pinning_message_id " : ""),
                                       "ORDER BY date_received ASC"), &res)) // for sms table, use 'date'
    return false;

  d_database.exec("BEGIN TRANSACTION");

  // set all id's 'negatively ascending' (negative because of UNIQUE constraint)
  long long int total = res.rows();

  for (long long int i = 0; i < total; ++i)
  {
    std::any oldid = res.value(i, 0);
    if (!d_database.exec("UPDATE " + d_mms_table + " SET _id = ? WHERE _id = ?", {-1 * (i + 1), oldid})) [[unlikely]]
      return false;
  }
  Logger::message_continue(".");

  for (long long int i = 0; i < total; ++i)
  {
    if (res.valueAsInt(i, 1, 0) > 0) // check attcount
    {
      std::any oldid = res.value(i, 0);
      if (!d_database.exec("UPDATE  " + d_part_table + " SET  " + d_part_mid + " = ? WHERE " + d_part_mid + " = ?", {-1 * (i + 1), oldid})) [[unlikely]]
        return false;
    }
  }

  for (long long int i = 0; i < total; ++i)
  {
    if (res.valueAsInt(i, "groupreceiptcount", 0) > 0) // check groupreceiptcount
    {
      std::any oldid = res.value(i, 0);
      if (!d_database.exec("UPDATE group_receipts SET mms_id = ? WHERE mms_id = ?", {-1 * (i + 1), oldid})) [[unlikely]]
        return false;
    }
  }

  if (adjustmention)
    for (long long int i = 0; i < total; ++i)
    {
      if (res.valueAsInt(i, "mentioncount", 0) > 0) // check mentioncount
      {
        std::any oldid = res.value(i, 0);
        if (!d_database.exec("UPDATE mention SET message_id = ? WHERE message_id = ?", {-1 * (i + 1), oldid})) [[unlikely]]
          return false;
      }
    }

  if (adjustpoll)
    for (long long int i = 0; i < total; ++i)
    {
      if (res.valueAsInt(i, "pollcount", 0) > 0) // check pollcount
      {
        std::any oldid = res.value(i, 0);
        if (!d_database.exec("UPDATE poll SET message_id = ? WHERE message_id = ?", {-1 * (i + 1), oldid})) [[unlikely]]
          return false;
      }
    }

  if (adjustmsl_message)
    for (long long int i = 0; i < total; ++i)
    {
      if (res.valueAsInt(i, "mslcount", 0) > 0) // check mslcount
      {
        std::any oldid = res.value(i, 0);
        if (!d_database.exec("UPDATE msl_message SET message_id = ? WHERE message_id = ?"s + (msl_has_is_mms ? " AND is_mms IS 1" : ""), {-1 * (i + 1), oldid})) [[unlikely]]
          return false;
      }
    }

  if (adjustreaction)
    for (long long int i = 0; i < total; ++i)
    {
      if (res.valueAsInt(i, "reactioncount", 0) > 0) // check reactioncount
      {
        std::any oldid = res.value(i, 0);
        if (!d_database.exec("UPDATE reaction SET message_id = ? WHERE message_id = ?"s + (reaction_has_is_mms ? " AND is_mms IS 1" : ""), {-1 * (i + 1), oldid})) [[unlikely]]
          return false;
      }
    }

  if (adjuststorysends)
    for (long long int i = 0; i < total; ++i)
    {
      if (res.valueAsInt(i, "storysendscount", 0) > 0) // check storysendscount
      {
        std::any oldid = res.value(i, 0);
        if (!d_database.exec("UPDATE story_sends SET message_id = ? WHERE message_id = ?", {-1 * (i + 1), oldid})) [[unlikely]]
          return false;
      }
    }

  if (adjustcall)
    for (long long int i = 0; i < total; ++i)
    {
      if (res.valueAsInt(i, "callcount", 0) > 0) // check callcount
      {
        std::any oldid = res.value(i, 0);
        if (!d_database.exec("UPDATE call SET message_id = ? WHERE message_id = ?", {-1 * (i + 1), oldid})) [[unlikely]]
          return false;
      }
    }

  if (adjustoriginal_message_id)
    for (long long int i = 0; i < total; ++i)
    {
      if (res.valueAsInt(i, "og_msgcount", 0) > 0)
      {
        std::any oldid = res.value(i, 0);
        if (!d_database.exec("UPDATE " + d_mms_table + " SET original_message_id = ? WHERE original_message_id = ?", {-1 * (i + 1), oldid})) [[unlikely]]
          return false;
      }
    }

  Logger::message_continue(".");

  if (adjustlatest_revision_id)
    for (long long int i = 0; i < total; ++i)
    {
      if (res.valueAsInt(i, "revisioncount", 0) > 0)
      {
        std::any oldid = res.value(i, 0);
        if (!d_database.exec("UPDATE " + d_mms_table + " SET latest_revision_id = ? WHERE latest_revision_id = ?", {-1 * (i + 1), oldid})) [[unlikely]]
          return false;
      }
    }

  // THIS ONE IS UNTESTED
  if (adjustpinning_message_id)
    for (long long int i = 0; i < total; ++i)
    {
      if (res.valueAsInt(i, "pincount", 0) > 0) // check pinning message count
      {
        std::any oldid = res.value(i, 0);
        if (!d_database.exec("UPDATE " + d_mms_table + " SET pinning_message_id = ? WHERE pinning_message_id = ?", {-1 * (i + 1), oldid})) [[unlikely]]
          return false;
      }
    }

  /*
  for (unsigned int i = 0; i < total; ++i)
  {
    if (d_showprogress && i % 1000 == 0)
      Logger::message_overwrite(__FUNCTION__, " (", i, "/", total, ")");

    std::any oldid = res.value(i, 0);
    ++negative_id_tmp;
    if (!d_database.exec("UPDATE " + d_mms_table + " SET _id = ? WHERE _id = ?", {-1 * negative_id_tmp, oldid}) ||
        !d_database.exec("UPDATE  " + d_part_table + " SET  " + d_part_mid + " = ? WHERE " + d_part_mid + " = ?", {-1 * negative_id_tmp, oldid}) ||
        !d_database.exec("UPDATE group_receipts SET mms_id = ? WHERE mms_id = ?", {-1 * negative_id_tmp, oldid}))
      return false;
    if (adjustmention && !d_database.exec("UPDATE mention SET message_id = ? WHERE message_id = ?", {-1 * negative_id_tmp, oldid}))
      return false;
    if (adjustmsl_message &&
        !d_database.exec("UPDATE msl_message SET message_id = ? WHERE message_id = ?"s + (msl_has_is_mms ? " AND is_mms IS 1" : ""), {-1 * negative_id_tmp, oldid}))
      return false;
    if (adjustreaction && !d_database.exec("UPDATE reaction SET message_id = ? WHERE message_id = ?"s + (reaction_has_is_mms ? " AND is_mms IS 1" : ""), {-1 * negative_id_tmp, oldid}))
      return false;
    if (adjuststorysends && !d_database.exec("UPDATE story_sends SET message_id = ? WHERE message_id = ?", {-1 * negative_id_tmp, oldid}))
      return false;
    if (adjustcall && !d_database.exec("UPDATE call SET message_id = ? WHERE message_id = ?", {-1 * negative_id_tmp, oldid}))
      return false;
    if (adjustoriginal_message_id && !d_database.exec("UPDATE " + d_mms_table + " SET original_message_id = ? WHERE original_message_id = ?", {-1 * negative_id_tmp, oldid}))
      return false;
    if (adjustlatest_revision_id && !d_database.exec("UPDATE " + d_mms_table + " SET latest_revision_id = ? WHERE latest_revision_id = ?", {-1 * negative_id_tmp, oldid}))
      return false;
  }
  */

  d_database.exec("COMMIT");

  // now make all id's positive again
  d_database.exec("BEGIN TRANSACTION");

  Logger::message_continue(".");

  if (!d_database.exec("UPDATE " + d_mms_table + " SET _id = _id * -1 WHERE _id < 0")) [[unlikely]]
    return false;

  if (!d_database.exec("UPDATE " + d_part_table + " SET " + d_part_mid + " = " + d_part_mid + " * -1 WHERE " + d_part_mid + " < 0")) [[unlikely]]
    return false;

  if (!d_database.exec("UPDATE group_receipts SET mms_id = mms_id * -1 WHERE mms_id < 0")) [[unlikely]]
    return false;

  if (adjustmention && !d_database.exec("UPDATE mention SET message_id = message_id * -1 WHERE message_id < 0")) [[unlikely]]
    return false;
  if (adjustpoll && !d_database.exec("UPDATE poll SET message_id = message_id * -1 WHERE message_id < 0")) [[unlikely]]
    return false;
  if (adjustmsl_message && !d_database.exec("UPDATE msl_message SET message_id = message_id * -1 WHERE message_id < 0"s + (msl_has_is_mms ? " AND is_mms IS 1" : ""))) [[unlikely]]
    return false;
  if (adjustreaction && !d_database.exec("UPDATE reaction SET message_id = message_id * -1 WHERE message_id < 0"s + (reaction_has_is_mms ? " AND is_mms IS 1" : ""))) [[unlikely]]
    return false;
  if (adjuststorysends && !d_database.exec("UPDATE story_sends SET message_id = message_id * -1 WHERE message_id < 0")) [[unlikely]]
    return false;
  if (adjustcall && !d_database.exec("UPDATE call SET message_id = message_id * -1 WHERE message_id < 0")) [[unlikely]]
    return false;
  if (adjustoriginal_message_id && !d_database.exec("UPDATE " + d_mms_table + " SET original_message_id = original_message_id * -1 WHERE original_message_id < 0")) [[unlikely]]
    return false;
  if (adjustlatest_revision_id && !d_database.exec("UPDATE " + d_mms_table + " SET latest_revision_id = latest_revision_id * -1 WHERE latest_revision_id < 0")) [[unlikely]]
    return false;
  // THIS ONE IS UNTESTED
  if (adjustpinning_message_id && !d_database.exec("UPDATE " + d_mms_table + " SET pinning_message_id = pinning_message_id * -1 WHERE pinning_message_id < 0")) [[unlikely]]
    return false;

  d_database.exec("COMMIT");

  // SAME FOR SMS
  if (d_database.containsTable("sms")) // removed in 168
  {
    if (!d_database.exec("SELECT _id FROM sms ORDER BY " + d_sms_date_received + " ASC", &res))
      return false;

    long long int negative_id_tmp = 0;
    for (unsigned int i = 0; i < res.rows(); ++i)
    {
      long long int oldid = res.getValueAs<long long int>(i, 0);
    ++negative_id_tmp;
    if (!d_database.exec("UPDATE sms SET _id = ? WHERE _id = ?", {-1 * negative_id_tmp, oldid}))
      return false;
    if (d_database.containsTable("msl_message"))
      if (!d_database.exec("UPDATE msl_message SET message_id = ? WHERE message_id = ?"s + (d_database.tableContainsColumn("msl_message", "is_mms") ? " AND is_mms IS NOT 1" : ""), {-1 * negative_id_tmp, oldid}))
        return false;
    if (d_database.containsTable("reaction")) // dbv >= 121
      if (!d_database.exec("UPDATE reaction SET message_id = ? WHERE message_id = ?"s + (d_database.tableContainsColumn("reaction", "is_mms") ? " AND is_mms IS NOT 1" : ""), {-1 * negative_id_tmp, oldid}))
        return false;
    }

    if (!d_database.exec("UPDATE sms SET _id = _id * -1 WHERE _id < 0"))
      return false;
    if (d_database.containsTable("msl_message"))
      if (!d_database.exec("UPDATE msl_message SET message_id = message_id * -1 WHERE message_id < 0"s + (d_database.tableContainsColumn("msl_message", "is_mms") ? " AND is_mms IS NOT 1" : "")))
        return false;
    if (d_database.containsTable("reaction")) // dbv >= 121
      if (!d_database.exec("UPDATE reaction SET message_id = message_id * -1 WHERE message_id < 0"s + (d_database.tableContainsColumn("reaction", "is_mms") ? " AND is_mms IS NOT 1" : "")))
        return false;
  }

  // auto t2 = std::chrono::high_resolution_clock::now();
  // auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
  // std::cout << " *** TIME: " << ms_int.count() << "ms\n";

  Logger::message_end("ok");
  return true;
}
