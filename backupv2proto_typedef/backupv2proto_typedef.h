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

#ifndef BACKUPV2PROTO_TYPEDEF_H_
#define BACKUPV2PROTO_TYPEDEF_H_

#include <vector>

// forward declare protobufparser
struct ZigZag32;
struct ZigZag64;
struct Fixed32;
struct Fixed64;
struct SFixed32;
struct SFixed64;
struct Enum;
struct Dummy;
namespace protobuffer
{
  typedef Dummy DUMMY;
  namespace optional
  {
    typedef double DOUBLE;
    typedef float FLOAT;
    typedef int32_t INT32;
    typedef Enum ENUM;
    typedef int64_t INT64;
    typedef uint32_t UINT32;
    typedef uint64_t UINT64;
    typedef ZigZag32 SINT32;
    typedef ZigZag64 SINT64;
    typedef Fixed32 FIXED32;
    typedef Fixed64 FIXED64;
    typedef SFixed32 SFIXED32;
    typedef SFixed64 SFIXED64;
    typedef bool BOOL;
    typedef std::string STRING;
    typedef unsigned char *BYTES;
  }
  namespace repeated
  {
    typedef std::vector<double> DOUBLE;
    typedef std::vector<float> FLOAT;
    typedef std::vector<int32_t> INT32;
    typedef std::vector<Enum> ENUM;
    typedef std::vector<int64_t> INT64;
    typedef std::vector<uint32_t> UINT32;
    typedef std::vector<uint64_t> UINT64;
    typedef std::vector<ZigZag32> SINT32;
    typedef std::vector<ZigZag64> SINT64;
    typedef std::vector<Fixed32> FIXED32;
    typedef std::vector<Fixed64> FIXED64;
    typedef std::vector<SFixed32> SFIXED32;
    typedef std::vector<SFixed64> SFIXED64;
    typedef std::vector<bool> BOOL;
    typedef std::vector<std::string> STRING;
    typedef std::vector<unsigned char *> BYTES;
  }
}
template <typename... Spec>
class ProtoBufParser;
//#include "../protobufparser/protobufparser.h"

// https://github.com/signalapp/Signal-Android/blob/main/lib/archive/src/main/protowire/Backup.proto

