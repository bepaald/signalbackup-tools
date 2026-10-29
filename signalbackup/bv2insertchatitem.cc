/*
  Copyright (C) 2026  Selwin van Dijk

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

#ifdef BEPAALD_BV2_ENABLED

#include "signalbackup.ih"

#include <functional>
#include <limits>

#include "../backupv2proto_typedef/backupv2proto_typedef.h"
#include "../groupv2statusmessageproto_typedef/groupv2statusmessageproto_typedef.h"
#include "../protobufparser/protobufparser.h"

/*
  Converts a single ChatItem into a row in the message table (plus attachments,
  reactions, mentions, etc). This mirrors what Signal Android does when restoring:
  https://github.com/signalapp/Signal-Android/blob/main/app/src/main/java/org/thoughtcrime/securesms/backup/v2/importer/ChatItemArchiveImporter.kt

  *new_mid is set to the new message._id, or -1 if the item was skipped. Returns
  false only on (fatal) errors.
*/
bool SignalBackup::bv2InsertChatItem(BackupV2::ChatItem const &ci, long long int thread_id, long long int chat_rid, uint64_t chat_backup_rid,
                                     long long int author_rid, BackupV2ImportState *state, long long int *new_mid)
{
  *new_mid = -1;

  // a couple of Android constants not (yet) in Types
  uint64_t constexpr ENCRYPTION_REMOTE_FAILED_BIT = 0x10000000;
  uint64_t constexpr SESSION_SWITCHOVER_TYPE = 18;
  uint64_t constexpr SPECIAL_TYPE_BLOCKED = 0xA00000000;
  uint64_t constexpr SPECIAL_TYPE_UNBLOCKED = 0xB00000000;
  long long int constexpr QUOTE_TARGET_MISSING_ID = -1;
  long long int constexpr PARENT_STORY_MISSING_ID = -1;

  auto jsonArray = [](std::vector<std::string> const &items) STATICLAMBDA
  {
    std::string result("[");
    for (unsigned int i = 0; i < items.size(); ++i)
      result += (i == 0 ? "" : ",") + items[i];
    return result + "]";
  };

  auto toBlob = [](ProtoBufParserBase const &p) STATICLAMBDA
  {
    return std::pair<unsigned char *, unsigned long long>(p.data(), p.size());
  };

  uint64_t date_sent = ci.getFieldView<3>().value_or(0);
  auto incoming = ci.getFieldView<8>();
  auto outgoing = ci.getFieldView<9>();
  bool sms = ci.getFieldView<7>().value_or(false);

  long long int from_rid = author_rid;
  long long int to_rid = outgoing.has_value() ? chat_rid : d_selfid;

  // DETERMINE BASE TYPE
  std::vector<BackupV2::SendStatus> sendstatus;
  if (outgoing.has_value())
    sendstatus = outgoing->getFieldView<1>();

  uint64_t type = Types::BASE_INBOX_TYPE;
  if (outgoing.has_value())
  {
    // sendstatus: pending = 3, sent = 4, delivered = 5, read = 6, viewed = 7, skipped = 8, failed = 9
    auto any = [&sendstatus](auto check) { return std::any_of(sendstatus.begin(), sendstatus.end(), check); };
    if (any([](auto const &s) { return s.template getFieldView<3>().has_value(); }))
      type = Types::BASE_SENDING_TYPE;
    else if (any([](auto const &s) { return s.template getFieldView<9>().has_value(); }))
      type = Types::BASE_SENT_FAILED_TYPE;
    else // NOTE: Android uses BASE_SENDING_SKIPPED_TYPE (28) if all are skipped, but this tool does not consider that outgoing
      type = Types::BASE_SENT_TYPE;
  }
  if (!sms)
    type |= Types::SECURE_MESSAGE_BIT | Types::PUSH_MESSAGE_BIT;
  if (ci.getFieldView<17>().has_value()) // gift badge
    type |= Types::SPECIAL_TYPE_GIFT_BADGE;
  if (auto storyreply = ci.getFieldView<19>(); storyreply.has_value() && storyreply->getFieldView<2>().has_value())
    type |= Types::SPECIAL_TYPE_STORY_REACTION;

  // COMMON FIELDS
  long long int date_received = date_sent;
  if (incoming.has_value())
    date_received = incoming->getFieldView<1>().value_or(date_sent);
  else if (outgoing.has_value() && outgoing->getFieldView<2>().value_or(0) != 0)
    date_received = outgoing->getFieldView<2>().value();

  long long int receipt_timestamp = 0;
  bool viewed = false;
  bool read_receipt = false;
  bool delivery_receipt = false;
  long long int unidentified = 0;
  for (auto const &s : sendstatus)
  {
    receipt_timestamp = std::max(receipt_timestamp, static_cast<long long int>(s.getFieldView<2>().value_or(0)));
    viewed |= s.getFieldView<7>().has_value();
    read_receipt |= s.getFieldView<6>().has_value();
    delivery_receipt |= s.getFieldView<5>().has_value();
    for (auto sealedsender : {s.getFieldView<4, 1>(), s.getFieldView<5, 1>(), s.getFieldView<6, 1>(), s.getFieldView<7, 1>()})
      if (sealedsender.value_or(false))
      {
        ++unidentified;
        break;
      }
  }
  read_receipt |= viewed;
  delivery_receipt |= read_receipt;

  long long int read = 1;
  if (incoming.has_value())
  {
    read = incoming->getFieldView<3>().value_or(false) ? 1 : 0;
    unidentified = incoming->getFieldView<4>().value_or(false) ? 1 : 0;
  }

  std::vector<std::pair<std::string, std::any>> columns{
    {"date_sent", static_cast<long long int>(date_sent)},
    {"date_received", date_received},
    {"date_server", incoming.has_value() ? static_cast<long long int>(incoming->getFieldView<2>().value_or(-1)) : -1},
    {"thread_id", thread_id},
    {"receipt_timestamp", receipt_timestamp},
    {"expires_in", static_cast<long long int>(ci.getFieldView<5>().value_or(0))},
    {"expire_started", static_cast<long long int>(ci.getFieldView<4>().value_or(0))},
    {"viewed", viewed ? 1 : 0},
    {"has_read_receipt", read_receipt ? 1 : 0},
    {"has_delivery_receipt", delivery_receipt ? 1 : 0},
    {"unidentified", unidentified},
    {"notified", outgoing.has_value() ? 0 : 1}};

  if (auto pindetails = ci.getFieldView<21>(); pindetails.has_value())
  {
    columns.emplace_back("pinned_at", static_cast<long long int>(pindetails->getFieldView<1>().value_or(0)));
    columns.emplace_back("pinned_until", pindetails->getFieldView<3>().value_or(false) ?
                         std::numeric_limits<long long int>::max() :
                         static_cast<long long int>(pindetails->getFieldView<2>().value_or(0)));
  }

  std::optional<std::string> body;
  std::pair<std::unique_ptr<unsigned char []>, size_t> message_ranges;
  std::pair<std::unique_ptr<unsigned char []>, size_t> quote_ranges;
  std::optional<MessageExtras> message_extras;
  std::vector<BackupV2::BodyRange> mentions;
  std::vector<BackupV2::Reaction> reactions;

  // things that need the message id, done after inserting the message
  std::function<bool(long long int)> followup;

  auto addQuote = [&](BackupV2::Quote const &quote)
  {
    auto target = quote.getFieldView<1>();
    auto quote_author_it = state->recipients.find(quote.getFieldView<2>().value_or(0));
    long long int quote_type = 0; // NORMAL
    switch (quote.getFieldView<5>().value_or(0))
    {
      case 2: quote_type = 1; break; // GIFT_BADGE
      case 4: quote_type = 2; break; // POLL
    }
    columns.emplace_back("quote_id", target.has_value() ? static_cast<long long int>(target.value()) : QUOTE_TARGET_MISSING_ID);
    columns.emplace_back("quote_author", quote_author_it != state->recipients.end() ? quote_author_it->second : 0);
    columns.emplace_back("quote_missing", target.has_value() ? 0 : 1);
    columns.emplace_back("quote_type", quote_type);
    if (auto quote_text = quote.getFieldView<3>(); quote_text.has_value())
    {
      if (auto quote_body = quote_text->getFieldView<1>(); quote_body.has_value())
        columns.emplace_back("quote_body", std::string(quote_body.value()));
      if (bv2BodyRangesToMessageRanges(quote_text->getFieldView<2>(), true, &quote_ranges))
        columns.emplace_back("quote_mentions", std::pair<unsigned char *, unsigned long long>(quote_ranges.first.get(), quote_ranges.second));
    }
  };

  auto addText = [&](std::optional<BackupV2::Text> const &text)
  {
    if (!text.has_value())
      return;
    if (auto text_body = text->getFieldView<1>(); text_body.has_value())
      body = std::string(text_body.value());
    auto ranges = text->getFieldView<2>();
    if (bv2BodyRangesToMessageRanges(ranges, false, &message_ranges))
      columns.emplace_back("message_ranges", std::pair<unsigned char *, unsigned long long>(message_ranges.first.get(), message_ranges.second));
    for (auto const &r : ranges)
      if (r.getFieldView<3>().has_value())
        mentions.push_back(r);
  };

  auto setExtras = [&](auto const &extra, auto setfield) // setfield: lambda adding 'extra' to MessageExtras
  {
    MessageExtras me;
    setfield(me, extra);
    message_extras = std::move(me);
  };

  // MESSAGE CONTENT
  if (auto standard = ci.getFieldView<11>(); standard.has_value())
  {
    if (auto quote = standard->getFieldView<1>(); quote.has_value())
      addQuote(quote.value());
    addText(standard->getFieldView<2>());
    reactions = standard->getFieldView<6>();

    followup = [this, standard, state, &jsonArray](long long int mid)
    {
      int order = 0;
      for (auto const &a : standard->getFieldView<3>())
        if (auto pointer = a.getFieldView<1>(); pointer.has_value())
          bv2InsertAttachment(pointer.value(), mid, order++, a.getFieldView<2>().value_or(0), false, std::string(), state);

      if (auto longtext = standard->getFieldView<5>(); longtext.has_value())
        bv2InsertAttachment(longtext.value(), mid, order++, 0, false, "text/x-signal-plain", state);

      if (auto quote = standard->getFieldView<1>(); quote.has_value())
      {
        if (quote->getFieldView<5>().value_or(0) == 3) // VIEW_ONCE -> tombstone
          return true;
        for (auto const &qa : quote->getFieldView<4>())
        {
          std::string quoted_ct(qa.getFieldView<1>().value_or(std::string_view()));
          long long int aid = -1;
          if (auto thumbnail = qa.getFieldView<3, 1>(); thumbnail.has_value())
            aid = bv2InsertAttachment(thumbnail.value(), mid, 0, 0, true, std::string(), state);
          else if (!quoted_ct.empty())
            aid = bv2InsertAttachment(BackupV2::FilePointer(), mid, 0, 0, true, quoted_ct, state);
          if (aid != -1)
            d_database.exec("UPDATE attachment SET quote_target_content_type = ?, file_name = COALESCE(file_name, ?) WHERE _id = ?",
                            {quoted_ct.empty() ? std::any(nullptr) : quoted_ct,
                             qa.getFieldView<2>().has_value() ? std::any(std::string(qa.getFieldView<2>().value())) : std::any(nullptr),
                             aid});
        }
      }

      std::vector<std::string> previews;
      for (auto const &lp : standard->getFieldView<4>())
      {
        long long int aid = -1;
        if (auto image = lp.getFieldView<3>(); image.has_value())
          aid = bv2InsertAttachment(image.value(), mid, order++, 0, false, std::string(), state);
        previews.emplace_back(d_database.getSingleResultAs<std::string>
                              ("SELECT json_object('url', ?1, 'title', ?2, 'description', ?3, 'date', ?4, "
                               "'attachmentId', CASE WHEN ?5 = -1 THEN NULL ELSE json_object('rowId', ?5, 'uniqueId', -1, 'valid', json('true')) END)",
                               {std::string(lp.getFieldView<1>().value_or(std::string_view())),
                                std::string(lp.getFieldView<2>().value_or(std::string_view())),
                                std::string(lp.getFieldView<4>().value_or(std::string_view())),
                                static_cast<long long int>(lp.getFieldView<5>().value_or(0)),
                                aid}, std::string()));
      }
      if (!previews.empty())
        d_database.exec("UPDATE message SET link_previews = ? WHERE _id = ?", {jsonArray(previews), mid});
      return true;
    };
  }
  else if (auto contact = ci.getFieldView<12>(); contact.has_value())
  {
    reactions = contact->getFieldView<2>();
    if (auto ca = contact->getFieldView<1>(); ca.has_value())
    {
      // see dtsetsharedcontactsjsonstring.cc for the json format
      auto str = [](std::optional<std::string_view> const &s) STATICLAMBDA
      { return s.has_value() ? std::any(std::string(s.value())) : std::any(nullptr); };

      std::array<char const *, 5> constexpr phonetypes{"HOME", "HOME", "MOBILE", "WORK", "CUSTOM"};
      std::array<char const *, 4> constexpr addresstypes{"HOME", "HOME", "WORK", "CUSTOM"};

      std::vector<std::string> phones;
      for (auto const &p : ca->getFieldView<3>())
        phones.emplace_back(d_database.getSingleResultAs<std::string>("SELECT json_object('number', ?, 'type', ?, 'label', ?)",
                                                                      {str(p.getFieldView<1>()), std::string(phonetypes[std::clamp<int64_t>(p.getFieldView<2>().value_or(0), 0, 4)]), str(p.getFieldView<3>())},
                                                                      std::string()));
      std::vector<std::string> emails;
      for (auto const &e : ca->getFieldView<4>())
        emails.emplace_back(d_database.getSingleResultAs<std::string>("SELECT json_object('email', ?, 'type', ?, 'label', ?)",
                                                                      {str(e.getFieldView<1>()), std::string(phonetypes[std::clamp<int64_t>(e.getFieldView<2>().value_or(0), 0, 4)]), str(e.getFieldView<3>())},
                                                                      std::string()));
      std::vector<std::string> addresses;
      for (auto const &a : ca->getFieldView<5>())
        addresses.emplace_back(d_database.getSingleResultAs<std::string>("SELECT json_object('type', ?, 'label', ?, 'street', ?, 'poBox', ?, 'neighborhood', ?, "
                                                                         "'city', ?, 'region', ?, 'postalCode', ?, 'country', ?)",
                                                                         {std::string(addresstypes[std::clamp<int64_t>(a.getFieldView<1>().value_or(0), 0, 3)]),
                                                                          str(a.getFieldView<2>()), str(a.getFieldView<3>()), str(a.getFieldView<4>()),
                                                                          str(a.getFieldView<5>()), str(a.getFieldView<6>()), str(a.getFieldView<7>()),
                                                                          str(a.getFieldView<8>()), str(a.getFieldView<9>())},
                                                                         std::string()));

      auto name = ca->getFieldView<1>();
      std::string displayname;
      if (name.has_value())
      {
        for (auto part : {name->getFieldView<3>(), name->getFieldView<1>(), name->getFieldView<5>(), name->getFieldView<2>(), name->getFieldView<4>()})
          if (part.has_value() && !part->empty())
            displayname += (displayname.empty() ? "" : " ") + std::string(part.value());
        if (displayname.empty() && name->getFieldView<6>().has_value())
          displayname = name->getFieldView<6>().value();
      }

      std::string shared_contacts =
        d_database.getSingleResultAs<std::string>("SELECT json_array(json_object("
                                                  "'name', json_object('displayName', ?1, 'givenName', ?2, 'familyName', ?3, 'prefix', ?4, 'suffix', ?5, 'middleName', ?6, 'nickname', ?7), "
                                                  "'organization', ?8, 'phoneNumbers', json(?9), 'emails', json(?10), 'postalAddresses', json(?11), "
                                                  "'avatar', json_object('attachmentId', NULL, 'isProfile', json('false'))))",
                                                  {displayname,
                                                   name.has_value() ? str(name->getFieldView<1>()) : std::any(nullptr),
                                                   name.has_value() ? str(name->getFieldView<2>()) : std::any(nullptr),
                                                   name.has_value() ? str(name->getFieldView<3>()) : std::any(nullptr),
                                                   name.has_value() ? str(name->getFieldView<4>()) : std::any(nullptr),
                                                   name.has_value() ? str(name->getFieldView<5>()) : std::any(nullptr),
                                                   name.has_value() ? str(name->getFieldView<6>()) : std::any(nullptr),
                                                   str(ca->getFieldView<7>()),
                                                   jsonArray(phones),
                                                   jsonArray(emails),
                                                   jsonArray(addresses)},
                                                  std::string());
      columns.emplace_back("shared_contacts", shared_contacts);
    }
  }
  else if (auto sticker_message = ci.getFieldView<13>(); sticker_message.has_value())
  {
    reactions = sticker_message->getFieldView<2>();
    followup = [this, sticker_message, state](long long int mid)
    {
      auto sticker = sticker_message->getFieldView<1>();
      if (!sticker.has_value())
        return true;
      auto data = sticker->getFieldView<5>();
      long long int aid = bv2InsertAttachment(data.value_or(BackupV2::FilePointer()), mid, 0, 0, false,
                                              data.has_value() && data->getFieldView<4>().has_value() ? "" : "image/webp", state);
      if (aid == -1)
        return true;
      auto packid = sticker->getFieldView<1>();
      auto packkey = sticker->getFieldView<2>();
      d_database.exec("UPDATE attachment SET sticker_pack_id = ?, sticker_pack_key = ?, sticker_id = ?, sticker_emoji = ? WHERE _id = ?",
                      {packid.has_value() ? bepaald::bytesToHexString(packid->first, packid->second, true) : std::string(),
                       packkey.has_value() ? bepaald::bytesToHexString(packkey->first, packkey->second, true) : std::string(),
                       static_cast<long long int>(sticker->getFieldView<3>().value_or(0)),
                       sticker->getFieldView<4>().has_value() ? std::any(std::string(sticker->getFieldView<4>().value())) : std::any(nullptr),
                       aid});
      return true;
    };
  }
  else if (ci.getFieldView<14>().has_value()) // remote deleted
  {
    columns.emplace_back("deleted_by", from_rid);
  }
  else if (auto update = ci.getFieldView<15>(); update.has_value())
  {
    uint64_t type_without_base = type & ~Types::BASE_TYPE_MASK;

    if (auto simple = update->getFieldView<1>(); simple.has_value())
    {
      switch (simple->getFieldView<1>().value_or(0))
      {
        case 1: type = Types::JOINED_TYPE | type_without_base; break;
        case 2: type = Types::KEY_EXCHANGE_IDENTITY_UPDATE_BIT | type_without_base; break;
        case 3: // identity verified/default have to/from swapped
          type = Types::KEY_EXCHANGE_IDENTITY_VERIFIED_BIT | type_without_base | Types::BASE_SENT_TYPE;
          std::swap(from_rid, to_rid);
          break;
        case 4:
          type = Types::KEY_EXCHANGE_IDENTITY_DEFAULT_BIT | type_without_base | Types::BASE_SENT_TYPE;
          std::swap(from_rid, to_rid);
          break;
        case 5: type = Types::CHANGE_NUMBER_TYPE; break;
        case 6: type = Types::BOOST_REQUEST_TYPE; break; // RELEASE_CHANNEL_DONATION_REQUEST_TYPE
        case 7: type = Types::END_SESSION_BIT | type_without_base; break;
        case 8: type = ENCRYPTION_REMOTE_FAILED_BIT | type_without_base; break;
        case 9: type = Types::BAD_DECRYPT_TYPE | type_without_base; break;
        case 10: type = Types::SPECIAL_TYPE_PAYMENTS_ACTIVATED | type_without_base; break;
        case 11: type = Types::SPECIAL_TYPE_PAYMENTS_ACTIVATE_REQUEST | type_without_base; break;
        case 12: type = Types::UNSUPPORTED_MESSAGE_TYPE | type_without_base; break;
        case 13: type = Types::SPECIAL_TYPE_REPORTED_SPAM | type_without_base; break;
        case 14: type = SPECIAL_TYPE_BLOCKED | type_without_base; break;
        case 15: type = SPECIAL_TYPE_UNBLOCKED | type_without_base; break;
        case 16: // directionless 1:1 message requests expect to recipient to be the other recipient not self
          type = Types::SPECIAL_TYPE_MESSAGE_REQUEST_ACCEPTED | type_without_base;
          to_rid = chat_rid;
          break;
        default: type = type_without_base; break;
      }
    }
    else if (auto groupchange = update->getFieldView<2>(); groupchange.has_value())
    {
      // Android stores the backup's GroupChangeChatUpdate as-is (in MessageExtras.gv2UpdateDescription.groupChangeUpdate)
      body = std::string();
      setExtras(::GroupChangeChatUpdate(groupchange->data(), groupchange->size()), [](MessageExtras &me, auto const &gccu)
      {
        GV2UpdateDescription gv2;
        gv2.addField<2>(gccu);
        me.addField<1>(gv2);
      });
      type |= Types::GROUP_V2_BIT | Types::GROUP_UPDATE_BIT;
    }
    else if (auto expiration = update->getFieldView<3>(); expiration.has_value())
    {
      type |= Types::EXPIRATION_TIMER_UPDATE_BIT;
      columns.emplace_back("expires_in", static_cast<long long int>(expiration->getFieldView<1>().value_or(0)));
    }
    else if (auto profilechange = update->getFieldView<4>(); profilechange.has_value())
    {
      type = Types::PROFILE_CHANGE_TYPE;
      StringChange sc;
      sc.addField<1>(std::string(profilechange->getFieldView<1>().value_or(std::string_view())));
      sc.addField<2>(std::string(profilechange->getFieldView<2>().value_or(std::string_view())));
      setExtras(sc, [](MessageExtras &me, StringChange const &change)
      {
        ProfileChangeDetails pcd;
        pcd.addField<1>(change);
        me.addField<3>(pcd);
      });
    }
    else if (auto threadmerge = update->getFieldView<5>(); threadmerge.has_value())
    {
      type = Types::THREAD_MERGE_TYPE | type_without_base;
      ProtoBufParser<protobuffer::optional::STRING> threadmergeevent; // ThreadMergeEvent { string previousE164 = 1; }
      threadmergeevent.addField<1>(bepaald::toString(threadmerge->getFieldView<1>().value_or(0)));
      body = threadmergeevent.getDataString();
    }
    else if (auto switchover = update->getFieldView<6>(); switchover.has_value())
    {
      type = SESSION_SWITCHOVER_TYPE | type_without_base;
      ProtoBufParser<protobuffer::optional::STRING> switchoverevent; // SessionSwitchoverEvent { string e164 = 1; }
      switchoverevent.addField<1>(bepaald::toString(switchover->getFieldView<1>().value_or(0)));
      body = switchoverevent.getDataString();
    }
    else if (auto individualcall = update->getFieldView<7>(); individualcall.has_value())
    {
      bool video = individualcall->getFieldView<2>().value_or(0) == 2;
      int64_t callstate = individualcall->getFieldView<4>().value_or(0);
      if (callstate == 3 || callstate == 4) // MISSED, MISSED_NOTIFICATION_PROFILE
        type = video ? Types::MISSED_VIDEO_CALL_TYPE : Types::MISSED_AUDIO_CALL_TYPE;
      else if (individualcall->getFieldView<3>().value_or(0) == 2) // OUTGOING
        type = video ? Types::OUTGOING_VIDEO_CALL_TYPE : Types::OUTGOING_AUDIO_CALL_TYPE;
      else
        type = video ? Types::INCOMING_VIDEO_CALL_TYPE : Types::INCOMING_AUDIO_CALL_TYPE;
      read = 1;
    }
    else if (auto groupcall = update->getFieldView<8>(); groupcall.has_value())
    {
      // see handledtcalltypemessage.cc
      ProtoBufParser<protobuffer::optional::STRING, protobuffer::optional::STRING, protobuffer::optional::INT64, protobuffer::repeated::STRING, protobuffer::optional::BOOL> groupcallbody;
      if (auto callid = groupcall->getFieldView<1>(); callid.has_value())
        groupcallbody.addField<1>(bepaald::toString(callid.value()));
      if (auto starter = groupcall->getFieldView<4>(); starter.has_value() && bepaald::contains(state->recipients, starter.value()))
      {
        std::string starter_aci = d_database.getSingleResultAs<std::string>("SELECT aci FROM recipient WHERE _id = ?", state->recipients[starter.value()], std::string());
        if (!starter_aci.empty())
          groupcallbody.addField<2>(starter_aci);
      }
      groupcallbody.addField<3>(static_cast<int64_t>(groupcall->getFieldView<5>().value_or(date_sent)));
      body = groupcallbody.getDataString();
      read = groupcall->getFieldView<7>().value_or(false) ? 1 : 0;
      type = Types::GROUP_CALL_TYPE;
    }
    else if (auto learnedprofile = update->getFieldView<9>(); learnedprofile.has_value())
    {
      type = Types::PROFILE_CHANGE_TYPE;
      LearnedProfileName lpn;
      if (auto e164 = learnedprofile->getFieldView<1>(); e164.has_value())
        lpn.addField<1>(bepaald::toString(e164.value()));
      else if (auto username = learnedprofile->getFieldView<2>(); username.has_value())
        lpn.addField<2>(std::string(username.value()));
      setExtras(lpn, [](MessageExtras &me, LearnedProfileName const &learned)
      {
        ProfileChangeDetails pcd;
        pcd.addField<3>(learned);
        me.addField<3>(pcd);
      });
    }
    else if (auto pollterminate = update->getFieldView<10>(); pollterminate.has_value())
    {
      type = Types::SPECIAL_TYPE_POLL_TERMINATE | type_without_base;
      uint64_t target = pollterminate->getFieldView<1>().value_or(0);
      std::string question(pollterminate->getFieldView<2>().value_or(std::string_view()));
      followup = [this, target, question, from_rid](long long int mid)
      {
        long long int poll_mid = d_database.getSingleResultAs<long long int>("SELECT _id FROM message WHERE date_sent = ? AND from_recipient_id = ?",
                                                                             {static_cast<long long int>(target), from_rid}, -1);
        PollTerminate pt;
        pt.addField<1>(question);
        pt.addField<2>(static_cast<uint64_t>(poll_mid));
        pt.addField<3>(target);
        MessageExtras me;
        me.addField<5>(pt);
        d_database.exec("UPDATE message SET message_extras = ? WHERE _id = ?",
                        {std::pair<unsigned char *, unsigned long long>(me.data(), me.size()), mid});
        if (poll_mid != -1)
          d_database.exec("UPDATE poll SET end_message_id = ? WHERE message_id = ?", {mid, poll_mid});
        return true;
      };
    }
    else if (auto pinmessage = update->getFieldView<11>(); pinmessage.has_value())
    {
      type = Types::SPECIAL_TYPE_PINNED_MESSAGE | type_without_base;
      uint64_t target = pinmessage->getFieldView<1>().value_or(0);
      auto target_author_it = state->recipients.find(pinmessage->getFieldView<2>().value_or(0));
      long long int target_author = target_author_it != state->recipients.end() ? target_author_it->second : -1;
      followup = [this, target, target_author](long long int mid)
      {
        std::string target_aci = d_database.getSingleResultAs<std::string>("SELECT aci FROM recipient WHERE _id = ?", target_author, std::string());
        if (target_aci.empty())
          return true;
        long long int pinned_mid = d_database.getSingleResultAs<long long int>("SELECT _id FROM message WHERE date_sent = ? AND from_recipient_id = ?",
                                                                               {static_cast<long long int>(target), target_author}, -1);
        unsigned char rawaci[16];
        std::string aci_hex(target_aci);
        std::erase(aci_hex, '-');
        if (!bepaald::hexStringToBytes(aci_hex, rawaci, 16))
          return true;
        PinnedMessage pm;
        pm.addField<1>(static_cast<uint64_t>(pinned_mid));
        pm.addField<2>({rawaci, 16});
        pm.addField<3>(target);
        MessageExtras me;
        me.addField<6>(pm);
        d_database.exec("UPDATE message SET message_extras = ? WHERE _id = ?",
                        {std::pair<unsigned char *, unsigned long long>(me.data(), me.size()), mid});
        if (pinned_mid != -1)
          d_database.exec("UPDATE message SET pinning_message_id = ? WHERE _id = ?", {mid, pinned_mid});
        return true;
      };
    }
    else
    {
      Logger::warning("Unsupported update message type. Skipping...");
      ++state->skippedchatitems;
      return true;
    }
  }
  else if (auto payment = ci.getFieldView<16>(); payment.has_value())
  {
    // Payments can not be restored from the ledger here, Android falls back to a 'tombstone'
    type |= Types::SPECIAL_TYPE_PAYMENTS_TOMBSTONE;
    PaymentTombstone pt;
    if (auto note = payment->getFieldView<3>(); note.has_value())
      pt.addField<1>(std::string(note.value()));
    setExtras(pt, [](MessageExtras &me, PaymentTombstone const &tombstone) { me.addField<4>(tombstone); });
  }
  else if (auto giftbadge = ci.getFieldView<17>(); giftbadge.has_value())
  {
    // the Android GiftBadge (redemptionToken = 1, redemptionState = 2) has the same layout
    // (and enum values) as the backup's GiftBadge
    body = giftbadge->getDataString();
  }
  else if (auto viewonce = ci.getFieldView<18>(); viewonce.has_value())
  {
    reactions = viewonce->getFieldView<2>();
    columns.emplace_back("view_once", 1);
    followup = [this, viewonce, state](long long int mid)
    {
      if (auto pointer = viewonce->getFieldView<1, 1>(); pointer.has_value())
        bv2InsertAttachment(pointer.value(), mid, 0, viewonce->getFieldView<1, 2>().value_or(0), false, std::string(), state);
      return true;
    };
  }
  else if (auto storyreply = ci.getFieldView<19>(); storyreply.has_value())
  {
    reactions = storyreply->getFieldView<3>();
    columns.emplace_back("parent_story_id", PARENT_STORY_MISSING_ID);
    columns.emplace_back("quote_missing", 1);
    columns.emplace_back("quote_id", QUOTE_TARGET_MISSING_ID);
    columns.emplace_back("quote_author", to_rid);
    if (auto emoji = storyreply->getFieldView<2>(); emoji.has_value())
      body = std::string(emoji.value());
    else if (auto textreply = storyreply->getFieldView<1>(); textreply.has_value())
    {
      addText(textreply->getFieldView<1>());
      followup = [this, storyreply, state](long long int mid)
      {
        if (auto longtext = storyreply->getFieldView<1, 2>(); longtext.has_value())
          bv2InsertAttachment(longtext.value(), mid, 0, 0, false, "text/x-signal-plain", state);
        return true;
      };
    }
  }
  else if (auto poll = ci.getFieldView<20>(); poll.has_value())
  {
    reactions = poll->getFieldView<5>();
    body = std::string(poll->getFieldView<1>().value_or(std::string_view()));
    followup = [this, poll, from_rid, state](long long int mid)
    {
      std::any poll_id;
      if (!insertRow("poll",
                     {{"author_id", from_rid},
                      {"message_id", mid},
                      {"question", std::string(poll->getFieldView<1>().value_or(std::string_view()))},
                      {"allow_multiple_votes", poll->getFieldView<2>().value_or(false) ? 1 : 0},
                      {"end_message_id", poll->getFieldView<4>().value_or(false) ? -1 : 0}}, // set to actual message when PollTerminate is processed
                     "_id", &poll_id)) [[unlikely]]
        return false;

      long long int option_order = 0;
      for (auto const &option : poll->getFieldView<3>())
      {
        std::any option_id;
        if (!insertRow("poll_option",
                       {{"poll_id", std::any_cast<long long int>(poll_id)},
                        {"option_text", std::string(option.getFieldView<1>().value_or(std::string_view()))},
                        {"option_order", option_order++}},
                       "_id", &option_id)) [[unlikely]]
          return false;

        for (auto const &vote : option.getFieldView<2>())
        {
          auto voter = state->recipients.find(vote.getFieldView<1>().value_or(0));
          if (voter == state->recipients.end())
            continue;
          insertRow("poll_vote",
                    {{"poll_id", std::any_cast<long long int>(poll_id)},
                     {"poll_option_id", std::any_cast<long long int>(option_id)},
                     {"voter_id", voter->second},
                     {"vote_count", static_cast<long long int>(vote.getFieldView<2>().value_or(0))},
                     {"vote_state", 4}}); // ADDED
        }
      }
      return true;
    };
  }
  else if (auto admindeleted = ci.getFieldView<22>(); admindeleted.has_value())
  {
    auto admin = state->recipients.find(admindeleted->getFieldView<1>().value_or(0));
    if (admin == state->recipients.end())
    {
      Logger::warning("Admin for deleted message not found. Skipping...");
      ++state->skippedchatitems;
      return true;
    }
    columns.emplace_back("deleted_by", admin->second);
  }
  else
  {
    Logger::warning("Unsupported message type. Skipping...");
    ++state->skippedchatitems;
    return true;
  }

  columns.emplace_back("type", static_cast<long long int>(type));
  columns.emplace_back("from_recipient_id", from_rid);
  columns.emplace_back("to_recipient_id", to_rid);
  columns.emplace_back("read", read);
  if (body.has_value())
    columns.emplace_back("body", body.value());
  if (message_extras.has_value())
    columns.emplace_back("message_extras", toBlob(message_extras.value()));
  for (auto const &m : mentions) // mentions_self
    if (bv2GetRecipientIdForAci(bv2UuidFromBytes(m.getFieldView<3>().value()), false) == d_selfid)
    {
      columns.emplace_back("mentions_self", 1);
      break;
    }

  std::any retval;
  if (!insertRow("message", columns, "_id", &retval)) [[unlikely]]
  {
    Logger::error("Failed to insert message");
    return false;
  }
  long long int mid = std::any_cast<long long int>(retval);
  *new_mid = mid;

  // REACTIONS
  for (auto const &r : reactions)
  {
    auto reaction_author = state->recipients.find(r.getFieldView<2>().value_or(0));
    if (reaction_author == state->recipients.end())
    {
      Logger::warning("Author of reaction not found. Skipping...");
      continue;
    }
    if (!insertRow("reaction",
                   {{"message_id", mid},
                    {"author_id", reaction_author->second},
                    {"emoji", std::string(r.getFieldView<1>().value_or(std::string_view()))},
                    {"date_sent", static_cast<long long int>(r.getFieldView<3>().value_or(0))},
                    {"date_received", static_cast<long long int>(r.getFieldView<4>().value_or(0))}})) [[unlikely]]
      Logger::warning("Failed to insert reaction");
  }

  // MENTIONS
  for (auto const &m : mentions)
  {
    long long int mention_rid = bv2GetRecipientIdForAci(bv2UuidFromBytes(m.getFieldView<3>().value()), true);
    if (mention_rid == -1) [[unlikely]]
      continue;
    if (!insertRow("mention",
                   {{"thread_id", thread_id},
                    {"message_id", mid},
                    {"recipient_id", mention_rid},
                    {"range_start", static_cast<long long int>(m.getFieldView<1>().value_or(0))},
                    {"range_length", static_cast<long long int>(m.getFieldView<2>().value_or(0))}})) [[unlikely]]
      Logger::warning("Failed to insert mention");
  }

  // GROUP RECEIPTS (only for group chats, a 1-on-1 chat has a single status for the chat recipient)
  if (!(sendstatus.size() == 1 && sendstatus[0].getFieldView<1>().value_or(0) == chat_backup_rid))
  {
    for (auto const &s : sendstatus)
    {
      auto receipt_rid = state->recipients.find(s.getFieldView<1>().value_or(0));
      if (receipt_rid == state->recipients.end())
        continue;
      long long int status = -1; // UNKNOWN (also pending)
      if (s.getFieldView<4>().has_value())
        status = 0;   // UNDELIVERED
      else if (s.getFieldView<5>().has_value())
        status = 1;   // DELIVERED
      else if (s.getFieldView<6>().has_value())
        status = 2;   // READ
      else if (s.getFieldView<7>().has_value())
        status = 3;   // VIEWED
      else if (s.getFieldView<8>().has_value())
        status = 4;   // SKIPPED
      else if (s.getFieldView<9>().has_value())
        status = 5;   // FAILED
      bool sealedsender = false;
      for (auto ss : {s.getFieldView<4, 1>(), s.getFieldView<5, 1>(), s.getFieldView<6, 1>(), s.getFieldView<7, 1>()})
        sealedsender |= ss.value_or(false);
      insertRow("group_receipts",
                {{"mms_id", mid},
                 {"address", receipt_rid->second},
                 {"status", status},
                 {"timestamp", static_cast<long long int>(s.getFieldView<2>().value_or(0))},
                 {"unidentified", sealedsender ? 1 : 0}});
    }
  }

  if (followup && !followup(mid)) [[unlikely]]
    return false;

  return true;
}

#endif