namespace BackupV2
{

/*
message Metadata {
  message EncryptedBackupId {
    bytes iv = 1; // 12 bytes, randomly generated
    bytes encryptedId = 2; // AES-256-CTR, key = local backup metadata key, message = backup ID bytes
    // local backup metadata key = hkdf(input: K_B, info: UTF8("20241011_SIGNAL_LOCAL_BACKUP_METADATA_KEY"), length: 32)
    // No hash of the ID; if it's decrypted incorrectly, the main backup will fail to decrypt anyway.
  }

  uint32 version = 1;
  EncryptedBackupId backupId = 2; // used to decrypt the backup file knowing only the Account Entropy Pool
}
*/
typedef ProtoBufParser<protobuffer::optional::BYTES, protobuffer::optional::BYTES> EncryptedBackupId;
typedef ProtoBufParser<protobuffer::optional::UINT32, EncryptedBackupId> Metadata;

typedef ProtoBufParser<protobuffer::optional::UINT64,
                       protobuffer::optional::UINT64,
                       protobuffer::optional::BYTES,
                       protobuffer::optional::STRING,
                       protobuffer::optional::STRING,
                       protobuffer::optional::BYTES> BackupInfo;

typedef ProtoBufParser<protobuffer::optional::BYTES, // id
                       protobuffer::optional::STRING, // currency
                       protobuffer::optional::BOOL // manually cancelled
                       > SubscriberData;

typedef ProtoBufParser<protobuffer::optional::BYTES, // id
                       protobuffer::optional::STRING, // purchasetoken  \ oneof
                       protobuffer::optional::UINT64 // transaction id  /
                       > IAPSubscriberData;

typedef ProtoBufParser<protobuffer::optional::BYTES, // key
                       protobuffer::DUMMY, // dummy
                       protobuffer::optional::UINT32, // size (plaintext)
                       protobuffer::optional::STRING, // cdn key
                       protobuffer::optional::UINT32, // transit cdn number
                       protobuffer::optional::UINT64, // uploadtimestamp
                       protobuffer::optional::UINT32, // media tier cdn number
                       protobuffer::DUMMY, // dummy
                       protobuffer::optional::BYTES, // localkey
                       protobuffer::optional::BYTES, // plaintexthash   \ onof
                       protobuffer::optional::BYTES  // encrypteddigest /
                       > LocatorInfo;
typedef ProtoBufParser<protobuffer::DUMMY, // dummy
                       protobuffer::DUMMY, // dummy
                       protobuffer::DUMMY, // dummy
                       protobuffer::optional::STRING, // ct
                       protobuffer::optional::BYTES, // mac
                       protobuffer::optional::UINT32, // mac chunksize
                       protobuffer::optional::STRING, // filename
                       protobuffer::optional::UINT32, // w
                       protobuffer::optional::UINT32, // h
                       protobuffer::optional::STRING, // caption
                       protobuffer::optional::STRING, // blurhash
                       protobuffer::DUMMY, // dummy
                       LocatorInfo
                       > FilePointer;

typedef ProtoBufParser<protobuffer::optional::UINT32, // angle
                       protobuffer::repeated::FIXED32, // colors, AARRGGBB
                       protobuffer::repeated::FLOAT // positions, [0-1]
                       > Gradient;

typedef ProtoBufParser<protobuffer::optional::UINT64, // id
                       protobuffer::optional::FIXED32, // solid   \ oneof
                       Gradient //                                /
                       > CustomChatColor;

typedef ProtoBufParser<protobuffer::optional::ENUM, // WallpaperPreset  \ onof
                       FilePointer,     //                              /
                       ProtoBufParser<>, // AutomaticBubbleColor          \ oneof
                       protobuffer::optional::ENUM, // bubbleColorPreset  /
                       protobuffer::optional::UINT64, // customColorid (in Accountsettings)
                       protobuffer::DUMMY, // dummy -> NOTE THIS ONE DOES NOT APPEAR IN proto, IT JUST SKIPS FROM 5 TO 7
                       protobuffer::optional::BOOL // dim wallpaper in dark mode
                       > ChatStyle;

typedef ProtoBufParser<protobuffer::optional::BOOL, // read receipts
                       protobuffer::optional::BOOL, // sealed sender indicator
                       protobuffer::optional::BOOL, // typing indicator
                       protobuffer::optional::BOOL, // link previews
                       protobuffer::optional::BOOL, // not discoverable by phone
                       protobuffer::optional::BOOL, // prefer contact avatars
                       protobuffer::optional::UINT32, // universal expire
                       protobuffer::repeated::STRING, // preferred reaction emoji
                       protobuffer::optional::BOOL, // display badges
                       protobuffer::optional::BOOL, // keep muted chats archived
                       protobuffer::optional::BOOL, // has set my story provicy
                       protobuffer::optional::BOOL, // has viewed onboarding story
                       protobuffer::optional::BOOL, // stories disabled
                       protobuffer::optional::BOOL, // story view receipts
                       protobuffer::optional::BOOL, // has seen group story education sheet
                       protobuffer::optional::BOOL, // has completed username onboarding
                       protobuffer::optional::ENUM, // PhoneNumberSharingMode
                       ChatStyle,
                       CustomChatColor,
                       protobuffer::optional::BOOL, // optimize ondevice storage
                       protobuffer::optional::UINT64, // backup tier
                       protobuffer::DUMMY, // dummy
                       protobuffer::optional::ENUM, // default sent media quality
                       ProtoBufParser<protobuffer::optional::ENUM, protobuffer::optional::ENUM, protobuffer::optional::ENUM, protobuffer::optional::ENUM>, // auto downloadsettings
                       protobuffer::DUMMY, // dummy
                       protobuffer::optional::UINT32, // screenlock timeout
                       protobuffer::optional::BOOL, // pin reminders
                       protobuffer::optional::ENUM, // app theme
                       protobuffer::optional::ENUM, // calls use less data
                       protobuffer::optional::BOOL, // allow sealed sender from anyone
                       protobuffer::optional::BOOL, // allow automatic key verification
                       protobuffer::optional::BOOL // has seen admin delete education dialog
                       > AccountSettings;

typedef ProtoBufParser<protobuffer::optional::BYTES, // entropy
                       protobuffer::optional::BYTES, // serverid
                       protobuffer::optional::ENUM  // color
                       > UsernameLink;

typedef ProtoBufParser<protobuffer::optional::BYTES, // profile key
                       protobuffer::optional::STRING, // username
                       UsernameLink,
                       protobuffer::optional::STRING, // given name
                       protobuffer::optional::STRING, // family name
                       protobuffer::optional::STRING, // avatar url path
                       SubscriberData,
                       protobuffer::DUMMY, // dummy
                       AccountSettings,
                       IAPSubscriberData,
                       protobuffer::optional::STRING, // svr pin
                       // ACTUALLY AndroidSpecificSettings:
                       protobuffer::optional::BYTES, // AndroidSpecificSettings
                       protobuffer::optional::STRING, // bio text
                       protobuffer::optional::STRING, // bio emoji
                       protobuffer::DUMMY // dummy
                       > AndroidSpecificSettings;

typedef ProtoBufParser<protobuffer::optional::BYTES,
                       protobuffer::optional::STRING,
                       UsernameLink,
                       protobuffer::optional::STRING,
                       protobuffer::optional::STRING,
                       protobuffer::optional::STRING,
                       SubscriberData,
                       protobuffer::DUMMY, // actual dummy
                       AccountSettings,
                       IAPSubscriberData,
                       protobuffer::optional::STRING,
                       AndroidSpecificSettings,
                       protobuffer::optional::STRING,
                       protobuffer::optional::STRING,
                       protobuffer::DUMMY // actual dummy
                       > AccountData;

typedef ProtoBufParser<protobuffer::optional::ENUM> Self; // avatarcolor

typedef ProtoBufParser<protobuffer::optional::BYTES, // aci
                       protobuffer::optional::BYTES, // pni
                       protobuffer::optional::STRING, // username
                       protobuffer::optional::UINT64, // e164
                       protobuffer::optional::BOOL, // blocked
                       protobuffer::optional::ENUM, // visibility
                       ProtoBufParser<>, // registered                                               \ oneof
                       ProtoBufParser<protobuffer::optional::UINT64>, // not registered (timestamp)  /
                       protobuffer::optional::BYTES, // profilekey
                       protobuffer::optional::BOOL, // profilesharing
                       protobuffer::optional::STRING, // profile given name
                       protobuffer::optional::STRING, // profile family name
                       protobuffer::optional::BOOL, // hide story
                       protobuffer::optional::BYTES, // identity key
                       protobuffer::optional::ENUM, // identity state
                       ProtoBufParser<protobuffer::optional::STRING, protobuffer::optional::STRING>, // nickname (given, family)
                       protobuffer::optional::STRING, // note
                       protobuffer::optional::STRING, // system given name
                       protobuffer::optional::STRING, // system family name
                       protobuffer::optional::STRING, // system nickname
                       protobuffer::optional::ENUM, // AvatarColor
                       protobuffer::optional::BYTES // key transparency data
                       > Contact;

typedef ProtoBufParser<protobuffer::optional::BYTES, // id
                       protobuffer::optional::ENUM, // role
                       protobuffer::DUMMY, // actual dummy
                       protobuffer::DUMMY, // actual dummy
                       protobuffer::optional::UINT32, // joined at revision
                       protobuffer::optional::STRING, // label emoji
                       protobuffer::optional::STRING  // label string
                       > Member;

typedef ProtoBufParser<Member,
                       protobuffer::optional::BYTES, // added by
                       protobuffer::optional::UINT64 // timestamp
                       > MemberPendingProfileKey;

typedef ProtoBufParser<protobuffer::optional::BYTES, // id
                       protobuffer::DUMMY, // actual dummy
                       protobuffer::DUMMY, // actual dummy
                       protobuffer::optional::UINT64 // timestamp
                       > MemberPendingAdminApproval;

typedef ProtoBufParser<protobuffer::optional::BYTES, // id
                       protobuffer::optional::UINT64 // timestamp
                       > MemberBanned;

typedef ProtoBufParser<protobuffer::optional::ENUM, // attributes
                       protobuffer::optional::ENUM, // members
                       protobuffer::optional::ENUM, // addfrominvitelink
                       protobuffer::optional::ENUM // memberlabel
                       > AccessControl;

typedef ProtoBufParser<protobuffer::optional::STRING, // title            \.
                       protobuffer::optional::BYTES,  // avatar            | oneof
                       protobuffer::optional::UINT32, // expiration timer  |
                       protobuffer::optional::STRING  // desc             /
                       > GroupAttributeBlob;

typedef ProtoBufParser<protobuffer::DUMMY, // dummy
                       GroupAttributeBlob, // title
                       protobuffer::optional::STRING,
                       GroupAttributeBlob, // avatarurl
                       AccessControl,
                       protobuffer::optional::UINT32, // version
                       std::vector<Member>,
                       std::vector<MemberPendingProfileKey>,
                       std::vector<MemberPendingAdminApproval>,
                       protobuffer::optional::BYTES, // invite link password
                       GroupAttributeBlob, // desc
                       protobuffer::optional::BOOL, // announcements only
                       std::vector<MemberBanned>,
                       protobuffer::optional::BOOL // terminated
                       > GroupSnapShot;

typedef ProtoBufParser<protobuffer::optional::BYTES, // masterkey
                       protobuffer::optional::BOOL, // whitelisted
                       protobuffer::optional::BOOL, // hidestory
                       protobuffer::optional::ENUM, // storysendmode
                       GroupSnapShot,
                       protobuffer::optional::BOOL, // blocked
                       protobuffer::optional::ENUM // AvatarColor
                       > Group;

typedef ProtoBufParser<protobuffer::optional::STRING, // name
                       protobuffer::optional::BOOL, // allow replies
                       protobuffer::optional::ENUM, // Privacymode
                       protobuffer::repeated::UINT64 // members
                       > DistributionList;

typedef ProtoBufParser<protobuffer::optional::BYTES, // distribution id
                       protobuffer::optional::UINT64, // deletion timestamp   \ oneof
                       DistributionList               //                      /
                       > DistributionListItem;
typedef ProtoBufParser<> ReleaseNotes;

typedef ProtoBufParser<protobuffer::optional::BYTES, // rootkey
                       protobuffer::optional::BYTES, // adminkey
                       protobuffer::optional::STRING, // name
                       protobuffer::optional::ENUM, // Restrictions
                       protobuffer::optional::UINT64, // expiration
                       protobuffer::DUMMY // actual dummy
                       > CallLink;

typedef ProtoBufParser<protobuffer::optional::UINT64, // id (for referencing within protobuf
                       Contact,              //   \.
                       Group,                //    |
                       DistributionListItem, //    |
                       Self,                 //    | oneof
                       ReleaseNotes,         //    |
                       CallLink              //   /
                       > Recipient;

// typedef ProtoBufParser<> Gradient;
// typedef ProtoBufParser<> CustumChatColor;
// typedef ProtoBufParser<> ;
// typedef ProtoBufParser<> ;
// typedef ProtoBufParser<> ;
// typedef ProtoBufParser<> ;

typedef ProtoBufParser<protobuffer::optional::STRING, // url
                       protobuffer::optional::STRING, // title
                       FilePointer,
                       protobuffer::optional::STRING, // description
                       protobuffer::optional::UINT64 // date
                       > LinkPreview;

typedef ProtoBufParser<FilePointer,
                       protobuffer::optional::ENUM, // flag
                       protobuffer::optional::BOOL, // donwloaded
                       protobuffer::optional::BYTES // client uuid
                       > MessageAttachment;

typedef ProtoBufParser<protobuffer::optional::UINT64, // id
                       protobuffer::optional::UINT64, // recipient id
                       protobuffer::optional::BOOL, // archived
                       protobuffer::optional::UINT32, // pinned order
                       protobuffer::optional::UINT64, // exp timerms
                       protobuffer::optional::UINT64, // mute until
                       protobuffer::optional::BOOL, // markedunread
                       protobuffer::optional::BOOL, // no notify mention if muted
                       ChatStyle,
                       protobuffer::optional::UINT32 // exp timer version
                       > Chat;

typedef ProtoBufParser<protobuffer::optional::UINT64, // date received
                       protobuffer::optional::UINT64, // date server sent
                       protobuffer::optional::BOOL, // read
                       protobuffer::optional::BOOL  // sealed sender
                       > IncomingMessageDetails;

typedef ProtoBufParser<protobuffer::optional::UINT64, // redid
                       protobuffer::optional::UINT64, //timestamp
                       // ONE OF:
                       ProtoBufParser<>, // Pending
                       ProtoBufParser<protobuffer::optional::BOOL>, // Sent -> sealedsender
                       ProtoBufParser<protobuffer::optional::BOOL>, // Delivered -> sealedsender
                       ProtoBufParser<protobuffer::optional::BOOL>, // Read -> sealedsender
                       ProtoBufParser<protobuffer::optional::BOOL>, // Viewed -> sealedsender
                       ProtoBufParser<>, // Skipped -> sealedsender
                       ProtoBufParser<protobuffer::optional::ENUM> // Failed -> failure reason
                       > SendStatus;

typedef ProtoBufParser<std::vector<SendStatus>,
                       protobuffer::optional::UINT64 // date received
                       > OutgoingMessageDetails;

typedef ProtoBufParser<> DirectionlessMessageDetails;

typedef ProtoBufParser<protobuffer::optional::STRING, // emoji
                       protobuffer::optional::UINT64, // author id
                       protobuffer::optional::UINT64, // timestamp
                       protobuffer::optional::UINT64 // sortorder
                       > Reaction;

typedef ProtoBufParser<protobuffer::optional::UINT64, // voter id
                       protobuffer::optional::UINT32 // votecount
                       > PollVote;

typedef ProtoBufParser<protobuffer::optional::STRING, // option
                       std::vector<PollVote>
                       > PollOption;

typedef ProtoBufParser<protobuffer::optional::UINT32, // start
                       protobuffer::optional::UINT32, // length
                       protobuffer::optional::BYTES, // mention aci   \ oneof
                       protobuffer::optional::ENUM   // style         /
                       > BodyRange;

typedef ProtoBufParser<protobuffer::optional::STRING, // body
                       std::vector<BodyRange>> Text;

typedef ProtoBufParser<protobuffer::optional::UINT64, // target sent timestamp
                       protobuffer::optional::UINT64, // author id
                       Text,
                       ProtoBufParser<protobuffer::optional::STRING, protobuffer::optional::STRING, MessageAttachment>, // QuotedAttachment: ct, filename, messageattachment
                       protobuffer::optional::ENUM // type
                       > Quote;
typedef ProtoBufParser<Quote,
                       Text,
                       std::vector<MessageAttachment>,
                       std::vector<LinkPreview>,
                       FilePointer,
                       std::vector<Reaction>
                       > StandardMessage;

typedef ProtoBufParser<protobuffer::optional::STRING, // givenname
                       protobuffer::optional::STRING, // familyname
                       protobuffer::optional::STRING, // prefix
                       protobuffer::optional::STRING, // suffix
                       protobuffer::optional::STRING, // middlename
                       protobuffer::optional::STRING //nickname
                       > Name;
typedef ProtoBufParser<protobuffer::optional::STRING, // value
                       protobuffer::optional::ENUM, // type
                       protobuffer::optional::STRING // label
                       > Phone;
typedef Phone Email;

typedef ProtoBufParser<protobuffer::optional::ENUM, // type
                       protobuffer::optional::STRING, // label
                       protobuffer::optional::STRING, // street
                       protobuffer::optional::STRING, // pobox
                       protobuffer::optional::STRING, // neighborhood
                       protobuffer::optional::STRING, // city
                       protobuffer::optional::STRING, // region
                       protobuffer::optional::STRING, // postcode
                       protobuffer::optional::STRING // country
                       > PostalAddress;
typedef ProtoBufParser<Name,
                       Phone,
                       Email,
                       PostalAddress,
                       FilePointer,
                       protobuffer::optional::STRING // orginization
                       > ContactAttachment;
typedef ProtoBufParser<ContactAttachment, // option
                       std::vector<Reaction>
                       > ContactMessage;

typedef ProtoBufParser<protobuffer::optional::BYTES, // packid
                       protobuffer::optional::BYTES, // packkey
                       protobuffer::optional::UINT32, // stickerid
                       protobuffer::optional::STRING, // emoji
                       FilePointer
                       > Sticker;
typedef ProtoBufParser<Sticker,
                       std::vector<Reaction>
                       > StickerMessage;

typedef ProtoBufParser<ProtoBufParser<>> RemoteDeletedMessage;

typedef ProtoBufParser<protobuffer::optional::UINT64, // callid
                       protobuffer::optional::ENUM, // type
                       protobuffer::optional::ENUM, // direction
                       protobuffer::optional::ENUM, // state
                       protobuffer::optional::UINT64, // timestamp
                       protobuffer::optional::BOOL // read
                       > IndividualCall;
typedef ProtoBufParser<protobuffer::optional::UINT64, // callid
                       protobuffer::optional::ENUM, // state
                       protobuffer::optional::UINT64, // ringer rec id
                       protobuffer::optional::UINT64, // started call rec id
                       protobuffer::optional::UINT64, // timestamp start
                       protobuffer::optional::UINT64, // timestamp end
                       protobuffer::optional::BOOL // read
                       > GroupCall;
typedef ProtoBufParser<protobuffer::optional::ENUM> SimpleChatUpdate; // TYPE
typedef ProtoBufParser<protobuffer::optional::UINT64> ExpirationTimerChatUpdate; // expire in ms
typedef ProtoBufParser<protobuffer::optional::STRING, protobuffer::optional::STRING> ProfileChangeChatUpdate; // previous name, new name
typedef ProtoBufParser<protobuffer::optional::UINT64> ThreadMergeChatUpdate; // previous e164
typedef ProtoBufParser<protobuffer::optional::UINT64> SessionSwitchoverChatUpdate; // e164

typedef ProtoBufParser<protobuffer::optional::BYTES, // updater aci
                       protobuffer::optional::BYTES, // new member aci
                       protobuffer::optional::BOOL,  // had invite
                       protobuffer::optional::BYTES  // inviter aci
                       > GroupMemberAddedUpdate;

typedef ProtoBufParser<protobuffer::optional::BYTES, // updater aci
                       protobuffer::optional::ENUM   // accesslevel
                       > GroupMemberLabelAccessLevelChangeUpdate;
typedef GroupMemberLabelAccessLevelChangeUpdate GroupMembershipAccessLevelChangeUpdate;
typedef GroupMemberLabelAccessLevelChangeUpdate GroupAttributesAccessLevelChangeUpdate;

typedef ProtoBufParser<protobuffer::optional::BYTES, // updater aci
                       protobuffer::optional::BOOL   // isAnnouncementonly
                       > GroupAnnouncementOnlyChangeUpdate;
typedef GroupAnnouncementOnlyChangeUpdate GroupInviteLinkEnabledUpdate; // updater, linkrequiresapproval
typedef GroupAnnouncementOnlyChangeUpdate GroupInviteLinkAdminApprovalUpdate; // updater, linkrequiresapproval
typedef GroupAnnouncementOnlyChangeUpdate GroupAvatarUpdate; // updater, removed

typedef ProtoBufParser<protobuffer::optional::BYTES, // updater aci
                       protobuffer::optional::BYTES, // updated aci
                       protobuffer::optional::BOOL   // admin status
                       > GroupAdminStatusUpdate;
typedef GroupAdminStatusUpdate GroupJoinRequestApprovalUpdate; // requestor, updater, wasApproved

typedef ProtoBufParser<protobuffer::optional::BYTES, // updater
                       protobuffer::optional::BYTES, // new member
                       protobuffer::optional::BOOL,  // had open invite
                       protobuffer::optional::BYTES  // inviter
                       > GroupMemberAddedUpdate;

typedef ProtoBufParser<protobuffer::optional::BYTES> GroupTerminateChangeUpdate; // updater aci
typedef GroupTerminateChangeUpdate GroupMemberLeftUpdate; // aci
typedef GroupTerminateChangeUpdate SelfInvitedToGroupUpdate; // inveter aci
typedef GroupTerminateChangeUpdate SelfInvitedOtherUserToGroupUpdate; //invitee id
typedef GroupTerminateChangeUpdate GroupMemberJoinedUpdate; // new member
typedef GroupTerminateChangeUpdate GroupSelfInvitationRevokedUpdate; // revoker
typedef GroupTerminateChangeUpdate GroupJoinRequestUpdate; // requestor
typedef GroupTerminateChangeUpdate GroupJoinRequestCanceledUpdate; // requestor
typedef GroupTerminateChangeUpdate GroupInviteLinkResetUpdate; // updater
typedef GroupTerminateChangeUpdate GroupInviteLinkDisabledUpdate; // updater
typedef GroupTerminateChangeUpdate GroupMemberJoinedByLinkUpdate; // new member
typedef GroupTerminateChangeUpdate GenericGroupUpdate; // updater
typedef GroupTerminateChangeUpdate GroupCreationUpdate; // updater

typedef ProtoBufParser<protobuffer::optional::BYTES, // updater
                       protobuffer::optional::STRING // new name
                       > GroupNameUpdate;
typedef GroupNameUpdate GroupDescriptionUpdate; // updater, new desc

typedef ProtoBufParser<protobuffer::optional::BYTES, // updater
                       std::vector<ProtoBufParser<protobuffer::optional::BYTES,  // inviter
                                                  protobuffer::optional::BYTES,  // invitee aci
                                                  protobuffer::optional::BYTES>> // invitee pni
                       > GroupInvitationRevokedUpdate; // remover

typedef ProtoBufParser<protobuffer::optional::BYTES, // removerer aci
                       protobuffer::optional::BYTES // removed aci
                       > GroupMemberRemovedUpdate;
typedef GroupMemberRemovedUpdate GroupInvitationAcceptedUpdate; // inviter, new member
typedef GroupMemberRemovedUpdate GroupInvitationDeclinedUpdate; // inviter, invitee

typedef ProtoBufParser<protobuffer::optional::BYTES, // inviter aci
                       protobuffer::optional::UINT32   // invitee count
                       > GroupUnknownInviteeUpdate;
typedef GroupUnknownInviteeUpdate GroupSequenceOfRequestsAndCancelsUpdate; // requestor, count

typedef ProtoBufParser<protobuffer::optional::UINT32> GroupV2MigrationInvitedMembersUpdate; // invited count
typedef GroupV2MigrationInvitedMembersUpdate GroupV2MigrationDroppedMembersUpdate; // droppedcount

typedef ProtoBufParser<protobuffer::optional::UINT64, protobuffer::optional::BYTES> GroupExpirationTimerUpdate; // expires_in, updater

typedef ProtoBufParser<GenericGroupUpdate,
                       GroupCreationUpdate,
                       GroupNameUpdate,
                       GroupAvatarUpdate,
                       GroupDescriptionUpdate,
                       GroupMembershipAccessLevelChangeUpdate,
                       GroupAttributesAccessLevelChangeUpdate,
                       GroupAnnouncementOnlyChangeUpdate,
                       GroupAdminStatusUpdate,
                       GroupMemberLeftUpdate,
                       GroupMemberRemovedUpdate,
                       SelfInvitedToGroupUpdate,
                       SelfInvitedOtherUserToGroupUpdate,
                       GroupUnknownInviteeUpdate,
                       GroupInvitationAcceptedUpdate,
                       GroupInvitationDeclinedUpdate,
                       GroupMemberJoinedUpdate,
                       GroupMemberAddedUpdate, // 18
                       GroupSelfInvitationRevokedUpdate,
                       GroupInvitationRevokedUpdate,
                       GroupJoinRequestUpdate,
                       GroupJoinRequestApprovalUpdate,
                       GroupJoinRequestCanceledUpdate,
                       GroupInviteLinkResetUpdate,
                       GroupInviteLinkEnabledUpdate,
                       GroupInviteLinkAdminApprovalUpdate,
                       GroupInviteLinkDisabledUpdate,
                       GroupMemberJoinedByLinkUpdate,
                       ProtoBufParser<>, // GroupV2MigrationUpdate
                       ProtoBufParser<>, // GroupV2MigrationSelfInvitedUpdate
                       GroupV2MigrationInvitedMembersUpdate,
                       GroupV2MigrationDroppedMembersUpdate,
                       GroupSequenceOfRequestsAndCancelsUpdate,
                       GroupExpirationTimerUpdate,
                       GroupMemberLabelAccessLevelChangeUpdate, // 35
                       GroupTerminateChangeUpdate
                       > Update;
typedef ProtoBufParser<std::vector<Update>> GroupChangeChatUpdate;
typedef ProtoBufParser<protobuffer::optional::UINT64, protobuffer::optional::STRING> LearnedProfileChatUpdate; // oneof(previous e164, previous username)
typedef ProtoBufParser<protobuffer::optional::UINT64, protobuffer::optional::STRING> PollterminateUpdate; // target sent timestamp, question
typedef ProtoBufParser<protobuffer::optional::UINT64, protobuffer::optional::UINT64> PinMessageUpdate; // target sent timesatmp, author id

typedef ProtoBufParser<SimpleChatUpdate,
                       GroupChangeChatUpdate,
                       ExpirationTimerChatUpdate,
                       ProfileChangeChatUpdate,
                       ThreadMergeChatUpdate,
                       SessionSwitchoverChatUpdate,
                       IndividualCall,
                       GroupCall,
                       LearnedProfileChatUpdate,
                       PollterminateUpdate,
                       PinMessageUpdate
                       > ChatUpdateMessage;

typedef ProtoBufParser<protobuffer::optional::ENUM, // Status
                       ProtoBufParser<protobuffer::repeated::BYTES, protobuffer::repeated::BYTES>, // publickey, keyImages
                       protobuffer::optional::UINT64, // timestamp
                       protobuffer::optional::UINT64, // timestamp
                       protobuffer::optional::UINT64, // timestamp
                       protobuffer::optional::BYTES, // transaction
                       protobuffer::optional::BYTES // receipt
                       >Transaction;
typedef ProtoBufParser<protobuffer::optional::ENUM>FailedTransaction; // failurereason

typedef ProtoBufParser<Transaction,      //  \ oneof
                       FailedTransaction //  /
                       > TransactionDetails;

typedef ProtoBufParser<protobuffer::optional::STRING, // amount mob
                       protobuffer::optional::STRING, // fee mob
                       protobuffer::optional::STRING, // note
                       TransactionDetails
  > PaymentNotification;

typedef ProtoBufParser<protobuffer::optional::BYTES, // recieptcreadentialpresentation
                       protobuffer::optional::ENUM // State
                       > GiftBadge;

typedef ProtoBufParser<MessageAttachment,
                       std::vector<Reaction>
                       > ViewOnceMessage;

typedef ProtoBufParser<ProtoBufParser<Text, FilePointer>, //     \ oneof
                       protobuffer::optional::STRING, // emoji   /
                       std::vector<Reaction>,
                       protobuffer::DUMMY
                       > DirectStoryReplyMessage;

typedef ProtoBufParser<protobuffer::optional::STRING, // question
                       protobuffer::optional::BOOL, // multivote
                       std::vector<PollOption>, //options
                       protobuffer::optional::BOOL, // ended
                       std::vector<Reaction> // reactions
                       > Poll;

typedef ProtoBufParser<protobuffer::optional::UINT64> AdminDeletedMessage; // adminid

typedef ProtoBufParser<protobuffer::optional::UINT64, // pinned_at
                       protobuffer::optional::UINT64, //  \.onof    expiresat
                       protobuffer::optional::BOOL    //  /         neeverexpires
                       > PinDetails;


typedef ProtoBufParser<protobuffer::optional::UINT64, // conv id
                       protobuffer::optional::UINT64, // author id
                       protobuffer::optional::UINT64, // date sent
                       protobuffer::optional::UINT64, // expire start time
                       protobuffer::optional::UINT64, // expires  in
                       // THE NEXT ITEM IS ACTUALLY A repeated ChatItem
                       // THIS WOULD REQUIRE A RECURSICE TYPEDEF.
                       // POSSIBLY CAN JUST USE protobuffer::optional::BYTES?
                       // AND INTERPRET AS ChatItem IF PRESENT
                       protobuffer::repeated::BYTES, // Revisions
                       protobuffer::optional::BOOL, // sms
                       IncomingMessageDetails, //       \.
                       OutgoingMessageDetails, //        > oneof
                       DirectionlessMessageDetails, //  /
                       StandardMessage,          //    \.
                       ContactMessage,           //     |
                       StickerMessage,           //     |
                       RemoteDeletedMessage,     //     |
                       ChatUpdateMessage,        //     |
                       PaymentNotification,      //      > oneof
                       GiftBadge,                //     |
                       ViewOnceMessage,          //     |
                       DirectStoryReplyMessage,  //     |
                       Poll,                     //     |
                       PinDetails,               //     |   <---- (not part of oneof)
                       AdminDeletedMessage       //    /
                       > ChatItem;

typedef ProtoBufParser<protobuffer::optional::BYTES, // packid
                       protobuffer::optional::BYTES // packkey
                       > StickerPack;

typedef ProtoBufParser<protobuffer::optional::UINT64, // callid
                       protobuffer::optional::UINT64, // recid
                       protobuffer::optional::ENUM, // State
                       protobuffer::optional::UINT64 // timestamp
                       > AdHocCall;

typedef ProtoBufParser<protobuffer::optional::STRING, // name
                       protobuffer::optional::STRING, // emoji
                       protobuffer::optional::FIXED32, // color
                       protobuffer::optional::UINT64, // created at
                       protobuffer::optional::BOOL, // allow all calls
                       protobuffer::optional::BOOL, // allow all mentions
                       protobuffer::repeated::UINT64, // allowed members
                       protobuffer::optional::BOOL, // schedule enabled
                       protobuffer::optional::UINT32, // schedule start
                       protobuffer::optional::UINT32, // schedule end
                       protobuffer::repeated::ENUM, // DayOfWeek
                       protobuffer::optional::BYTES // id
                       > NotificationProfile;

typedef ProtoBufParser<protobuffer::optional::STRING, // name
                       protobuffer::optional::BOOL, // showonlyunread
                       protobuffer::optional::BOOL, // show muted
                       protobuffer::optional::BOOL, // include all 1-on-1
                       protobuffer::optional::BOOL, // include all groups
                       protobuffer::optional::ENUM, // FolderType
                       protobuffer::repeated::UINT64, // included recids
                       protobuffer::repeated::UINT64, // excluded recids
                       protobuffer::optional::BYTES // id
                       > ChatFolder;

typedef ProtoBufParser<AccountData,
                       Recipient,
                       Chat,
                       ChatItem,
                       StickerPack,
                       AdHocCall,
                       NotificationProfile,
                       ChatFolder> Frame;

} // namespace

#define ACCOUNTDATA_FRAMENUMBER 1
#define RECIPIENT_FRAMENUMBER 2
#define CHAT_FRAMENUMBER 3
#define CHATITEM_FRAMENUMBER 4
#define STICKERPACK_FRAMENUMBER 5
#define ADHOCCALL_FRAMENUMBER 6
#define NOTIFICATIONPROFILE_FRAMENUMBER 7
#define CHATFOLDER_FRAMENUMBER 8

#endif
