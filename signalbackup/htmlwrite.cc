/*
 *     IMPORTANT LICENSE NOTICE
 *
 *   The HTML and CSS produced by these functions is (heavily) modified from the
 *   template found in https://github.com/GjjvdBurg/signal2html.
 *
 *   To adhere to the license of that project, the code in this file can be
 *   considered to fall under the same MIT license. The full license text from
 *   the original project is copied below.
 */

/*
  Copyright 2020-2021, signal2html contributors.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

/*
  Many thanks to Gertjan van den Burg (https://github.com/GjjvdBurg) for his
  original project (used with permission) without which this function would
  not have come together so quickly (if at all).
*/

#include "signalbackup.ih"

#include <chrono>

#include "../mimetypes/mimetypes.h"
#include "../common_be.h"

bool SignalBackup::HTMLwriteStart(std::ofstream &file, long long int thread_recipient_id,
                                  std::string const &directory, std::string const &threaddir, bool isgroup,
                                  bool isnotetoself, bool isreleasechannel, std::set<long long int> const &recipient_ids,
                                  std::map<long long int, RecipientInfo> *recipient_info,
                                  std::map<long long int, std::string> *written_avatars,
                                  bool overwrite, bool append, bool light, bool themeswitch,
                                  bool searchpage, bool exportdetails, bool pagemenu) const
{
  std::vector<long long int> groupmembers;
  if (isgroup)
  {
    SqliteDB::QueryResults results;
    d_database.exec("SELECT group_id from recipient WHERE _id IS ?", thread_recipient_id, &results);
    if (results.rows() == 1)
      getGroupMembersOld(&groupmembers, results.valueAsString(0, "group_id"));
  }

  GroupInfo groupinfo;
  if (isgroup)
    getGroupInfo(thread_recipient_id, &groupinfo);

  // sort group members by admin and name
  std::sort(groupmembers.begin(), groupmembers.end(),
            [this, &groupinfo, &recipient_info](auto left, auto right) {
              return (bepaald::contains(groupinfo.admin_ids, left) && !bepaald::contains(groupinfo.admin_ids, right)) ||
                ((bepaald::contains(groupinfo.admin_ids, left) == bepaald::contains(groupinfo.admin_ids, right)) &&
                 (getRecipientInfoFromMap(recipient_info, left).display_name < getRecipientInfoFromMap(recipient_info, right).display_name));});

  std::string thread_avatar = bepaald::contains(written_avatars, thread_recipient_id) ?
    (*written_avatars)[thread_recipient_id] :
    ((*written_avatars)[thread_recipient_id] = HTMLwriteAvatar(thread_recipient_id, directory, threaddir, overwrite, append));

  std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  //file << "<!-- Generated on " << std::put_time(std::localtime(&now), "%F %T") // %F and %T do not work on minGW
  file <<
    "<!-- Generated on " << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") <<
    " by signalbackup-tools (" << VERSIONDATE << "). "
    "Input database version: " << d_databaseversion << ". -->\n";

  std::string title = (isnotetoself ? "Note to Self" : getRecipientInfoFromMap(recipient_info, thread_recipient_id).display_name);
  HTMLescapeString(&title);

  bool ismuted = getRecipientInfoFromMap(recipient_info, thread_recipient_id).mute_until == 0x7FFFFFFFFFFFFFFF;
  bool isblocked = getRecipientInfoFromMap(recipient_info, thread_recipient_id).blocked;

  file <<
    "<!DOCTYPE html>\n"
    "<html>\n"
    "  <head>\n"
    "    <meta charset=\"utf-8\">\n"
    "    <title>" << title << "</title>\n"
    "    <style>\n"
    "      :root" << (themeswitch ? "[data-theme=\"" + (light ? "light"s : "dark") + "\"]" : "") << " {\n"
    "        /* " << (light ? "light" : "dark") << " */\n"
    "        --body-bgc: " << (light ? "#EDF0F6;" : "#000000;") << "\n"
    "        --body-c: " << (light ? "#000000;" : "#FFFFFF;") << "\n"
    "        --messageheader-c: " << (light ? "#000000;" : "#FFFFFF;") << "\n"
    "        --conversationbox-bc: " << (light ? "#FBFCFF;" : "#1B1C1F;") << "\n"
    "        --conversationbox-c: " << (light ? "#000000;" : "#FFFFFF;") << "\n"
    "        --msgincoming-b: " << (light ? "#E7EBF3;" : "#303133;") << "\n"
    // "        --msgoutgoing-c: " << (light ? "#FFFFFF;" : "#FFFFFF;") << "\n"
    "        --msgoutgoing-c: #FFFFFF;\n"
    // "        --spoilerout-b: " << (light ? "rgba(255, 255, 255, 0.5);" : "rgba(255, 255, 255, 0.5);") << "\n"
    "        --spoilerout-b: rgba(255, 255, 255, 0.5);\n"
    "        --spoilerin-b: " << (light ? "rgba(0, 0, 0, 0.5);" : "rgba(255, 255, 255, 0.5);") << "\n"
    "        --deletedmsg-border: " << (light ? "#000000;" : "#FFFFFF;") << "\n"
    "        --deletedmsg-c: " << (light ? "#000000;" : "#FFFFFF;") << "\n"
    "        --nobgbubble-footer-c: " << (light ? "#000000;" : "#FFFFFF;") << "\n"
    "        --nobgbubble-checkmarks-f: " << (light ? "brightness(0);" : "none;") << "\n"
    "        --mentionin-bc: " << (light ? "#C6C6C6;" : "#5E5E5E;") << "\n"
    "        --msgreaction-bc: " << (light ? "#E7EBF3;" : "#303133;") << "\n"
    "        --msgreaction-border: " << (light ? "#FBFCFF;" : "#1B1C1F;") << "\n"
    "        --msgreaction-c: " << (light ? "#000000;" : "#FFFFFF;") << "\n"
    "        --msgreactioninfo-bc: " << (light ? "#D2D6DE;": "#505050;") << "\n"
    "        --msgreactioninfo-border: " << (light ? "#1B1C1F;": "#FBFCFF;") << "\n"
    "        --reactioncount-c: " << (light ? "#000000;" : "#FFFFFF;") << "\n"
    "        --incominglinkpreview-bc: " << (light ? "rgba(255, 255, 255, .5);" : "rgba(255, 255, 255, .16);") << "\n"
    // "        --outgoinglinkpreview-bc: " << (light ? "rgba(255, 255, 255, .485);" : "rgba(255, 255, 255, .485);") << "\n"
    "        --outgoinglinkpreview-bc: rgba(255, 255, 255, .485);\n"
    "        --icon-f: " << (light ? "brightness(0);" : "none;") << "\n"
    "        --menuitem-c: " << (light ? "#000000;" : "#FFFFFF;") << "\n"
    "        --media-status-checkmarks-f: " << (light ? "brightness(.75);" : "brightness(.25);") << "\n"
    "        --nav-disabled-f: " << (light ? "brightness(.8);" : "brightness(.15);") << "\n"
    "        --shared-contact-incoming-f: " << (light ? "brightness(1);" : "brightness(.5);") << "\n"
    "        --shared-contact-outgoing-f: " << (light ? "brightness(.9);" : "brightness(1);") << "\n"
    "      }\n"
    "\n";

  if (themeswitch)
  {
    file <<
      "      :root[data-theme=\"" << (!light ? "light" : "dark") << "\"] {\n"
      "        /* " << (!light ? "light" : "dark") << " */\n"
      "        --body-bgc: " << (!light ? "#EDF0F6;" : "#000000;") << "\n"
      "        --body-c: " << (!light ? "#000000;" : "#FFFFFF;") << "\n"
      "        --messageheader-c: " << (!light ? "#000000;" : "#FFFFFF;") << "\n"
      "        --conversationbox-bc: " << (!light ? "#FBFCFF;" : "#1B1C1F;") << "\n"
      // "        --conversationbox-bc: " << (!light ? (getRecipientInfoFromMap(recipient_info, thread_recipient_id).wall_light.empty() ? "#FBFCFF;" : "#" + getRecipientInfoFromMap(recipient_info, thread_recipient_id).wall_light + ";") : (getRecipientInfoFromMap(recipient_info, thread_recipient_id).wall_dark.empty() ? "#1B1C1F;" : "#" + getRecipientInfoFromMap(recipient_info, thread_recipient_id).wall_dark + ";")) << "\n"
      "        --conversationbox-c: " << (!light ? "#000000;" : "#FFFFFF;") << "\n"
      "        --msgincoming-b: " << (!light ? "#E7EBF3;" : "#303133;") << "\n"
      // "        --msgoutgoing-c: " << (!light ? "#FFFFFF;" : "#FFFFFF;") << "\n"
      "        --msgoutgoing-c: #FFFFFF;\n"
      // "        --spoilerout-b: " << (!light ? "rgba(255, 255, 255, 0.5);" : "rgba(255, 255, 255, 0.5);") << "\n"
      "        --spoilerout-b: rgba(255, 255, 255, 0.5);\n"
      "        --spoilerin-b: " << (!light ? "rgba(0, 0, 0, 0.5);" : "rgba(255, 255, 255, 0.5);") << "\n"
      "        --deletedmsg-border: " << (!light ? "#000000;" : "#FFFFFF;") << "\n"
      "        --deletedmsg-c: " << (!light ? "#000000;" : "#FFFFFF;") << "\n"
      "        --nobgbubble-footer-c: " << (!light ? "#000000;" : "#FFFFFF;") << "\n"
      "        --nobgbubble-checkmarks-f: " << (!light ? "brightness(0);" : "none;") << "\n"
      "        --mentionin-bc: " << (!light ? "#C6C6C6;" : "#5E5E5E;") << "\n"
      "        --msgreaction-bc: " << (!light ? "#E7EBF3;" : "#303133;") << "\n"
      "        --msgreaction-border: " << (!light ? "#FBFCFF;" : "#1B1C1F;") << "\n"
      "        --msgreaction-c: " << (!light ? "#000000;" : "#FFFFFF;") << "\n"
      "        --msgreactioninfo-bc: " << (!light ? "#D2D6DE;": "#505050;") << "\n"
      "        --msgreactioninfo-border: " << (!light ? "#1B1C1F;": "#FBFCFF;") << "\n"
      "        --reactioncount-c: " << (!light ? "#000000;" : "#FFFFFF;") << "\n"
      "        --incominglinkpreview-bc: " << (!light ? "rgba(255, 255, 255, .5);" : "rgba(255, 255, 255, .16);") << "\n"
      // "        --outgoinglinkpreview-bc: " << (!light ? "rgba(255, 255, 255, .485);" : "rgba(255, 255, 255, .485);") << "\n"
      "        --outgoinglinkpreview-bc: rgba(255, 255, 255, .485);\n"
      "        --icon-f: " << (!light ? "brightness(0);" : "none;") << "\n"
      "        --menuitem-c: " << (!light ? "#000000;" : "#FFFFFF;") << "\n"
      "        --media-status-checkmarks-f: " << (!light ? "brightness(.75);" : "brightness(.25);") << "\n"
      "        --nav-disabled-f: " << (!light ? "brightness(.8);" : "brightness(.15);") << "\n"
      "        --shared-contact-incoming-f: " << (!light ? "brightness(1);" : "brightness(.5);") << "\n"
      "        --shared-contact-outgoing-f: " << (!light ? "brightness(.9);" : "brightness(1);") << "\n"
      "      }"
      "\n";
  }

  file << R"(      body {
        margin: 0px;
        padding: 0px;
        width: 100%;
        background-color: var(--body-bgc);
      }

      #theme-switch {
        display: none;
      }

      #page {
        background-color: var(--body-bgc);
        margin: 0px;
        display: flex;
        flex-direction: row;
        transition: color .2s, background-color .2s;
      })";

  if (exportdetails)
  {
    file <<
      "\n\n"
      "      .export-details {\n"
      "        display: none;\n"
      "        grid-template-columns: repeat(2 , 1fr);\n"
      "        color: var(--body-c);\n"
      "        margin-left: auto;\n"
      "        margin-right: auto;\n"
      "        margin-bottom: 10px;\n"
      "        margin-top: 10px;\n"
      "        grid-gap: 0px 15px;\n"
      "        width: fit-content;\n"
      "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;\n"
      "        break-inside: avoid;\n"
      "      }\n"
      "      .export-details-fullwidth {\n"
      "        text-align: center;\n"
      "        font-weight: bold;\n"
      "        grid-column: 1 / 3;\n"
      "      }\n"
      "      .export-details div:nth-child(odd of :not(.export-details-fullwidth)) {\n"
      "        text-align: right;\n"
      "        font-style: italic;\n"
      "      }\n";
  }

  file << R"(
      .controls-wrapper {
        display: flex;
        justify-content: center;
        flex-direction: row;
        margin: 0 auto;
        flex: 1 1 100%;
      }

      .conversation-wrapper {
        display: flex;
        flex-direction: column;
        align-items: center;
        width: calc(50% + 60px);
      }

      #message-header {
        text-align: center;
        color: var(--messageheader-c);
        font-family: Roboto, "Noto Sans", "Liberation Sans", OpenSans, sans-serif;
        padding-top: 30px;
        padding-bottom: 30px;
        width: 90%;
      }

      .conversation-box {
        display: flex;
        flex-direction: column;
        padding-left: 30px;
        padding-right: 30px;
        padding-bottom: 30px;
        margin-bottom: 30px;
        background-color: var(--conversationbox-bc);
        color: var(--conversationbox-c);
        font-family: Roboto, "Noto Sans", "Liberation Sans", OpenSans, sans-serif;
        border-radius: 10px;
        width: calc(100% - 60px);
      }

      .monospace
      {
        font-family: 'Roboto Mono', 'Noto Mono', "Liberation Mono", OpenMono,  monospace;
      }

      .spoiler {
        transition: background .2s, filter .2s;
      }

      .msg-outgoing .spoiler {
        filter: blur(5px) saturate(0%) contrast(0) brightness(2);
        background: var(--spoilerout-b);
      }

      .msg-incoming .spoiler {
        filter: blur(5px) saturate(0%) contrast(0);
        background: var(--spoilerin-b);
      }

      .spoiler:hover,
      .spoiler:active {
        background: transparent;
        filter: none;
        transition: background .2s, filter .2s;
      }

      .incoming-group-msg {
        display: flex;
        flex-direction: row;
      }

      .msg-incoming {
        align-self: flex-start;
        background: var(--msgincoming-b);
      }

)";

  for (long long int id : recipient_ids)
    file << "      .msg-sender-" << id << " { background: #" << getRecipientInfoFromMap(recipient_info, id).color << ";}\n";
  file << '\n';
  for (long long int id : recipient_ids)
    file << "      .msg-name-" << id << " { color: #" << getRecipientInfoFromMap(recipient_info, id).color << ";}\n";

  file << R"(
      .msg-outgoing {
        align-self: flex-end;
        background: #)" << /*(isgroup ? s_html_colormap.at("ULTRAMARINE") : */getRecipientInfoFromMap(recipient_info, thread_recipient_id).color/*)*/ << R"(;
        color: var(--msgoutgoing-c);
      }

      .deleted-msg {
        background: rgba(0, 0, 0, 0);
        border: 1px solid var(--deletedmsg-border);
        color: var(--deletedmsg-c);
      }

      .avatar {
        font-weight: 500;
        border-radius: 50% 50%;
        aspect-ratio: 1 / 1;
        text-align: center;
        color: #FFFFFF;
      }

      .avatar-emoji-initial {
        font-family: "Apple Color Emoji", "Noto Color Emoji", sans-serif;
      }
)";

  for (long long int id : recipient_ids)
  {
    std::string recipient_avatar = bepaald::contains(written_avatars, id) ?
      (*written_avatars)[id] :
      ((*written_avatars)[id] = HTMLwriteAvatar(id, directory, threaddir, overwrite, append));
    if (!recipient_avatar.empty())
    {
      file << R"(
      .avatar-)" << id << R"( {
        background-image: url(")" << recipient_avatar << R"(");
        background-position: center;
        background-repeat: no-repeat;
        background-size: cover;
        color: #FFFFFF;
      }
)";
    }
  }
  file << R"(
      .convo-avatar {
        font-size: x-large;
        margin: auto 15px 15px 0px;
        height: 37px;
        line-height: 30px;
        display: flex;
        justify-content: center;
        align-items: center;
        align-content: center;
      }

      .header-avatar {
        font-size: 70px;
        width: 120px;
        line-height: 120px;
        margin-left: auto;
        margin-right: auto;
        margin-bottom: 5px;
        z-index: 1;
        position: relative;
        transition: z-index, transform .25s ease;
        transition-delay: .25s, 0s;
      }

      .note-to-self-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" width="80" height="80" viewBox="0 0 80 80" fill="white"><path d="M58,7.5A6.51,6.51 0,0 1,64.5 14L64.5,66A6.51,6.51 0,0 1,58 72.5L22,72.5A6.51,6.51 0,0 1,15.5 66L15.5,14A6.51,6.51 0,0 1,22 7.5L58,7.5M58,6L22,6a8,8 0,0 0,-8 8L14,66a8,8 0,0 0,8 8L58,74a8,8 0,0 0,8 -8L66,14a8,8 0,0 0,-8 -8ZM60,24L20,24v1.5L60,25.5ZM60,34L20,34v1.5L60,35.5ZM60,44L20,44v1.5L60,45.5ZM50,54L20,54v1.5L50,55.5Z"/></svg>');
        background-position: center;
        background-repeat: no-repeat;
        background-size: cover;
        position: relative;
        width: 86px;
        height: 86px;
        top: calc(50% - 86px / 2);
        left: calc(50% - 86px / 2);
      }

      .group-avatar-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" width="40" height="40" viewBox="0 0 40 40" fill="white"><path d="M29,16.75a6.508,6.508 0,0 1,6.5 6.5L35.5,24L37,24v-0.75a8,8 0,0 0,-6.7 -7.885,6.5 6.5,0 1,0 -8.6,0 7.941,7.941 0,0 0,-2.711 0.971A6.5,6.5 0,1 0,9.7 25.365,8 8,0 0,0 3,33.25L3,34L4.5,34v-0.75a6.508,6.508 0,0 1,6.5 -6.5h6a6.508,6.508 0,0 1,6.5 6.5L23.5,34L25,34v-0.75a8,8 0,0 0,-6.7 -7.885,6.468 6.468,0 0,0 1.508,-7.771A6.453,6.453 0,0 1,23 16.75ZM14,25.5a5,5 0,1 1,5 -5A5,5 0,0 1,14 25.5ZM21,10.5a5,5 0,1 1,5 5A5,5 0,0 1,21 10.5Z"/></svg>');
        background-position: center;
        background-repeat: no-repeat;
        background-size: cover;
        position: relative;
        width: 86px;
        height: 86px;
        top: calc(50% - 86px / 2);
        left: calc(50% - 86px / 2);
      }
)";
  if (!thread_avatar.empty() && !isnotetoself)
  {
    file << R"(
      .header-avatar:hover {
        cursor: zoom-in;
      }

      #message-header input[type=checkbox] {
        display: none;
      }

      #message-header input[type=checkbox]:checked ~ label > .avatar {
        transform: translateY(240px) scale(5);
        border-radius: 0;
        position: relative;
        z-index: 2;
        cursor: zoom-out;
        transition: transform .25s ease;
      })";
  }
  file << R"(
      .msg {
        max-width: 50%;
        border-radius: .6em;
        margin: 7px 0;
        padding: 10px;
        position: relative;
      }

      .msg pre {
        font-family: Roboto, "Noto Sans", "Liberation Sans", OpenSans, sans-serif;
        white-space: pre-wrap;
        margin-top: 0px;
        margin-bottom: 5px;
        overflow-wrap: anywhere;
      }

      .msg pre a {
        color: inherit;
        text-decoration: underline;
      }

      .styled-link:link,
      .styled-link:visited,
      .styled-link:hover,
      .styled-link:active
      {
        color: #315FF4;
        text-decoration: none;
      }

      .footer {
        display: flex;
        flex-direction: row;
        justify-content: flex-end;
        align-items: center;
      }

      .footer-status {
        margin-bottom: 10px;
      }

      .msg-data,
      .edited {
        font-size: x-small;
        opacity: 75%;
        display: block;
      }

      .edited {
        margin-right: 5px;
        position: relative;
      }

      .edited pre {
        display: inline;
      }

      .edited-info-header {
        font-style: italic;
      }

      .history-header {
        display: flex;
        align-items: center;
        text-align: center;
        font-style: italic;
        margin-top: 0px;
        margin-bottom: 0px;
        color: var(--conversationbox-c);
      }
      .history-header::before,
      .history-header::after {
        content: '';
        flex: 1;
        border-bottom: 1px solid var(--conversationbox-c);
      }
      .history-header:not(:empty)::before {
        margin-right: .25em;
      }
      .history-header:not(:empty)::after {
        margin-left: .25em;
      }

      .footer-icons {
        margin-left: 5px;
      }

      .checkmarks-sent {
        height: 14px;
        width: 14px;
        background-image: url('data:image/svg+xml;utf-8,<svg xmlns="http://www.w3.org/2000/svg" width="14" height="14" viewBox="0 0 24 24" fill="white" stroke="white" stroke-width="0.5"><path d="M12,2.5A9.5,9.5 0,1 1,2.5 12,9.511 9.511,0 0,1 12,2.5M12,1A11,11 0,1 0,23 12,11 11,0 0,0 12,1ZM17.834,9.4L16.773,8.338l-6.541,6.541 -3,-3 -1.061,1.06L10.232,17Z"/></svg>');
      }

      .checkmarks-received {
        height: 12px;
        width: 19px;
        background-image: url('data:image/svg+xml;utf-8,<svg xmlns="http://www.w3.org/2000/svg" width="19" height="11.9999" viewBox="0 0 34.833624 22" fill="white" stroke="white" stroke-width="0.5"><path d="M 11.000092 0 A 11 11 0 1 0 15.268357 21.069186 A 12.375103 12.375103 0 0 1 13.4923 19.744305 A 9.5 9.5 0 0 1 1.500338 11.000092 A 9.511 9.511 0 0 1 11.000092 1.500338 A 9.5 9.5 0 0 1 12.729598 2.1054863 A 12.375103 12.375103 0 0 1 14.473428 0.56934069 A 11 11 0 0 0 11.000092 0 z M 6.2305207 10.878346 L 5.1706161 11.938251 L 9.2311968 15.998831 L 10.212325 15.017703 A 12.375103 12.375103 0 0 1 9.7253416 13.384877 L 9.2311968 13.879022 L 6.2305207 10.878346 z"/><path d="M 22.183427,1.5 A 9.5,9.5 0 1 1 12.683423,11 9.511,9.511 0 0 1 22.183427,1.5 m 0,-1.5 a 11,11 0 1 0 11,11 11,11 0 0 0 -11,-11 z m 5.834,8.3999998 -1.061,-1.062 -6.541,6.5410002 -3.000002,-3 -1.061001,1.06 4.061003,4.061 z"/></svg>');
      }

      .checkmarks-read {
        height: 12px;
        width: 19px;
        background-image: url('data:image/svg+xml;utf-8,<svg xmlns="http://www.w3.org/2000/svg" width="19" height="12" viewBox="0 0 34.833627 22.000184" fill="white" stroke="none"><path d="M 11.000092 0 A 11 11 0 1 0 16.181451 20.693208 A 12.375104 12.375104 0 0 1 11.74131 13.492301 L 9.2311973 15.998832 L 5.1670356 11.934671 L 6.2269403 10.874766 L 9.2276165 13.875442 L 11.512141 11.590918 A 12.375104 12.375104 0 0 1 11.45843 11.000092 A 12.375104 12.375104 0 0 1 16.188612 1.3033964 A 11 11 0 0 0 11.000092 0 z"/><path d="m 23.833441,0 a 11,11 0 1 0 11,11 11,11 0 0 0 -11,-11 z m -1.768001,16 -4.066003,-4.066 1.061001,-1.06 3.000002,3 6.541001,-6.541 1.066,1.067 z"/></svg>');
      }

      .is-expiring {
        height: 12px;
        width: 12px;
        background-image: url('data:image/svg+xml;utf-8,<svg xmlns="http://www.w3.org/2000/svg" width="12" height="12" viewBox="0 0 12 12" fill="white" stroke="none"><path d="M6.75,6a0.75,0.75 0,0 1,-1.5 0c0,-0.414 0.475,-3.581 0.5,-3.75S5.862,2 6,2s0.226,0.087 0.25,0.25S6.75,5.589 6.75,6ZM12,6a6,6 0,1 0,-6 6A6.006,6.006 0,0 0,12 6ZM11,6A5,5 0,1 1,6 1,5.006 5.006,0 0,1 11,6Z"/></svg>');
      }

      .msg-incoming .is-expiring {
        filter: var(--icon-f);
      }

      .msg-viewonce .viewonce {
        display: flex;
        align-items: center;
        margin-bottom: 5px;
      }

      .msg-viewonce .viewonce > pre {
        margin-bottom: 0px;
      }

      .msg-viewonce .unviewed_viewonce_icon,
      .msg-viewonce .viewonce_icon {
        aspect-ratio: 1/1;
        display: inline-block;
        position: relative;
        margin-right: 10px;
      }

      .msg-viewonce .viewonce_icon {
        width: 50px;
      }

      .msg-viewonce .unviewed_viewonce_icon {
        width: 20px;
      }

      .msg-viewonce.msg-incoming .unviewed_viewonce_icon,
      .msg-viewonce.msg-outgoing .viewonce_icon {
        background-image: url('data:image/svg+xml;utf-8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="white" stroke="none"><path d="M13.25 7.75v7.87a.88.88 0 1 1-1.75 0V9.5h-1.25a.74.74 0 0 1-.12-1.5h0A2.34 2.34 0 0 0 12 7.22.73.73 0 0 1 12.5 7a.76.76 0 0 1 .75.75zM12 1a11 11 0 1 0 7.31 19.21l-.19 1.09v.7a.88.88 0 0 0 1.76 0v-4a.89.89 0 0 0-.88-.88h-4a.88.88 0 0 0 0 1.76h1.34l1-.18a9.24 9.24 0 1 1 2.91-6.7 9.37 9.37 0 0 1-.41 2.74.87.87 0 1 0 1.67.52A11.19 11.19 0 0 0 23 12 11 11 0 0 0 12 1z"/></svg>');
      }

      .msg-viewonce.msg-incoming .unviewed_viewonce_icon {
        filter: var(--icon-f);
      }

      .msg-viewonce.msg-incoming .viewonce_icon {
        background-image: url('data:image/svg+xml;utf-8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="white" stroke="none"><path d="M13.25 7.75v7.87a.88.88 0 1 1-1.75 0V9.5h-1.25a.74.74 0 0 1-.12-1.5h0A2.34 2.34 0 0 0 12 7.22.73.73 0 0 1 12.5 7a.76.76 0 0 1 .75.75zM8.89 3.29a9.18 9.18 0 0 1 2.65-.53H12A.88.88 0 1 0 12 1h-.54a11.3 11.3 0 0 0-3.16.63.88.88 0 0 0 .29 1.7 1 1 0 0 0 .3-.04zM3.64 8a9.2 9.2 0 0 1 1.5-2.25.87.87 0 1 0-1.29-1.14 11.11 11.11 0 0 0-1.79 2.68.88.88 0 0 0 .41 1.17.84.84 0 0 0 .38.08.87.87 0 0 0 .79-.54zm-.88 8.19a.87.87 0 0 0 .53-1.11 9.31 9.31 0 0 1-.53-2.66v-.46A.88.88 0 1 0 1 12v.54a11.3 11.3 0 0 0 .63 3.16.87.87 0 0 0 .82.58.79.79 0 0 0 .31-.05zm5.7 5.3A.88.88 0 0 0 8 20.36a9.2 9.2 0 0 1-2.25-1.5.87.87 0 1 0-1.17 1.29 10.86 10.86 0 0 0 2.67 1.79.85.85 0 0 0 .38.09.89.89 0 0 0 .83-.5zM12.54 23a11.3 11.3 0 0 0 3.16-.63.86.86 0 0 0 .53-1.12.87.87 0 0 0-1.11-.53 9.31 9.31 0 0 1-2.66.53H12A.88.88 0 1 0 12 23zm7.61-3.61a10.86 10.86 0 0 0 1.79-2.67.87.87 0 1 0-1.58-.72 9.2 9.2 0 0 1-1.5 2.25.86.86 0 0 0 .06 1.23.81.81 0 0 0 .58.23.84.84 0 0 0 .65-.33zM23 12v-.54a11.3 11.3 0 0 0-.63-3.16.87.87 0 0 0-1.65.58 9.31 9.31 0 0 1 .53 2.66V12A.88.88 0 1 0 23 12zm-3.55-6.92a.87.87 0 0 0-.07-1.23 10.86 10.86 0 0 0-2.67-1.79.87.87 0 1 0-.71 1.58 9.2 9.2 0 0 1 2.25 1.5.85.85 0 0 0 .59.23.89.89 0 0 0 .61-.29z"/></svg>');
        filter: var(--icon-f);
      }

      .msg-emoji {
        font-family: "Apple Color Emoji", "Noto Color Emoji", sans-serif;
        font-style: normal;
      }

      .msg-all-emoji {
        font-size: xxx-large;
      }

      .msg-outgoing .msg-all-emoji {
        text-align: right;
      }

      .no-bg-bubble {
        background: rgba(0, 0, 0, 0);
      }

      .no-bg-bubble .footer {
        color: var(--nobgbubble-footer-c);
      }

      .no-bg-bubble .checkmarks-sent,
      .no-bg-bubble .checkmarks-read,
      .no-bg-bubble .checkmarks-received {
        filter: var(--nobgbubble-checkmarks-f);
      }

      :not(.spoiler) > .mention-in,
      .spoiler:hover > .mention-in {
        background-color: var(--mentionin-bc);
      }

      :not(.spoiler) > .mention-out,
      .spoiler:hover > .mention-out {
        background-color: rgba(0, 0, 0, 0.244);
      }

      .msg-date-change {
        font-size: small;
        align-self: center;
        position: relative;
        z-index: 0;
      }

      .msg-dl-link a {
        font-size: large;
        font-weight: 700;
        text-decoration: none;
        color: inherit;
        padding-left: 5px;
      }

      .msg-name {
        font-weight: bold;
        font-size: smaller;
        margin-bottom: 5px;
        display: block;
        width: max-content;
      }

      .call-link {
        border-top-left-radius: 0.6em;
        border-top-right-radius: 0.6em;
        border-bottom-left-radius: 0em;
        border-bottom-right-radius: 0em;
        padding: 7px;
        margin-bottom: 5px;
      }

      .call-link-avatar-container {
        aspect-ratio: 1 / 1;
        height: 70px;
        border-radius: 50%;
      }

      .call-link-avatar,
      .shared-contact-avatar {
        background-position: center;
        background-repeat: no-repeat;
        background-size: cover;
        aspect-ratio: 1 / 1;
        height: 50px;
      }

      .shared-contact-avatar {
        border-radius: 50%;
      }

      .call-link-avatar {
        position: relative;
        top: 10px;
        left:10px;
      }

      .shared-contact-avatar-default {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" width="256" height="256" fill="white"><path d="M174.1 188.5c-13.6 7.7-29.4 12.2-46.1 12.2s-32.5-4.4-46.1-12.2C45.7 199.6 18.1 224 10 256h236c-8.1-32-35.7-56.5-71.9-67.5zM128 20c-44.8 0-81.1 36.3-81.1 81.1s36.3 81.1 81.1 81.1 81.1-36.3 81.1-81.1S172.8 20 128 20zm0 142c-25.8 0-47.8-16-56.7-38.7h113.4C175.8 146 153.8 162 128 162z"/></svg>');
      }

      .call-link-avatar {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" width="24" height="24" fill="%23086DA0"><path d="M6.76 3.63h4.98l2 .04c.56.04 1.05.14 1.52.38a3.88 3.88 0 0 1 1.7 1.7c.23.46.33.95.37 1.5.04.46.04 1 .04 1.64l2.8-2.8c1.19-1.18 3.2-.35 3.2 1.32v9.18c0 1.67-2.01 2.5-3.2 1.32l-2.8-2.8-.04 1.63c-.04.56-.14 1.05-.38 1.52a3.88 3.88 0 0 1-1.7 1.7c-.46.23-.95.33-1.5.37-.54.05-1.2.05-2.01.05H6.76c-.8 0-1.47 0-2-.05-.56-.03-1.06-.13-1.52-.38a3.88 3.88 0 0 1-1.7-1.7c-.23-.46-.33-.95-.37-1.5-.05-.54-.05-1.2-.04-2.01V9.26l.04-2c.03-.56.13-1.06.38-1.52a3.88 3.88 0 0 1 1.7-1.7c.46-.23.95-.33 1.5-.37.54-.05 1.2-.05 2.01-.04zm8.86 5.67l-.03-1.9c-.04-.45-.1-.69-.2-.86a2.09 2.09 0 0 0-.93-.93c-.17-.1-.41-.16-.86-.2l-1.9-.04H6.8l-1.9.04c-.45.04-.69.1-.86.2-.4.19-.74.53-.94.93-.1.16-.15.41-.2.86l-.03 1.9v5.4l.04 1.9c.04.45.1.69.2.86a2.09 2.09 0 0 0 .93.93c.17.1.41.16.86.2l1.9.04h4.9l1.9-.04c.45-.04.69-.1.86-.2a2.09 2.09 0 0 0 .93-.93c.1-.17.16-.41.2-.86l.04-1.9V9.3zm1.76 2.7c0 .4.16.8.45 1.09l3.58 3.58.06.04h.08l.06-.05c0-.01.02-.03.02-.07V7.4c0-.04-.02-.06-.02-.07l-.06-.03-.08-.01s-.03 0-.06.04l-3.58 3.58c-.3.29-.45.68-.45 1.09z"/></svg>');
      }

      .msg-incoming .shared-contact-avatar-default {
        filter: var(--shared-contact-incoming-f);
      }

      .msg-outgoing .shared-contact-avatar-default {
        filter: var(--shared-contact-outgoing-f);
      }

      .call-link-info,
      .shared-contact-info {
        margin-left: 5px;
      }

      .call-link,
      .shared-contact {
        display: flex;
        flex-direction: row;
      }

      .call-link-title,
      .shared-contact-name {
        font-weight: bold;
        margin-bottom: 5px;
        display: block;
      }

      .call-link pre,
      .shared-contact pre {
        font-size: small;
      }

      .msg p {
        margin-top: 0;
        margin-bottom: 5px;
        display: block;
      }

      .msg img, .msg video {
        max-width: 100%;
        max-height: 400px;
      }

      img {
        image-orientation: from-image;
      }

      audio {
        max-width: 100%;
        width: 400px;
      }

      .shared-contact-avatar input[type=checkbox],
      .msg-img-container input[type=checkbox],
      .msg-linkpreview-img-container input[type=checkbox],
      .thread-subtitle input[type=checkbox] {
        display: none;
      }

      .msg-img-container,
      .msg-vid-container {
        text-align: center;
        padding-bottom: 5px;
      }

      .msg-vid-container video {
        display: flex;
      }

      .msg-img-container label {
        display: flex;
        flex-direction: column;
        margin-bottom: 0px;
        align-items: flex-start;
      }

      .msg-quote-attach label {
        align-items: flex-end;
      }

      .caption {
        font-size: small;
      }

      .shared-contact-avatar img,
      .msg-img-container img {
        cursor: zoom-in;
        z-index: 1;
        position: relative;
        transition: z-index, transform .25s ease;
        transition-delay: .25s, 0s;
      }

      .msg-img-container img {
        border-radius: 0.6em;
      }

      .shared-contact-avatar img {
        border-radius: 50%;
      }

      .msg-linkpreview-img-container img {
        border-top-left-radius: 0.6em;
        border-top-right-radius: 0.6em;
        border-bottom-left-radius: 0em;
        border-bottom-right-radius: 0em;
        cursor: zoom-in;
        z-index: 1;
        position: relative;
        transition: z-index, transform .25s ease;
        transition-delay: .25s, 0s;
      }

      .msg-linkpreview-img-container {
        border-top-left-radius: 0.6em;
        border-top-right-radius: 0.6em;
        border-bottom-left-radius: 0em;
        border-bottom-right-radius: 0em;
      }

      .linkpreview {
        padding: 5px;
        margin-bottom: 5px;
      }

      .linkpreview_title {
        font-weight: 550;
      }

      .linkpreview_description {
      }

      .shared-contact-avatar input[type=checkbox]:checked ~ label > img,
      .msg-img-container input[type=checkbox]:checked ~ label > img,
      .msg-linkpreview-img-container input[type=checkbox]:checked ~ label > img {
        border-radius: 0;
        cursor: zoom-out;
        z-index: 2;
        position: relative;
        transition: transform .25s ease;
      }

      .msg-img-container input[type=checkbox]:checked ~ label > img,
      .msg-linkpreview-img-container input[type=checkbox]:checked ~ label > img {
        transform: scale(2.5);
      }

      .shared-contact-avatar input[type=checkbox]:checked ~ label > img {
        transform: scale(10);
      }

      .pending-attachment {
        padding: 5px;
        margin: 5px;
        text-align: center;
        border: 1px dashed;
      }

      .attachment-unknown-type {
        border-radius: .6em;
        padding-top: 6px;
        padding-bottom: 6px;
        padding-left: 9px;
        padding-right: 9px;
        margin-bottom: 5px;
        display: flex;
        align-items: center;
      }

      .msg-incoming .attachment-unknown-type {
        background-color: var(--incominglinkpreview-bc);
      }

      .msg-outgoing .attachment-unknown-type {
        background-color: rgba(0, 0, 0, 0.244);
      }

      .document-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 113 161"><path d="M112.5 40.5v105a14.97 14.97 0 0 1-15 15h-82a14.97 14.97 0 0 1-15-15v-130a14.97 14.97 0 0 1 15-15h57" fill="%23f9f9f9" stroke="%23aaaaaa"/><path d="M112.5 40.5h-25a14.97 14.97 0 0 1-15-15V.5z" fill="%23fbfbfb" stroke="%23bbbbbb"/></svg>');
        display: inline-block;
        height: 75px;
        aspect-ratio: 112 / 160;
        margin-right: 10px;
        margin-top: 3px;
        margin-bottom: 3px;
        color: black;
        text-align: center;
        vertical-align: middle;
      }

      .document-ext {
        position: relative;
        top: calc(50% - 0.5em);
        font-variant: all-small-caps;
        font-weight: bold;
      }

      .msg-with-reaction {
        margin-bottom: 20px;
        padding-bottom: 15px;
      }

      .msg-reactions {
        margin-top: 5px;
        text-align: center;
        position: absolute;
        display: flex;
      }

      .msg-incoming .msg-reactions {
        right: 10px;
      }

      .msg-outgoing .msg-reactions {
        left: 10px;
      }

      .msg-reaction {
        padding-left: 4px;
        padding-right: 4px;
        padding-bottom: 1.5px;
        background-color: var(--msgreaction-bc);
        border-radius: 13px;
        border: 1px solid var(--msgreaction-border);
        line-height: 150%;
        position: relative;
        color: var(--msgreaction-c);
      }

      .reaction-count {
        color: var(--reactioncount-c);
        margin-left: 5px;
      }

      .msg-receipt-info,
      .edited-info,
      .msg-reaction .msg-reaction-info {
        display: block;
        position: absolute;
        z-index: 1;
        background-color: var(--msgreactioninfo-bc);
        border: 1px solid var(--msgreactioninfo-border);
        padding: 5px;
        border-radius: 6px;
        visibility: hidden;
        opacity: 0;
        transition: opacity 0.2s;
      }

      .edited-info,
      .msg-reaction .msg-reaction-info {
        bottom: 36px;
        left: -118px;
        width: 250px;
      }

      .msg-receipt-info {)";

  file <<
    "\n"
    "        width: " << (isgroup ? "270" : "210") << "px;"
    "\n"
    "        right: calc(((" << (isgroup ? "270" : "210") << "px / 2) * -1) + 15px);";

  file << R"(
        font-size: small;
        bottom: 33px;
      }

      .msg-receipt-info,
      .msg-reaction .msg-reaction-info {
        text-align: center;
      }

      .edited-info {
        font-size: initial;
        text-align: inherit;
        width: 300px;
      }

      .edited-info > .msg {
        max-width: 100%;
      }

      /* Draw an arrow using border styles */
      .msg-receipt-info::before,
      .edited-info::before,
      .msg-reaction .msg-reaction-info::before {
        content: "";
        position: absolute;
        top: calc(100% - 5px);
        left: calc(50% - 5px);
        width: 10px;
        height: 10px;
        transform: rotate(45deg);
        background-color: var(--msgreactioninfo-bc);
        border-bottom: 1px solid var(--msgreactioninfo-border);
        border-right: 1px solid var(--msgreactioninfo-border);
      }

      .footer-icons:hover,
      .footer-icons:hover .msg-receipt-info,
      .edited:hover,
      .edited:hover .edited-info,
      .msg-reaction:hover .msg-reaction-info {
        visibility: visible;
        opacity: 1;
      }

      .msg-quote {
        display: flex;
        width: 98%;
        padding: 5px 0px 5px 0px;
        border-radius: .3em;
        margin-bottom: 5px;
        margin-right: 10px;
        justify-content: space-between;
        border-left: 5px solid #FFFFFF;
      }

      .msg-incoming .msg-quote,
      .msg-incoming .msg-linkpreview-img-container,
      .msg-incoming .call-link,
      .msg-incoming .linkpreview {
        background-color: var(--incominglinkpreview-bc);
      }

      .msg-outgoing .msg-quote,
      .msg-outgoing .msg-linkpreview-img-container,
      .msg-outgoing .call-link,
      .msg-outgoing .linkpreview {
        background-color: var(--outgoinglinkpreview-bc);
        color: #000000;
      }

      .msg-quote-message {
        padding-left: 5px;
      }

      .msg-quote-attach {
        flex-grow: 1;
        min-width: 30%;
        max-width: 30%;
        margin-right: 5px;
        text-align: right;
      }

      .msg-quote-attach .msg-img-container input[type=checkbox]:checked ~ label > img {
        transform: scale(5);
      }
      .msg-quote-attach img {
        max-height: 5em;
      }

      .msg-outgoing .msg-data {
        text-align: right;
      }

      .msg-outgoing .msg-reactions {
        text-align: left;
      }

      .msg-status {
        background: none;
        align-self: center;
      }

      .msg-status div {
        background: none;
        text-align: center;
      }

      .msg-status {
        max-width: 70%;
      }

      .msg-status div.status-text {
        align-items: center;
        display: flex;
      }

      .msg-status div.status-text-red {
        color: #FF0000;
      }

      .status-text pre {
        margin-left: auto;
        margin-right: auto;
      }

      .thread-disappearing-messages-info {
        display: inline-block;
        height: 18px;
        aspect-ratio: 1 / 1;
        top: 2px;
        margin-left: 9px;
        margin-right: 2px;
        position: relative;
        background-image: url('data:image/svg+xml;utf-8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 12 12" fill="white" stroke="none"><path d="M6.75 6a.75.75 0 0 1-1.5 0c0-.414.475-3.581.5-3.75S5.862 2 6 2s.226.087.25.25L6.75 6zM12 6a6 6 0 1 0-6 6 6.006 6.006 0 0 0 6-6zm-1 0a5 5 0 1 1-5-5 5.006 5.006 0 0 1 5 5z"/></svg>');
        filter: var(--icon-f);
      }

      .msg-status .msg-video-call-missed {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 14.117647 14.117647" stroke="none" fill="red"><path d="m 13.288805,3.2188235 c -0.2276,-0.097 -0.4791,-0.1231 -0.7217,-0.0749 -0.2426,0.0482 -0.465,0.1684 -0.6383,0.3449 l -1.87,1.86 v -1.54 c -0.0026,-0.4633 -0.1877997,-0.9069 -0.5154997,-1.2345 -0.3276,-0.3277 -0.7712,-0.5129 -1.2345,-0.5155 h -6.5 c -0.4633,0.0026 -0.90689998,0.1878 -1.23449998,0.5155 -0.3277,0.3276 -0.5129,0.7712 -0.5155,1.2345 v 6.5000005 c 0.0026,0.4633 0.1878,0.9069 0.5155,1.2345 0.3276,0.3277 0.77119998,0.5129 1.23449998,0.5155 h 6.5 c 0.4633,-0.0026 0.9069,-0.1878 1.2345,-0.5155 0.3277,-0.3276 0.5128997,-0.7712 0.5154997,-1.2345 V 8.7688235 l 1.87,1.8600005 c 0.1146,0.1172 0.2515,0.2103 0.4026,0.2739 0.1512,0.0635 0.3135,0.0962 0.4774,0.0961 0.1652,6e-4 0.3288,-0.0334 0.48,-0.1 0.2289,-0.0923 0.4248,-0.2513 0.5621,-0.4564 0.1373,-0.2051 0.2098,-0.4468005 0.2079,-0.6936005 v -5.38 c 0.0019,-0.2468 -0.0706,-0.4885 -0.2079,-0.6936 -0.1373,-0.2051 -0.3332,-0.3641 -0.5621,-0.4564 z M 9.0588053,10.308824 c -0.0026,0.1981 -0.0824,0.3874 -0.2225,0.5275 -0.1401,0.1401 -0.3294,0.2199 -0.5275,0.2225 h -6.5 c -0.1981,-0.0026 -0.3874,-0.0824 -0.5275,-0.2225 -0.1401,-0.1401 -0.2199,-0.3294 -0.2225,-0.5275 V 3.8088235 c 0.0026,-0.1981 0.0824,-0.3874 0.2225,-0.5275 0.1401,-0.1401 0.3294,-0.2199 0.5275,-0.2225 h 6.5 c 0.1981,0.0026 0.3874,0.0824 0.5275,0.2225 0.1401,0.1401 0.2199,0.3294 0.2225,0.5275 z M 13.058805,9.7488235 c 2e-4,0.0488 -0.0139,0.0966 -0.0406,0.1374 -0.0267,0.0409 -0.0647,0.0731 -0.1094,0.0926 -0.0465,0.0198 -0.0977,0.0256 -0.1474,0.0167 -0.0498,-0.0089 -0.0958,-0.0321 -0.1326,-0.0667 l -2.57,-2.58 v -0.58 l 2.57,-2.58 c 0.0418,-0.0267 0.0904,-0.0409 0.14,-0.0409 0.0496,0 0.0982,0.0142 0.14,0.0409 0.0447,0.0195 0.0827,0.0517 0.1094,0.0926 0.0267,0.0408 0.0408,0.0886 0.0406,0.1374 z m -7.2899997,-2.69 2.14,2.15 -0.7,0.7 -2.15,-2.14 -2.15,2.14 -0.7,-0.7 2.14,-2.15 -2.14,-2.15 0.7,-0.7 2.15,2.14 2.15,-2.14 0.7,0.7 z"/></svg>');
        display: inline-block;
        height: 18px;
        aspect-ratio: 1 / 1;
        margin-right: 10px;
        margin-bottom: 5px;
      }

      .msg-status .msg-video-call-incoming {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 14.117647 14.117647" stroke="none" fill="white"><path d="m 13.288805,3.2188234 c -0.2276,-0.097 -0.4791,-0.1231 -0.7217,-0.0749 -0.2426,0.0482 -0.465,0.1684 -0.6383,0.3449 l -1.87,1.86 v -1.54 c -0.0026,-0.4633 -0.1877996,-0.9069 -0.5154999,-1.2345 -0.3276,-0.3277 -0.7712,-0.5129 -1.2345,-0.5155 h -6.5 c -0.4633,0.0026 -0.90689997,0.1878 -1.23449997,0.5155 -0.32769998,0.3276 -0.51289998,0.7712 -0.51549998,1.2345 v 6.5000006 c 0.0026,0.4633 0.1878,0.9069 0.51549998,1.2345 0.3276,0.3277 0.77119997,0.5129 1.23449997,0.5155 h 6.5 c 0.4633,-0.0026 0.9069,-0.1878 1.2345,-0.5155 0.3277003,-0.3276 0.5128999,-0.7712 0.5154999,-1.2345 V 8.7688234 l 1.87,1.8600006 c 0.1146,0.1172 0.2515,0.2103 0.4026,0.2739 0.1512,0.0635 0.3135,0.0962 0.4774,0.0961 0.1652,6e-4 0.3288,-0.0334 0.48,-0.1 0.2289,-0.0923 0.4248,-0.2513 0.5621,-0.4564 0.1373,-0.2051 0.2098,-0.4468003 0.2079,-0.6936003 V 4.3688234 c 0.0019,-0.2468 -0.0706,-0.4885 -0.2079,-0.6936 -0.1373,-0.2051 -0.3332,-0.3641 -0.5621,-0.4564 z M 9.0588051,10.308824 c -0.0026,0.1981 -0.0824,0.3874 -0.2225,0.5275 -0.1401,0.1401 -0.3294,0.2199 -0.5275,0.2225 h -6.5 c -0.1981,-0.0026 -0.3874,-0.0824 -0.5275,-0.2225 -0.1401,-0.1401 -0.2199,-0.3294 -0.2225,-0.5275 V 3.8088234 c 0.0026,-0.1981 0.0824,-0.3874 0.2225,-0.5275 0.1401,-0.1401 0.3294,-0.2199 0.5275,-0.2225 h 6.5 c 0.1981,0.0026 0.3874,0.0824 0.5275,0.2225 0.1401,0.1401 0.2199,0.3294 0.2225,0.5275 z M 13.058805,9.7488237 c 2e-4,0.0488 -0.0139,0.0966 -0.0406,0.1374 -0.0267,0.0409 -0.0647,0.0731 -0.1094,0.0926 -0.0465,0.0198 -0.0977,0.0256 -0.1474,0.0167 -0.0498,-0.0089 -0.0958,-0.0321 -0.1326,-0.0667 l -2.57,-2.5800003 v -0.58 l 2.57,-2.58 c 0.0418,-0.0267 0.0904,-0.0409 0.14,-0.0409 0.0496,0 0.0982,0.0142 0.14,0.0409 0.0447,0.0195 0.0827,0.0517 0.1094,0.0926 0.0267,0.0408 0.0408,0.0886 0.0406,0.1374 z M 3.7688051,9.0588234 h 3.29 v 1.0000006 h -5 V 5.0588234 h 1 v 3.29 l 4.15,-4.14 0.7,0.7 z"/></svg>');
        filter: var(--icon-f);
      }

      .msg-status .msg-video-call-outgoing {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 14.117647 14.117647" stroke="none" fill="white"><path d="m 13.288805,3.2188235 c -0.2276,-0.097 -0.4791,-0.1231 -0.7217,-0.0749 -0.2426,0.0482 -0.465,0.1684 -0.6383,0.3449 l -1.87,1.86 v -1.54 c -0.0026,-0.4633 -0.1877997,-0.9069 -0.5154997,-1.2345 -0.3276,-0.3277 -0.7712,-0.5129 -1.2345,-0.5155 h -6.5 c -0.4633,0.0026 -0.90689998,0.1878 -1.23449998,0.5155 -0.3277,0.3276 -0.5129,0.7712 -0.5155,1.2345 v 6.5000005 c 0.0026,0.4633 0.1878,0.9069 0.5155,1.2345 0.3276,0.3277 0.77119998,0.5129 1.23449998,0.5155 h 6.5 c 0.4633,-0.0026 0.9069,-0.1878 1.2345,-0.5155 0.3277,-0.3276 0.5128997,-0.7712 0.5154997,-1.2345 V 8.7688235 l 1.87,1.8600005 c 0.1146,0.1172 0.2515,0.2103 0.4026,0.2739 0.1512,0.0635 0.3135,0.0962 0.4774,0.0961 0.1652,6e-4 0.3288,-0.0334 0.48,-0.1 0.2289,-0.0923 0.4248,-0.2513 0.5621,-0.4564 0.1373,-0.2051 0.2098,-0.4468005 0.2079,-0.6936005 v -5.38 c 0.0019,-0.2468 -0.0706,-0.4885 -0.2079,-0.6936 -0.1373,-0.2051 -0.3332,-0.3641 -0.5621,-0.4564 z M 9.0588053,10.308824 c -0.0026,0.1981 -0.0824,0.3874 -0.2225,0.5275 -0.1401,0.1401 -0.3294,0.2199 -0.5275,0.2225 h -6.5 c -0.1981,-0.0026 -0.3874,-0.0824 -0.5275,-0.2225 -0.1401,-0.1401 -0.2199,-0.3294 -0.2225,-0.5275 V 3.8088235 c 0.0026,-0.1981 0.0824,-0.3874 0.2225,-0.5275 0.1401,-0.1401 0.3294,-0.2199 0.5275,-0.2225 h 6.5 c 0.1981,0.0026 0.3874,0.0824 0.5275,0.2225 0.1401,0.1401 0.2199,0.3294 0.2225,0.5275 z M 13.058805,9.7488235 c 2e-4,0.0488 -0.0139,0.0966 -0.0406,0.1374 -0.0267,0.0409 -0.0647,0.0731 -0.1094,0.0926 -0.0465,0.0198 -0.0977,0.0256 -0.1474,0.0167 -0.0498,-0.0089 -0.0958,-0.0321 -0.1326,-0.0667 l -2.57,-2.58 v -0.58 l 2.57,-2.58 c 0.0418,-0.0267 0.0904,-0.0409 0.14,-0.0409 0.0496,0 0.0982,0.0142 0.14,0.0409 0.0447,0.0195 0.0827,0.0517 0.1094,0.0926 0.0267,0.0408 0.0408,0.0886 0.0406,0.1374 z m -4.9999997,-5.69 v 5 h -1 v -3.29 l -4.15,4.14 -0.7,-0.7 4.14,-4.15 h -3.29 v -1 z"/></svg>');
        filter: var(--icon-f);
      }

      .msg-status .msg-group-call {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 16 16" stroke="none" fill="white"><path d="M14.23,4.16a1.23,1.23 0,0 0,-1.36 0.27L11,6.29L11,4.75A1.76,1.76 0,0 0,9.25 3L2.75,3A1.76,1.76 0,0 0,1 4.75v6.5A1.76,1.76 0,0 0,2.75 13h6.5A1.76,1.76 0,0 0,11 11.25L11,9.71l1.87,1.86a1.23,1.23 0,0 0,0.88 0.37,1.18 1.18,0 0,0 0.48,-0.1A1.23,1.23 0,0 0,15 10.69L15,5.31A1.23,1.23 0,0 0,14.23 4.16ZM10,11.25a0.76,0.76 0,0 1,-0.75 0.75L2.75,12A0.76,0.76 0,0 1,2 11.25L2,4.75A0.76,0.76 0,0 1,2.75 4h6.5a0.76,0.76 0,0 1,0.75 0.75ZM14,10.69a0.25,0.25 0,0 1,-0.15 0.23,0.26 0.26,0 0,1 -0.28,-0.05L11,8.29L11,7.71l2.57,-2.58a0.26,0.26 0,0 1,0.28 0,0.25 0.25,0 0,1 0.15,0.23Z"/></svg>');
        filter: var(--icon-f);
      }

      .msg-status .msg-call-incoming {
        background-image: url('data:image/svg+xml;utf-8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke="white" stroke-width="2" ><polyline points="16 2 16 8 22 8"></polyline><line x1="23" y1="1" x2="16" y2="8"></line><path d="M22 16.92v3a2 2 0 0 1-2.18 2 19.79 19.79 0 0 1-8.63-3.07 19.5 19.5 0 0 1-6-6 19.79 19.79 0 0 1-3.07-8.67A2 2 0 0 1 4.11 2h3a2 2 0 0 1 2 1.72 12.84 12.84 0 0 0 .7 2.81 2 2 0 0 1-.45 2.11L8.09 9.91a16 16 0 0 0 6 6l1.27-1.27a2 2 0 0 1 2.11-.45 12.84 12.84 0 0 0 2.81.7A2 2 0 0 1 22 16.92z"/></svg>');
        filter: var(--icon-f);
      }

      .msg-status .msg-call-missed {
        background-image: url('data:image/svg+xml;utf-8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke="red" stroke-width="2"><line x1="23" y1="1" x2="17" y2="7"></line><line x1="17" y1="1" x2="23" y2="7"></line><path d="M22 16.92v3a2 2 0 0 1-2.18 2 19.79 19.79 0 0 1-8.63-3.07 19.5 19.5 0 0 1-6-6 19.79 19.79 0 0 1-3.07-8.67A2 2 0 0 1 4.11 2h3a2 2 0 0 1 2 1.72 12.84 12.84 0 0 0 .7 2.81 2 2 0 0 1-.45 2.11L8.09 9.91a16 16 0 0 0 6 6l1.27-1.27a2 2 0 0 1 2.11-.45 12.84 12.84 0 0 0 2.81.7A2 2 0 0 1 22 16.92z"/></svg>');
      }

      .msg-status .msg-call-outgoing {
        background-image: url('data:image/svg+xml;utf-8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke="white" stroke-width="2"><polyline points="23 7 23 1 17 1"></polyline><line x1="16" y1="8" x2="23" y2="1"></line><path d="M22 16.92v3a2 2 0 0 1-2.18 2 19.79 19.79 0 0 1-8.63-3.07 19.5 19.5 0 0 1-6-6 19.79 19.79 0 0 1-3.07-8.67A2 2 0 0 1 4.11 2h3a2 2 0 0 1 2 1.72 12.84 12.84 0 0 0 .7 2.81 2 2 0 0 1-.45 2.11L8.09 9.91a16 16 0 0 0 6 6l1.27-1.27a2 2 0 0 1 2.11-.45 12.84 12.84 0 0 0 2.81.7A2 2 0 0 1 22 16.92z"/></svg>');
        transform: scale(-1, 1);
        filter: var(--icon-f);
      }

      .msg-status .msg-info-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="white" stroke="none"><path d="M12,2.5A9.5,9.5 0,1 1,2.5 12,9.511 9.511,0 0,1 12,2.5M12,1A11,11 0,1 0,23 12,11 11,0 0,0 12,1ZM12,8.5A1.5,1.5 0,0 0,13.5 7a1.5,1.5 0,1 0,-2.56 1.06A1.435,1.435 0,0 0,12 8.5ZM13,16.5L13,10L9.5,10v1.5h2v5L9,16.5L9,18h6L15,16.5Z"/></svg>');
        filter: var(--icon-f);
      }

      .thread-subtitle .msg-security-icon,
      .msg-status .msg-security-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="white" stroke="none"><path d="M21.793,7.888A19.35,19.35 0,0 1,12 23C7.6,20.4 2,15.5 2,4.5 9,4.5 12,1 12,1s2.156,2.5 7.05,3.268L17.766,5.553A14.7,14.7 0,0 1,12 3,15.653 15.653,0 0,1 3.534,5.946c0.431,8.846 4.8,12.96 8.458,15.29A17.39,17.39 0,0 0,19.983 9.7ZM22.53,5.03 L21.47,3.97 12,13.439 8.53,9.97 7.47,11.03 12,15.561Z"/></svg>');
        filter: var(--icon-f);
      }

      .msg-status .msg-pencil-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="white" stroke="none"><path d="M21.561,4.561 L19.439,2.439a1.5,1.5 0,0 0,-2.121 0L3.823,15.934a1.5,1.5 0,0 0,-0.394 0.7L2.317,21.076a0.5,0.5 0,0 0,0.607 0.607l4.445,-1.112a1.5,1.5 0,0 0,0.7 -0.394l13.5,-13.495A1.5,1.5 0,0 0,21.561 4.561ZM7.005,19.116l-2.828,0.707L4.884,17l9.772,-9.773 2.122,2.122ZM17.838,8.283 L15.717,6.162 18.379,3.5 20.5,5.621Z"/></svg>');
        filter: var(--icon-f);
      }

      .msg-status .msg-thread-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 16 16" fill="white" stroke="none"><path d="M5.33 2.32a3.35 3.35 0 0 0-3.35 3.35c0 .82.3 1.57.79 2.15a.99.99 0 0 1 .23.73l-.1 1.2.95-.65a1.01 1.01 0 0 1 .78-.16l.29.06h0a5.51 5.51 0 0 0 .15 1.3c-.2 0-.4-.03-.58-.06l-1.4.97c-.68.47-1.6-.06-1.54-.9l.15-1.73a4.61 4.61 0 0 1-1.02-2.9c0-2.57 2.09-4.65 4.65-4.65A4.64 4.64 0 0 1 9.39 3.4c-.44.1-.86.25-1.25.44a3.35 3.35 0 0 0-2.8-1.51zm5.34 2.03c2.56 0 4.65 2.08 4.65 4.65a4.61 4.61 0 0 1-1.01 2.89l.15 1.8a.99.99 0 0 1-1.54.9l-1.45-1.01a4.25 4.25 0 0 1-.8.07 4.65 4.65 0 1 1 0-9.3zM14.02 9a3.35 3.35 0 1 0-3.35 3.35 2.99 2.99 0 0 0 .67-.07 1.03 1.03 0 0 1 .77.16l1 .7-.1-1.27c-.03-.27.06-.53.23-.72A3.37 3.37 0 0 0 14.02 9z"/></svg>');
        filter: var(--icon-f);
      }

      .msg-status .msg-megaphone-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 16 16" fill="white" stroke="none"><path d="M14.117,2C14.0399,2.0002 13.9658,2.0294 13.9094,2.0818L10.7086,5.0142H3.2721C2.9434,5.0142 2.6281,5.1448 2.3957,5.3773C2.1633,5.6097 2.0327,5.925 2.0327,6.2537V9.3522C2.0327,9.6809 2.1633,9.9962 2.3957,10.2286C2.6281,10.461 2.9434,10.5916 3.2721,10.5916H10.7086L13.9075,13.5241C13.9639,13.5765 14.0381,13.6057 14.1151,13.6059C14.1973,13.6059 14.2761,13.5732 14.3342,13.5151C14.3923,13.457 14.425,13.3782 14.425,13.296V2.3105C14.4251,2.2285 14.3928,2.1498 14.3351,2.0916C14.2774,2.0334 14.1989,2.0005 14.117,2ZM13.6522,12.2958L12.7784,11.2281L11.0699,9.6621H3.2721C3.1899,9.6621 3.1111,9.6294 3.053,9.5713C2.9949,9.5132 2.9622,9.4344 2.9622,9.3522V6.2537C2.9622,6.1715 2.9949,6.0927 3.053,6.0346C3.1111,5.9765 3.1899,5.9438 3.2721,5.9438H11.0699L12.7784,4.3778L13.6522,3.3101L13.4973,5.0142V10.5916L13.6522,12.2958Z"/><path d="M5.7509,5.3241h0.9296v8.6759h-0.9296z"/></svg>');
        filter: var(--icon-f);
      }

      .msg-status .msg-member-add-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 16 16" fill="white" stroke="none"><path d="M12.2831,8.4211C12.699,7.9623 12.9758,7.3883 13.0794,6.7696C13.1831,6.1509 13.1091,5.5145 12.8665,4.9385C12.6239,4.3626 12.2234,3.8723 11.7141,3.5279C11.2049,3.1835 10.6091,3 10,3C9.3909,3 8.7951,3.1835 8.2859,3.5279C7.7766,3.8723 7.3761,4.3626 7.1335,4.9385C6.8909,5.5145 6.8169,6.1509 6.9206,6.7696C7.0242,7.3883 7.301,7.9623 7.7169,8.4211C6.9461,8.6021 6.2577,9.0474 5.7642,9.684C5.2707,10.3207 5.0013,11.111 5,11.926V13H5.9375V11.926C5.9383,11.2285 6.2074,10.5599 6.6858,10.0668C7.1642,9.5736 7.8128,9.2962 8.4894,9.2954H11.5106C12.1872,9.2962 12.8358,9.5736 13.3142,10.0668C13.7926,10.5599 14.0617,11.2285 14.0625,11.926V13H15V11.926C14.9987,11.111 14.7293,10.3207 14.2358,9.684C13.7423,9.0474 13.0539,8.6021 12.2831,8.4211ZM10,8.49C9.5674,8.49 9.1444,8.3577 8.7847,8.11C8.425,7.8622 8.1446,7.51 7.979,7.0979C7.8134,6.6859 7.7701,6.2325 7.8545,5.7951C7.9389,5.3576 8.1473,4.9558 8.4532,4.6405C8.7591,4.3251 9.1489,4.1103 9.5732,4.0233C9.9976,3.9363 10.4374,3.981 10.8371,4.1516C11.2368,4.3223 11.5785,4.6113 11.8188,4.9822C12.0592,5.353 12.1875,5.789 12.1875,6.235C12.1875,6.833 11.957,7.4066 11.5468,7.8295C11.1366,8.2524 10.5802,8.49 10,8.49Z"/><path d="M5,6.8H3.2V5H2.8V6.8H1V7.2H2.8V9H3.2V7.2H5V6.8Z" stroke="white" stroke-width="0.5"/></svg>');
        filter: var(--icon-f);
      }

      .msg-status .msg-member-remove-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 16 16" fill="white" stroke="none"><path d="M12.2831,8.4211C12.699,7.9623 12.9758,7.3883 13.0794,6.7696C13.1831,6.1509 13.1091,5.5145 12.8665,4.9385C12.6239,4.3626 12.2234,3.8723 11.7141,3.5279C11.2049,3.1835 10.6091,3 10,3C9.3909,3 8.7951,3.1835 8.2859,3.5279C7.7766,3.8723 7.3761,4.3626 7.1335,4.9385C6.8909,5.5145 6.8169,6.1509 6.9206,6.7696C7.0242,7.3883 7.301,7.9623 7.7169,8.4211C6.9461,8.6021 6.2577,9.0474 5.7642,9.684C5.2707,10.3207 5.0013,11.111 5,11.926V13H5.9375V11.926C5.9383,11.2285 6.2074,10.5599 6.6858,10.0668C7.1642,9.5736 7.8128,9.2962 8.4894,9.2954H11.5106C12.1872,9.2962 12.8358,9.5736 13.3142,10.0668C13.7926,10.5599 14.0617,11.2285 14.0625,11.926V13H15V11.926C14.9987,11.111 14.7293,10.3207 14.2358,9.684C13.7423,9.0474 13.0539,8.6021 12.2831,8.4211ZM10,8.49C9.5674,8.49 9.1444,8.3577 8.7847,8.11C8.425,7.8622 8.1446,7.51 7.979,7.0979C7.8134,6.6859 7.7701,6.2325 7.8545,5.7951C7.9389,5.3576 8.1473,4.9558 8.4532,4.6405C8.7591,4.3251 9.1489,4.1103 9.5732,4.0233C9.9976,3.9363 10.4374,3.981 10.8371,4.1516C11.2368,4.3223 11.5785,4.6113 11.8188,4.9822C12.0592,5.353 12.1875,5.789 12.1875,6.235C12.1875,6.833 11.957,7.4066 11.5468,7.8295C11.1366,8.2524 10.5802,8.49 10,8.49Z"/><path d="M4.5,6.8H2.925V7.05H2.575V6.8H1V7.3H2.575V7.05H2.925V7.3H4.5V6.8Z"/><path d="M2.925,7.05V6.8H4.5V7.3H2.925V7.05ZM2.925,7.05H2.575M2.575,7.05V6.8H1V7.3H2.575V7.05Z" stroke="white" stroke-width="0.5"/></svg>');
        filter: var(--icon-f);
      }

      .msg-status .msg-avatar-update-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="white" stroke="none"><path d="M17.5,2L6.5,2A4.5,4.5 0,0 0,2 6.5v11A4.5,4.5 0,0 0,6.5 22h11A4.5,4.5 0,0 0,22 17.5L22,6.5A4.5,4.5 0,0 0,17.5 2ZM6.5,3.5h11a3,3 0,0 1,3 3v6.75l-0.621,-0.932L16,8.439l-3,3 -4,-4L4.121,12.318 3.5,13.25L3.5,6.5A3,3 0,0 1,6.5 3.5ZM17.5,20.5L6.5,20.5a3,3 0,0 1,-3 -3L3.5,15.061L9,9.561l5.97,5.969 1.06,-1.06L14.061,12.5 16,10.561l4.5,4.5L20.5,17.5A3,3 0,0 1,17.5 20.5Z"/></svg>');
        filter: var(--icon-f);
      }

      .msg-status .msg-group-quit-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 16 16" fill="white" stroke="none"><path d="M6,9.5V13H5V9.5H6ZM11,2H7C6.4696,2 5.9609,2.2107 5.5858,2.5858C5.2107,2.9609 5,3.4696 5,4V6.5H6V4C6,3.7348 6.1054,3.4804 6.2929,3.2929C6.4804,3.1054 6.7348,3 7,3H11C11.2652,3 11.5196,3.1054 11.7071,3.2929C11.8946,3.4804 12,3.7348 12,4V13H13V4C13,3.4696 12.7893,2.9609 12.4142,2.5858C12.0391,2.2107 11.5304,2 11,2ZM7.957,4.979L7.257,5.689L8.671,7.1L9.371,7.5H2V8.5H9.375L8.675,8.9L7.252,10.311L7.957,11.021L11,8L7.957,4.979Z"/></svg>');
        filter: var(--icon-f);
      }

      .msg-status .msg-members-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 40 40" fill="white" stroke="white" stroke-width="0.5"><path d="M29,16.75a6.508,6.508 0,0 1,6.5 6.5L35.5,24L37,24v-0.75a8,8 0,0 0,-6.7 -7.885,6.5 6.5,0 1,0 -8.6,0 7.941,7.941 0,0 0,-2.711 0.971A6.5,6.5 0,1 0,9.7 25.365,8 8,0 0,0 3,33.25L3,34L4.5,34v-0.75a6.508,6.508 0,0 1,6.5 -6.5h6a6.508,6.508 0,0 1,6.5 6.5L23.5,34L25,34v-0.75a8,8 0,0 0,-6.7 -7.885,6.468 6.468,0 0,0 1.508,-7.771A6.453,6.453 0,0 1,23 16.75ZM14,25.5a5,5 0,1 1,5 -5A5,5 0,0 1,14 25.5ZM21,10.5a5,5 0,1 1,5 5A5,5 0,0 1,21 10.5Z"/></svg>');
        filter: var(--icon-f);
      }

      .msg-status .msg-member-approved-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 16 16" fill="white" stroke="none"><path d="M13.283,8.4211C13.6989,7.9623 13.9756,7.3883 14.0793,6.7696C14.1829,6.1509 14.1089,5.5145 13.8664,4.9385C13.6238,4.3626 13.2233,3.8723 12.714,3.5279C12.2047,3.1835 11.6089,3 10.9999,3C10.3908,3 9.795,3.1835 9.2857,3.5279C8.7765,3.8723 8.3759,4.3626 8.1334,4.9385C7.8908,5.5145 7.8168,6.1509 7.9204,6.7696C8.0241,7.3883 8.3009,7.9623 8.7167,8.4211C7.946,8.6021 7.2576,9.0474 6.7641,9.684C6.2706,10.3207 6.0012,11.111 5.9999,11.926V13H6.9374V11.926C6.9382,11.2285 7.2073,10.5599 7.6857,10.0668C8.1641,9.5736 8.8127,9.2962 9.4892,9.2954H12.5105C13.187,9.2962 13.8356,9.5736 14.314,10.0668C14.7924,10.5599 15.0615,11.2285 15.0624,11.926V13H15.9999V11.926C15.9986,11.111 15.7291,10.3207 15.2356,9.684C14.7421,9.0474 14.0538,8.6021 13.283,8.4211ZM10.9999,8.49C10.5672,8.49 10.1443,8.3577 9.7845,8.11C9.4248,7.8622 9.1444,7.51 8.9789,7.0979C8.8133,6.6859 8.77,6.2325 8.8544,5.7951C8.9388,5.3576 9.1471,4.9558 9.4531,4.6405C9.759,4.3251 10.1488,4.1103 10.5731,4.0233C10.9974,3.9363 11.4373,3.981 11.837,4.1516C12.2367,4.3223 12.5783,4.6113 12.8187,4.9822C13.0591,5.353 13.1874,5.789 13.1874,6.235C13.1874,6.833 12.9569,7.4066 12.5467,7.8295C12.1364,8.2524 11.58,8.49 10.9999,8.49ZM5.9749,5.2322L6.682,5.9394L3.1465,9.4749L3.1465,9.4749L2.4394,10.182L2.4394,10.182L1.7323,9.4749L0.3181,8.0607L1.0252,7.3536L2.4394,8.7678L5.9749,5.2322Z"/></svg>');
        filter: var(--icon-f);
      }

      .msg-status .msg-member-rejected-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 16 16" fill="white" stroke="none"><path d="M12.2831,8.4211C12.699,7.9623 12.9758,7.3883 13.0794,6.7696C13.1831,6.1509 13.1091,5.5145 12.8665,4.9385C12.6239,4.3626 12.2234,3.8723 11.7141,3.5279C11.2049,3.1835 10.6091,3 10,3C9.3909,3 8.7951,3.1835 8.2859,3.5279C7.7766,3.8723 7.3761,4.3626 7.1335,4.9385C6.8909,5.5145 6.8169,6.1509 6.9206,6.7696C7.0242,7.3883 7.301,7.9623 7.7169,8.4211C6.9461,8.6021 6.2577,9.0474 5.7642,9.684C5.2707,10.3207 5.0013,11.111 5,11.926V13H5.9375V11.926C5.9383,11.2285 6.2074,10.5599 6.6858,10.0668C7.1642,9.5736 7.8128,9.2962 8.4894,9.2954H11.5106C12.1872,9.2962 12.8358,9.5736 13.3142,10.0668C13.7926,10.5599 14.0617,11.2285 14.0625,11.926V13H15V11.926C14.9987,11.111 14.7293,10.3207 14.2358,9.684C13.7423,9.0474 13.0539,8.6021 12.2831,8.4211ZM10,8.49C9.5674,8.49 9.1444,8.3577 8.7847,8.11C8.425,7.8622 8.1446,7.51 7.979,7.0979C7.8134,6.6859 7.7701,6.2325 7.8545,5.7951C7.9389,5.3576 8.1473,4.9558 8.4532,4.6405C8.7591,4.3251 9.1489,4.1103 9.5732,4.0233C9.9976,3.9363 10.4374,3.981 10.8371,4.1516C11.2368,4.3223 11.5785,4.6113 11.8188,4.9822C12.0592,5.353 12.1875,5.789 12.1875,6.235C12.1875,6.833 11.957,7.4066 11.5468,7.8295C11.1366,8.2524 10.5802,8.49 10,8.49Z"/><path d="M4.5556,8.2728L3.2828,7L4.5556,5.7272L4.2728,5.4444L3,6.7172L1.7272,5.4444L1.4444,5.7272L2.7172,7L1.4444,8.2728L1.7272,8.5556L3,7.2828L4.2728,8.5556L4.5556,8.2728Z" stroke="white" stroke-width="0.5"/></svg>');
        filter: var(--icon-f);
      }

      .msg-status .msg-profile-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 20 20" fill="white" stroke="none"><path d="M13.653,9.893a5,5 0,1 0,-7.306 0A5.589,5.589 0,0 0,2 15.333V17H3.5V15.333A4.088,4.088 0,0 1,7.583 11.25h4.834A4.088,4.088 0,0 1,16.5 15.333V17H18V15.333A5.589,5.589 0,0 0,13.653 9.893ZM10,10a3.5,3.5 0,1 1,3.5 -3.5A3.5,3.5 0,0 1,10 10Z"/></svg>');
        filter: var(--icon-f);
      }

      .msg-status .msg-checkmark {
        background-image: url('data:image/svg+xml;utf-8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="white" stroke="none"><path d="M9.172,18.5l-6.188,-6.187l1.061,-1.061l5.127,5.127l10.783,-10.784l1.061,1.061l-11.844,11.844z"/></svg>');
        filter: var(--icon-f);
      }

      .msg-status .msg-expiration-timer-disabled {
        background-image: url('data:image/svg+xml;utf-8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 16 16" fill="white" stroke="none"><path d="M11.47,13.89C10.4585,14.6149 9.2445,15.0033 8,15C6.4087,15 4.8826,14.3679 3.7574,13.2426C2.6321,12.1174 2,10.5913 2,9C2.0004,7.7576 2.3923,6.5469 3.12,5.54L3.83,6.25C3.2027,7.2109 2.9251,8.3584 3.0437,9.4999C3.1623,10.6413 3.6699,11.7072 4.4814,12.5186C5.2928,13.3301 6.3587,13.8377 7.5001,13.9563C8.6416,14.0749 9.7891,13.7973 10.75,13.17L11.47,13.89ZM14.71,14.29L14,15L2,3L2.71,2.29L4.53,4.12C5.3872,3.4951 6.3948,3.1086 7.45,3L7,1H9L8.55,3C9.7326,3.1074 10.8567,3.5633 11.78,4.31C11.8082,4.1801 11.8708,4.0602 11.9613,3.9628C12.0519,3.8655 12.1669,3.7943 12.2945,3.7569C12.422,3.7194 12.5573,3.7169 12.6861,3.7498C12.8149,3.7826 12.9325,3.8496 13.0265,3.9435C13.1204,4.0375 13.1874,4.1551 13.2202,4.2839C13.2531,4.4127 13.2506,4.548 13.2131,4.6755C13.1757,4.8031 13.1045,4.9181 13.0072,5.0087C12.9098,5.0992 12.7899,5.1618 12.66,5.19C13.4844,6.2077 13.9531,7.4672 13.9946,8.7763C14.0362,10.0854 13.6482,11.3721 12.89,12.44L14.71,14.29ZM13,9C12.999,8.0979 12.7539,7.2129 12.2907,6.4387C11.8275,5.6646 11.1636,5.0302 10.3691,4.6027C9.5747,4.1753 8.6795,3.9707 7.7783,4.0107C6.877,4.0507 6.0034,4.3338 5.25,4.83L7.49,7.08L7.75,5H8.25L8.66,8.24L12.17,11.75C12.7097,10.9342 12.9983,9.9781 13,9Z"/></svg>');
        filter: var(--icon-f);
      }

      .msg-status .msg-expiration-timer-set {
        background-image: url('data:image/svg+xml;utf-8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 16 16" fill="white" stroke="none"><path d="M12.66,5.22C12.7899,5.1918 12.9098,5.1292 13.0072,5.0387C13.1045,4.9481 13.1757,4.8331 13.2132,4.7055C13.2506,4.578 13.2531,4.4427 13.2202,4.3139C13.1874,4.1851 13.1205,4.0675 13.0265,3.9735C12.9325,3.8795 12.8149,3.8126 12.6861,3.7798C12.5573,3.7469 12.422,3.7494 12.2945,3.7868C12.1669,3.8243 12.0519,3.8955 11.9613,3.9928C11.8708,4.0902 11.8082,4.2101 11.78,4.34C10.8603,3.5825 9.7358,3.1161 8.55,3L9,1H7L7.45,3C6.2642,3.1161 5.1397,3.5825 4.22,4.34C4.1918,4.2101 4.1292,4.0902 4.0387,3.9928C3.9481,3.8955 3.8331,3.8243 3.7055,3.7868C3.578,3.7494 3.4427,3.7469 3.3139,3.7798C3.1851,3.8126 3.0675,3.8795 2.9736,3.9735C2.8795,4.0675 2.8126,4.1851 2.7798,4.3139C2.7469,4.4427 2.7494,4.578 2.7868,4.7055C2.8244,4.8331 2.8955,4.9481 2.9928,5.0387C3.0902,5.1292 3.2101,5.1918 3.34,5.22C2.6259,6.1004 2.1759,7.1652 2.042,8.2908C1.9081,9.4165 2.0959,10.5571 2.5835,11.5805C3.0712,12.6038 3.8387,13.4681 4.7973,14.0732C5.756,14.6783 6.8664,14.9995 8,14.9995C9.1336,14.9995 10.244,14.6783 11.2027,14.0732C12.1613,13.4681 12.9289,12.6038 13.4165,11.5805C13.9041,10.5571 14.0919,9.4165 13.958,8.2908C13.8241,7.1652 13.3741,6.1004 12.66,5.22ZM8,14C7.0111,14 6.0444,13.7068 5.2221,13.1573C4.3999,12.6079 3.759,11.827 3.3806,10.9134C3.0022,9.9998 2.9032,8.9944 3.0961,8.0246C3.289,7.0546 3.7652,6.1637 4.4645,5.4645C5.1637,4.7652 6.0546,4.289 7.0245,4.0961C7.9945,3.9032 8.9998,4.0022 9.9134,4.3806C10.8271,4.759 11.6079,5.3999 12.1574,6.2221C12.7068,7.0444 13,8.0111 13,9C13,9.6566 12.8707,10.3068 12.6194,10.9134C12.3681,11.52 11.9998,12.0712 11.5355,12.5355C11.0712,12.9998 10.52,13.3681 9.9134,13.6194C9.3068,13.8707 8.6566,14 8,14ZM8.75,9C8.75,9.1989 8.671,9.3897 8.5303,9.5303C8.3897,9.671 8.1989,9.75 8,9.75C7.8011,9.75 7.6103,9.671 7.4697,9.5303C7.329,9.3897 7.25,9.1989 7.25,9L7.75,5H8.25L8.75,9Z"/></svg>');
        filter: var(--icon-f);
      }

      .msg-status .msg-phone-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="white" stroke="none"><path d="M17.21 22a8.08 8.08 0 0 1-2.66-.51 20.79 20.79 0 0 1-7.3-4.73 21 21 0 0 1-4.74-7.3c-.78-2.22-.67-4 .35-5.45h0a5 5 0 0 1 2-1.67 2.72 2.72 0 0 1 3.51.81l2.11 3a2.69 2.69 0 0 1-.35 3.49l-.93.85c-.09.08-.15.22-.08.31A20 20 0 0 0 11 13a20 20 0 0 0 2.21 1.91.24.24 0 0 0 .3-.08l.85-.93a2.68 2.68 0 0 1 3.49-.35l3 2.11a2.68 2.68 0 0 1 .85 3.43 5.22 5.22 0 0 1-1.71 2 4.69 4.69 0 0 1-2.78.91zM4.09 4.87c-.46.64-1 1.77-.16 4.08a19.28 19.28 0 0 0 4.38 6.74A19.49 19.49 0 0 0 15 20.07c2.31.81 3.44.3 4.09-.16a3.55 3.55 0 0 0 1.2-1.42A1.21 1.21 0 0 0 20 16.9l-3-2.12a1.18 1.18 0 0 0-1.53.15l-.82.9a1.72 1.72 0 0 1-2.33.29 21.9 21.9 0 0 1-2.37-2.05 22.2 22.2 0 0 1-2-2.37 1.71 1.71 0 0 1 .3-2.32l.89-.82A1.19 1.19 0 0 0 9.21 7L7.1 4a1.19 1.19 0 0 0-1.51-.38 3.72 3.72 0 0 0-1.5 1.25z"/></svg>');
        filter: var(--icon-f);
      }

      .msg-status .msg-video-call-incoming,
      .msg-status .msg-video-call-outgoing,
      .msg-status .msg-group-call,
      .msg-status .msg-call-incoming,
      .msg-status .msg-call-missed,
      .msg-status .msg-call-outgoing,
      .msg-status .msg-info-icon,
      .msg-status .msg-security-icon,
      .thread-subtitle .msg-security-icon,
      .msg-status .msg-pencil-icon,
      .msg-status .msg-thread-icon,
      .msg-status .msg-megaphone-icon,
      .msg-status .msg-member-add-icon,
      .msg-status .msg-member-remove-icon,
      .msg-status .msg-avatar-update-icon,
      .msg-status .msg-group-quit-icon,
      .msg-status .msg-members-icon,
      .msg-status .msg-member-approved-icon,
      .msg-status .msg-member-rejected-icon,
      .msg-status .msg-profile-icon,
      .msg-status .msg-checkmark,
      .msg-status .msg-expiration-timer-disabled,
      .msg-status .msg-expiration-timer-set,
      .msg-status .msg-phone-icon {
        display: inline-block;
        height: 18px;
        aspect-ratio: 1 / 1;
        margin-right: 8px;
        top: 2px;
        position: relative;
      }

      .thread-subtitle .msg-security-icon {
        margin-right: 3px;
      }

      #menu {
        display: flex;
        flex-direction: column;
        position: fixed;
        top: 20px;
        left: 20px;
      }

      #menu a:link,
      #menu a:visited,
      #menu a:hover,
      #menu a:active {
        color: #FFFFFF;
        text-decoration: none;
      }

      .menu-item .nav-up
      {
        margin-right: 5px;
      }

      .menu-item {
        display: flex;
        flex-direction: row;
        color: var(--menuitem-c);
        align-items: center;
        font-family: Roboto, "Noto Sans", "Liberation Sans", OpenSans, sans-serif;
        padding: 5px;
      }

)";
  if (pagemenu)
    file <<
      "      .expandedmenu a:link,\n"
      "      .expandedmenu a:visited,\n"
      "      .expandedmenu a:hover,\n"
      "      .expandedmenu a:active {\n"
      "        color: #FFFFFF;\n"
      "        text-decoration: none;\n"
      "      }\n"
      "\n"
      "      .expandedmenu .menu-item {\n"
      "        padding-left: 0px;\n"
      "        padding-bottom: 0px;\n"
      "      }\n"
      "\n"
      "      .expandedmenu .currentpage {\n"
      "        font-weight: bold;\n"
      "      }\n"
      "\n"
      "      .expandable-menu-item {\n"
      "        display: flex;\n"
      "        flex-direction: column;\n"
      "        margin-right: 0px;\n"
      "        cursor: pointer;\n"
      "        align-items: end;\n"
      "      }\n"
      "\n"
      "      .expandedmenu-container {\n"
      "        background-color: var(--conversationbox-bc);\n"
      "        border-radius: 10px;\n"
      "        padding: 10px;\n"
      "        margin-top: 7px;\n"
      "      }\n"
      "\n"
      "      .expandedmenu {\n"
      "        display: flex;\n"
      "        flex-direction: column;\n"
      "        align-items: flex-start;\n"
      "        width: 0px;\n"
      "        max-height: 0px;\n"
      "        overflow: hidden;\n"
      "        opacity: 0%;\n"
      "        border: none;\n"
      "        background: var(--body-bgc);\n"
      "        transition: max-height .25s ease-out, padding .25s ease-out, opacity .25s ease-out, width 0s 0.25s, overflow 0s 0.25s;\n"
      "        position: absolute;\n"
      "      }\n"
      "\n"
      "     .expandable-menu-item:hover .expandedmenu {\n"
      "       width: max-content;\n"
      "       max-height: 90vh;\n"
      "       top: 35px;\n"
      "       opacity: 100%;\n"
      "       transition: max-height .25s ease-out, padding .25s ease-out, opacity .15s ease-out;\n"
      "       align-items: end;\n"
      "       overflow-y: scroll;\n"
      "     }\n"
      "\n"
      "     .menu-header .label-text {\n"
      "       display: inline-block;\n"
      "       height: 100%;\n"
      "       vertical-align: middle;\n"
      "     }\n"
      "\n"
      "     #jump-to-page-icon {\n"
      "       display: inline-block;\n"
      "       width:30px;\n"
      "       height: 30px;\n"
      "       margin-right: 5px;\n"
      "       vertical-align: middle;\n"
      "       background: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"white\" stroke-width=\"1.5\" stroke-linecap=\"round\"><defs><path id=\"p\" d=\"M15.078 2.75a3 3 0 0 0-2.828-2h-8.5a3 3 0 0 0-3 3h0v12.5a3 3 0 0 0 3 3h1.191\"/><path id=\"l\" d=\"M12,8.75 h8\"/></defs><path d=\"M8.75 7.75v12.5a3 3 0 0 0 3 3h8.5a3 3 0 0 0 3-3V7.75a3 3 0 0 0-3-3h-8.5a3 3 0 0 0-3 3z\"/><path d=\"M12,19.25h5\"/><use href=\"%23p\"/><use href=\"%23p\" x=\"4\" y=\"2\"/><use href=\"%23l\"/><use href=\"%23l\" y=\"3.5\"/><use href=\"%23l\" y=\"7\"/></svg>');\n"
      "       filter: var(--icon-f);\n"
      "     }\n"
      "\n";
  file << R"(      #thread-title {
        display: flex;
        align-items: center;
        width: auto;
        margin-left: auto;
        margin-right: auto;
      }

      .threadtitle {
        display: inline-block;
        font-size: x-large;
        font-family: Roboto, "Noto Sans", "Liberation Sans", OpenSans, sans-serif;
        padding: 0px;
        margin-top: 0px;
        margin-bottom: 0px;
        margin-left: auto;
        margin-right: auto;
        min-width: 100%;
        white-space: pre-wrap;
        overflow-wrap: anywhere;
      }

      .threadtitle-info {
        font-size: large;
      }

      .menu-icon {
        margin-right: 0px;
        width: 30px;
        aspect-ratio: 1 / 1;
        background-position: center;
        background-repeat: no-repeat;
        background-size: cover;
      }

      #bottom {
        display: flex;
        position: fixed;
        bottom: 20px;
        right: 20px;
      }

)";

  if (themeswitch || searchpage || pagemenu)
  {
    file << R"(
      #theme {
        display: flex;
        flex-direction: row;
        position: fixed;
        top: 20px;
        right: 20px;
      }
)";
  }

  if (isnotetoself || isreleasechannel)
  {
    file << R"(
    .official {
      display: inline-block;
      background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns="http://www.w3.org/2000/svg" viewBox="3 3 18 18"><path d="M10.662 3.455a1.82 1.82 0 0 1 2.676 0l.37.402a1.82 1.82 0 0 0 1.697.551l.535-.108a1.82 1.82 0 0 1 2.165 1.573l.063.542a1.82 1.82 0 0 0 1.049 1.444l.496.227a1.82 1.82 0 0 1 .827 2.545l-.268.476a1.82 1.82 0 0 0 0 1.785l.268.476a1.82 1.82 0 0 1-.827 2.545l-.496.227a1.82 1.82 0 0 0-1.049 1.444l-.063.542A1.82 1.82 0 0 1 15.94 19.7l-.535-.108a1.82 1.82 0 0 0-1.697.551l-.37.402a1.82 1.82 0 0 1-2.676 0l-.37-.402a1.82 1.82 0 0 0-1.697-.551l-.535.108a1.82 1.82 0 0 1-2.165-1.573l-.063-.542a1.82 1.82 0 0 0-1.049-1.444l-.496-.227a1.82 1.82 0 0 1-.827-2.545l.268-.476a1.82 1.82 0 0 0 0-1.785l-.268-.476a1.82 1.82 0 0 1 .827-2.545l.496-.227a1.82 1.82 0 0 0 1.049-1.444l.063-.542A1.82 1.82 0 0 1 8.06 4.3l.535.108a1.82 1.82 0 0 0 1.697-.551l.37-.402z" fill="%232c6bed"/><path d="M16.524 9.948l-.999-.999-4.55 4.551-2.164-2.164-.999.999 3.162 3.162 5.549-5.549z" fill="white"/></svg>');
      height: 24px;
      aspect-ratio: 1 / 1;
      margin-left: 8px;
      margin-bottom: 3px;
      vertical-align: bottom;
    }
)";
  }

  if (isblocked)
  {
    file <<
      "\n"
      "    .blocked {\n"
      "      display: inline-block;\n"
      "      background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 24 24\" fill=\"white\" stroke=\"white\"><path d=\"M12 1a11 11 0 1 0 11 11A11 11 0 0 0 12 1zm0 1.5a9.448 9.448 0 0 1 6.159 2.281L4.781 18.159A9.488 9.488 0 0 1 12 2.5zm0 19a9.448 9.448 0 0 1-6.159-2.281L19.219 5.841A9.488 9.488 0 0 1 12 21.5z\"/></svg>');\n"
      "      height: 24px;\n"
      "      aspect-ratio: 1 / 1;\n"
      "      margin-right: 6px;\n"
      "      filter: var(--icon-f);\n"
      "      margin-bottom: 2px;\n"
      "      vertical-align: bottom;\n"
      "    }\n";
  }

  if (ismuted && !isblocked)
  {
    file <<
      "\n"
      "    .muted {\n"
      "      display: inline-block;\n"
      "      background-image: url('data:image/svg+xml;utf-8,<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 16 16\" fill=\"white\"><path d=\"M6.163 14h3.674a1.875 1.875 0 0 1-3.674 0zM8 2a3.233 3.233 0 0 1 3.041 2.171l.113.322L3.5 10.507a8.079 8.079 0 0 0 .335-1.136L4.84 4.548A3.25 3.25 0 0 1 8 2m0-1a4.236 4.236 0 0 0-4.138 3.337l-1.007 4.83a5.83 5.83 0 0 1-1.785 3.25l-.879.69.618.786 14-11-.618-.786-2.206 1.734A4.225 4.225 0 0 0 8 1zm6.54 10.035a2.846 2.846 0 0 1-1.395-1.868l-.662-3.176h0l-.878.689.564 2.7a3.954 3.954 0 0 0 1.89 2.558A.059.059 0 0 1 14 12H4.834l-1.272 1H14a1.056 1.056 0 0 0 .54-1.965z\"/></svg>');\n"
      "      height: 24px;\n"
      "      aspect-ratio: 1 / 1;\n"
      "      margin-top: 3px;\n"
      "      margin-right: 8px;\n"
      "      filter: var(--icon-f);\n"
      "      margin-bottom: 2px;\n"
      "      vertical-align: bottom;\n"
      "    }\n";
  }

  if (themeswitch)
  {
    file << R"*(
    .themebutton {
      display: block;
      background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 25 25" fill="white" stroke="white"><defs><path id="s" d="m12.5,2.5 2.5,2.5c0.27,0.27 0.63,0.43 1,0.43L19.62,5.42h1.79l-3.8e-5,-0.4C21.41,4.24 20.77,3.61 20,3.6l-3.8,0 -2.64,-2.65C13,0.4 12,0.4 11.5,0.96l-0.28,0.28z"/></defs><path d="m12,8.13c0,-0.4 0.35,-0.8 0.8,-0.76 2.7,0.14 4.86,2.4 4.86,5.13 0,2.75 -2.15,5 -4.86,5.14 -0.45,0 -0.8,-0.35 -0.8,-0.77z"/><use href="%23s"/><use href="%23s" transform="rotate(45 12.5 12.5)"/><use href="%23s" transform="rotate(90 12.5 12.5)"/><use href="%23s" transform="rotate(135 12.5 12.5)"/><use href="%23s" transform="rotate(180 12.5 12.5)"/><use href="%23s" transform="rotate(225 12.5 12.5)"/><use href="%23s" transform="rotate(270 12.5 12.5)"/><use href="%23s" transform="rotate(315 12.5 12.5)"/></svg>');
      filter: var(--icon-f);
    }
)*";
  }

  if (searchpage)
  {
    file << R"(
    .searchbutton {
      display: block;
      background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="white" stroke="white"><path d="M10 2.125a7.875 7.875 0 1 0 4.716 14.182l4.989 4.989a1.125 1.125 0 0 0 1.59-1.591l-4.988-4.989A7.875 7.875 0 0 0 10 2.125zM3.875 10a6.125 6.125 0 1 1 12.25 0 6.125 6.125 0 0 1-12.25 0z"/></svg>');
      filter: var(--icon-f);
    }
)";
  }

    file << R"(
    .nav-up {
      background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 20 20" fill="white" stroke="white"><path d="M9.5,17.5l1.1,-1.1l-4.9,-4.9l-1.1,-0.8H17V9.2H4.6l1.1,-0.8l4.9,-5L9.5,2.5L2,10L9.5,17.5z"/></svg>');
      filter: var(--icon-f);
    }

    .menu-item-bottom {
      display: flex;
      color: var(--menuitem-c);
      border-radius: 50%;
      background-color: var(--msgreactioninfo-bc);
      width: 40px;
      height: 40px;
      align-items: center;
      justify-content: center;
      padding: 0px;
    }

    .nav-bottom {
      transform: rotate(270deg);
      width: 25px;
      height: 25px;
    }

    .nav-one {
      background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 20 20" fill="none" stroke="white" stroke-width="3"><path d="M 13.8,2.94 6.73,10 13.8,17.06"/></svg>');
      filter: var(--icon-f);
    }

    .nav-max {
      background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 20 20" fill="none" stroke="white" stroke-width="3"><path d="M 10.75,2.94 3.68,10 10.75,17.06M 16.85,2.94 9.78,10 16.85,17.06"/></svg>');
      filter: var(--icon-f);
    }

    .nav-fwd {
      transform: scaleX(-1);
    }

    .nav-disabled {
      filter: var(--nav-disabled-f);
    }

    .conversation-link {
      display: flex;
    }

    .conversation-link-left {
      padding-right: 20px;
      order: -1;
    }

    .conversation-link-right {
      padding-left: 20px;
      order: 1;
    }

    .conversation-link > div {
      align-self: flex-end;
      position: sticky;
      bottom: 30px;
      padding-right: 5px;
      padding-left: 5px;
      padding-top: 5px;
    }

    .groupdetails {
      display: block;
      max-height: 0px;
      max-width: 90%;
      margin-left: auto;
      margin-right: auto;
      overflow: hidden;
      padding-top: 0px;
      padding-bottom: 0px;
      transition: padding-top 0.05s ease, padding-bottom 0.05s ease, max-height 0.25s ease;
    }

    .columnview {
      display: flex;
      flex-flow: row wrap;
      justify-content: space-between;
      overflow-wrap: anywhere;
    }

    .column-left-align,
    .column-right-align {
      flex: 0 0 49%;
    }

    .column-right-align {
      padding-right: 1%;
      text-align: right;
    }

    .column-left-align {
      padding-left: 1%;
      text-align: left;
    }

    .columnview-header {
      flex: 0 0 100%;
      text-align: center;
    }

    .groupdetails .columnview-header {
      font-style: italic;
    }

    .msg-receipt-info .columnview-header {
      text-align: left;
      font-weight: bold;
      padding-top: 3px;
      padding-left: 1%;
    }

    .thread-subtitle input[type=checkbox]:checked ~ label > .groupdetails {
      max-height: none;
      padding-top: 5px;
      padding-bottom: 5px;
      overflow: visible;
      transition: padding-top 0.2s ease, padding-bottom 0.2s ease, max-height 0.4s ease;
    }

    .thread-subtitle input[type=checkbox] ~ label > small::before {
      content: '(show';
    }

    .thread-subtitle input[type=checkbox]:checked ~ label >  small::before {
      content: '(hide';
    }

    @media print {
      #menu {
        display: none;
      })";

    if (exportdetails)
    {
      file
        << "\n\n"
        << "      .export-details {\n"
        << "        display: grid;\n"
        << "      }\n";
    }

    file << R"(
      #theme {
        display: none;
      }

      .thread-subtitle > label > small {
        display: none;
      }

      .msg {
        break-inside: avoid;
        /* both fit-content and max-content seem fine here, so just including both as fall back */
        width: -webkit-fit-content;
        width: -moz-fit-content;
        width: fit-content;
        /*leave it up to print settings */
        /*background-color: transparent;*/
      }

      .msg-incoming, .msg-outgoing {
        border: 1px solid black;
        display: block;
      }

      .msg-incoming .spoiler,
      .msg-outgoing .spoiler {
        filter: none;
        background: rgba(0, 0, 0, .2);
      }

      .no-bg-bubble {
        border: 0;
      }

      .groupdetails {
        max-height: none;
        padding-top: 5px;
        padding-bottom: 5px;
        overflow: visible;
      }

      .incoming-group-msg {
        display: block;
      }

      .incoming-group-msg > .msg-incoming {
        display: inline-block;
      }

      .msg.msg-incoming, .incoming-group-msg {
        margin-right: auto;
      }

      .msg.msg-outgoing {
        margin-left: auto;
      }

      .msg-status, .msg-date-change {
        margin: 0 auto;
      }

      .conversation-wrapper {
        width: 100%;
      }

      body, .controls-wrapper, .conversation-wrapper, .conversation-box {
        display: block;
        /*leave it up to print settings */
        /*background-color: transparent;*/
      }

      .conversation-box {
        padding: 0 3px;
        margin: 0;
        box-sizing: border-box;
        width: 100%;
        border-radius: 0;
        /*leave it up to print settings */
        /*color: black; */
      }

      .msg-reaction {
        border: none;
      }

      #message-header {
        padding-top: 0;
        padding-bottom: 10px;
        /*leave it up to print settings */
        /* color: black;*/
      }

      .msg-quote {
        border: 1px solid grey;
        border-left: 5px solid grey;
      }

      .msg-incoming .is-expiring,
      .thread-disappearing-messages-info,
      .msg-status .msg-video-call-incoming, .msg-status .msg-video-call-outgoing,
      .msg-status .msg-group-call, .msg-status .msg-call-incoming,
      .msg-status .msg-call-missed, .msg-status .msg-call-outgoing,
      .msg-status .msg-info-icon, .msg-status .msg-security-icon,
      .msg-status .msg-pencil-icon, .msg-status .msg-megaphone-icon,
      .msg-status .msg-member-add-icon, .msg-status .msg-member-remove-icon,
      .msg-status .msg-avatar-update-icon, .msg-status .msg-group-quit-icon,
      .msg-status .msg-members-icon, .msg-status .msg-member-approved-icon,
      .msg-status .msg-member-rejected-icon,
      .msg-status .msg-profile-icon, .msg-status .msg-checkmark,
      .msg-status .msg-expiration-timer-disabled, .msg-status .msg-expiration-timer-set,
      .msg-status .msg-phone-icon, .msg-status .msg-thread-icon,
      .msg-incoming .shared-contact-avatar-default,
      .msg-outgoing .shared-contact-avatar-default {
        -webkit-print-color-adjust: exact;
        print-color-adjust: exact;
        filter: brightness(0.5);
      }

      .status-text > div, .footer-icons {
        -webkit-print-color-adjust: exact;
        color-adjust: exact;
        print-color-adjust: exact;
        filter: var(--media-status-checkmarks-f);
      }

      .status-text > div.msg-call-missed {
        filter: none;
      }

      .avatar {
        -webkit-print-color-adjust: exact;
        color-adjust: exact;
        print-color-adjust: exact;
        display: inline-block;
      }

      .convo-avatar, .incoming-group-msg, .msg {
        break-inside: avoid;
       }

      .convo-avatar {
        vertical-align: bottom;
      }

      .right-column,
      .left-column {
        padding: 0px;
        text-align: left;
        flex: 0 0 100%;
      }

      .left-column {
        font-style: italic;
      }

      .left-column::before {
        content: '- ';
      }

      .columnview-header {
        display: none;
      }

      #bottom {
        display: none;
      }
      /* todo: print style for audio, video and attachment previews */
    } /* end @media print */

    </style>
  </head>
  <body>
)";

  if (themeswitch)
  {

    file << R"(
  <script>
    function setCookie(name, value, days)
    {
      var expires = "";
      if (days)
      {
        var date = new Date();
        date.setTime(date.getTime() + (days * 24 * 60 * 60 * 1000));
        expires = "; expires=" + date.toUTCString();
      }
      document.cookie = name + "=" + (value || "")  + expires + "; SameSite=None; Secure; path=/";
    }

    function getCookie(name)
    {
      var nameEQ = name + "=";
      var ca = document.cookie.split(';');
      for (var i = 0; i < ca.length; ++i)
      {
        var c = ca[i];
        while (c.charAt(0) == ' ')
          c = c.substring(1, c.length);
        if (c.indexOf(nameEQ) == 0)
          return c.substring(nameEQ.length, c.length);
      }
      return null;
    }

    function eraseCookie(name)
    {
      document.cookie = name + '=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/'
    }

    // Important to be 1st in the DOM
    const theme = getCookie('theme') || ')" << (light ? "light" : "dark") << R"(';
    //alert(theme);

    document.documentElement.dataset.theme = theme;
  </script>)";
  }

  // set expiration timer string
  std::string exptimer = "Off";
  std::string exptimer_short;
  long long int expiration_timer = isgroup ? groupinfo.expiration_timer : getRecipientInfoFromMap(recipient_info, thread_recipient_id).message_expiration_time;
  if (expiration_timer)
  {
    if (expiration_timer < 60) // less than full minute
    {
      exptimer = bepaald::toString(expiration_timer) + " second"
        + (expiration_timer == 1 ? "" : "s");
      exptimer_short = bepaald::toString(expiration_timer) + "s";
    }
    else if (expiration_timer < 60 * 60) // less than full hour
    {
      exptimer = bepaald::toString(expiration_timer / 60) + " minute"
        + (expiration_timer / 60 == 1 ? "" : "s");
      exptimer_short = bepaald::toString(expiration_timer / 60) + "m";
    }
    else if (expiration_timer < 24 * 60 * 60) // less than full day
    {
      exptimer = bepaald::toString(expiration_timer / (60 * 60)) + " hour"
        + (expiration_timer / (60 * 60) == 1 ? "" : "s");
      exptimer_short = bepaald::toString(expiration_timer / (60 * 60)) + "h";
    }
    else if (expiration_timer < 7 * 24 * 60 * 60) // less than full week
    {
      exptimer = bepaald::toString(expiration_timer / (24 * 60 * 60)) + " day"
        + (expiration_timer / (24 * 60 * 60) == 1 ? "" : "s");
      exptimer_short = bepaald::toString(expiration_timer / (24 * 60 * 60)) + "d";
    }
    else // show expiration_timer in number of weeks
    {
      exptimer = bepaald::toString(expiration_timer / (7 * 24 * 60 * 60)) + " week"
        + (expiration_timer / (7 * 24 * 60 * 60) == 1 ? "" : "s");
      exptimer_short = bepaald::toString(expiration_timer / (7 * 24 * 60 * 60)) + "w";
    }
  }



file << R"(
  <input type="checkbox" id="theme-switch">
  <div id="page">
    <div class="controls-wrapper">
      <div class="conversation-wrapper">
        <div id="message-header">)";
  if (thread_avatar.empty() || isnotetoself)
  {
    if (isgroup)
    {
      file << R"(
          <div class="avatar header-avatar msg-sender-)" << thread_recipient_id << R"(">
            <div class="group-avatar-icon"></div>
          </div>)";
    }
    if (isnotetoself)
    {
      file << R"(
          <div class="avatar header-avatar msg-sender-)" << thread_recipient_id << R"(">
            <div class="note-to-self-icon"></div>
          </div>)";
    }
    if (!isgroup && !isnotetoself)
    {
      file << R"(
          <div class="avatar header-avatar)" << (getRecipientInfoFromMap(recipient_info, thread_recipient_id).initial_is_emoji ? " avatar-emoji-initial" : "") << R"( msg-sender-)" << thread_recipient_id << R"(">)" << getRecipientInfoFromMap(recipient_info, thread_recipient_id).initial << R"(</div>)";
    }
  }
  else
  {
    file << R"(
          <input type="checkbox" id="zoomCheck-avatar">
          <label for="zoomCheck-avatar">
            <img class="avatar avatar-)" << thread_recipient_id << " header-avatar msg-sender-" << thread_recipient_id << "\" src=\"" << thread_avatar << R"(" alt=")" + getRecipientInfoFromMap(recipient_info, thread_recipient_id).initial + R"(">
          </label>)";
  }

  file <<
    "\n"
    "          <div id=\"thread-title\"><pre class=\"threadtitle\">";

  if (isblocked)
    file << "<span class=\"blocked\"></span>";
  else if (ismuted)
    file << "<span class=\"muted\"></span>";

  file << title;

  if (expiration_timer)
    file << "<span class=\"thread-disappearing-messages-info\"></span><span class=\"threadtitle-info\">" + exptimer_short + "</span>";
  if (isnotetoself || isreleasechannel)
    file << "<span class=\"official\"></span>";

  file <<
    "</pre></div>\n";

  if (!isnotetoself && getRecipientInfoFromMap(recipient_info, thread_recipient_id).verified)
    file <<
      "          <div class=\"thread-subtitle\">\n"
      "            <span class=\"msg-security-icon\"></span>verified\n"
      "          </div>\n";
  file <<
    "            <div class=\"thread-subtitle\">\n";
  if (isgroup)
  {
    file <<
      groupmembers.size() << " member" << (groupmembers.size() != 1 ? "s" : "") << "\n"
      "            <input type=\"checkbox\" id=\"showmembers\">\n"
      "            <label for=\"showmembers\">\n"
      "              <small> details)</small>\n"
      "              <span class=\"groupdetails\">\n"
      "                <span class=\"columnview\">\n";

    // group description
    if (!groupinfo.description.empty())
    {
      file <<
        "                  <span class=\"column-right-align\">Description:</span>\n"
        "                  <span class=\"column-left-align\">" << HTMLescapeString(groupinfo.description) << "</span>\n";
    }

    // group members
    file <<
      "                  <span class=\"column-right-align\">Members:</span>\n"
      "                  <span class=\"column-left-align\">";
    for (unsigned int gm = 0; gm < groupmembers.size(); ++gm)
      file << HTMLescapeString(getRecipientInfoFromMap(recipient_info, groupmembers[gm]).display_name)
           << (bepaald::contains(groupinfo.admin_ids, groupmembers[gm]) ? " <i>(admin)</i>" : "") << ((gm < groupmembers.size() - 1) ? ", " : "");
    file << "</span>\n";

    if (d_database.tableContainsColumn("groups", "decrypted_group"))
    {
      // pending members
      file <<
        "                  <span class=\"column-right-align\">Pending members:</span>\n"
        "                  <span class=\"column-left-align\">";
      if (groupinfo.pending_members.size() == 0)
        file << "(none)";
      else
        for (unsigned int pm = 0; pm < groupinfo.pending_members.size(); ++pm)
          file << HTMLescapeString(getRecipientInfoFromMap(recipient_info, groupinfo.pending_members[pm]).display_name)
               << ((pm < groupinfo.pending_members.size() - 1) ? ", " : "");
      file << "</span>\n";

      // 'requesting' members
      file <<
        "                  <span class=\"column-right-align\">Requesting members:</span>\n"
        "                  <span class=\"column-left-align\">";
      if (groupinfo.requesting_members.size() == 0)
        file << "(none)";
      else
        for (unsigned int rm = 0; rm < groupinfo.requesting_members.size(); ++rm)
          file << HTMLescapeString(getRecipientInfoFromMap(recipient_info, groupinfo.requesting_members[rm]).display_name)
               << ((rm < groupinfo.requesting_members.size() - 1) ? ", " : "");
      file << "</span>\n";

      // banned members
      file <<
        "                  <span class=\"column-right-align\">Banned members:</span>\n"
        "                  <span class=\"column-left-align\">";
      if (groupinfo.banned_members.size() == 0)
        file << "(none)";
      else
        for (unsigned int bm = 0; bm < groupinfo.banned_members.size(); ++bm)
          file << HTMLescapeString(getRecipientInfoFromMap(recipient_info, groupinfo.banned_members[bm]).display_name)
               << ((bm < groupinfo.banned_members.size() - 1) ? ", " : "");
      file << "</span>\n";

      // coolumn header: access control
      file <<
        "                  <span class=\"columnview-header\">Permissions</span>\n"
        "                  <span class=\"column-right-align\">Add members:</span>\n"
        "                  <span class=\"column-left-align\">" << groupinfo.access_control_members << "</span>\n"
        "                  <span class=\"column-right-align\">Edit group info:</span>\n"
        "                  <span class=\"column-left-align\">" << groupinfo.access_control_attributes << "</span>\n"
        "                  <span class=\"column-right-align\">Send messages:</span>\n"
        "                  <span class=\"column-left-align\">" << (groupinfo.isannouncementgroup ? "Only admins" : "All members") << "</span>\n"
        "                  <span class=\"column-right-align\">Approve members from invite link:</span>\n"
        "                  <span class=\"column-left-align\">" << groupinfo.access_control_addfromlinkinvite << "</span>\n"
        // column header: option
        "                  <span class=\"columnview-header\">Options</span>\n"
        // expiration timer
        "                  <span class=\"column-right-align\">Disappearing messages:</span>\n"
        "                  <span class=\"column-left-align\">" << exptimer << "</span>\n"
        // link enabled?
        "                  <span class=\"column-right-align\">Group link:</span>\n"
        "                  <span class=\"column-left-align\">" << (groupinfo.link_invite_enabled ? "Enabled" : "Off") <<  "</span>\n";
        // muted
        if (getRecipientInfoFromMap(recipient_info, thread_recipient_id).mute_until != -1)
        {
          long long int mute = getRecipientInfoFromMap(recipient_info, thread_recipient_id).mute_until;
          file <<
            "                  <span class=\"column-right-align\">Muted:</span>\n"
            "                  <span class=\"column-left-align\">" << (mute == 0 ? "No" : (mute == std::numeric_limits<int64_t>::max() ? "Always" : "Could not be determined"))  <<  "</span>\n";
        }

      // notify-on-mention
      if (getRecipientInfoFromMap(recipient_info, thread_recipient_id).mention_setting != -1)
      {
        file <<
          "                  <span class=\"column-right-align\">Mentions:</span>\n"
          "                  <span class=\"column-left-align\">" << (getRecipientInfoFromMap(recipient_info, thread_recipient_id).mention_setting ? "Do not notify" : "Always notify") <<  "</span>\n";
      }

      // custom notifications
      if (getRecipientInfoFromMap(recipient_info, thread_recipient_id).custom_notifications != -1)
      {
        file <<
          "                  <span class=\"column-right-align\">Custom notifications:</span>\n"
          "                  <span class=\"column-left-align\">" << (getRecipientInfoFromMap(recipient_info, thread_recipient_id).custom_notifications ? "Enabled" : "Disabled") <<  "</span>\n";

        // custom notifications were enabled, let's print them
        if (getRecipientInfoFromMap(recipient_info, thread_recipient_id).custom_notifications)
        {
          SqliteDB::QueryResults notification_res;
          if (d_database.exec("SELECT message_ringtone, message_vibrate, call_ringtone, call_vibrate FROM recipient WHERE _id = ?", thread_recipient_id, &notification_res) &&
              notification_res.rows() == 1)
          {
            //std::string msg_ringtone = notification_res.valueAsString(0, "message_ringtone");
            long long int msg_vibrate = notification_res.valueAsInt(0, "message_vibrate");
            //std::string call_ringtone = notification_res.valueAsString(0, "call_ringtone");
            long long int call_vibrate = notification_res.valueAsInt(0, "call_vibrate");

            file <<
              // "                  <span class=\"column-right-align\">Message ringtone:</span>\n"
              // "                  <span class=\"column-left-align\">" << msg_ringtone <<  "</span>\n"
              "                  <span class=\"column-right-align\">Message vibrate:</span>\n"
              "                  <span class=\"column-left-align\">" << (msg_vibrate == 0 ? "(default)" : (msg_vibrate == 1 ? "Enabled" : "Disabled")) <<  "</span>\n"
              // "                  <span class=\"column-right-align\">Call ringtone:</span>\n"
              // "                  <span class=\"column-left-align\">" << call_ringtone <<  "</span>\n"
              "                  <span class=\"column-right-align\">Call vibrate:</span>\n"
              "                  <span class=\"column-left-align\">" << (call_vibrate == 0 ? "(default)" : (call_vibrate == 1 ? "Enabled" : "Disabled")) <<  "</span>\n";
          }
        }
      }
    }
    else // decrypted_group info not available
      file << "                  <span class=\"columnview-header\">More group details not available</span>\n";

    file <<
      "                </span>\n"
      "              </span>\n"
      "            </label>\n";
  }
  else // !isgroup
    file << (getRecipientInfoFromMap(recipient_info, thread_recipient_id).display_name == getRecipientInfoFromMap(recipient_info, thread_recipient_id).phone ? "" :
             getRecipientInfoFromMap(recipient_info, thread_recipient_id).phone) << '\n';
  file << R"(          </div>
        </div>
        <div class="conversation-box">

)";

  return true;

}

void SignalBackup::HTMLwriteAttachmentDiv(std::ofstream &htmloutput, SqliteDB::QueryResults const &attachment_results, int indent,
                                          std::string const &directory, std::string const &threaddir, bool use_original_filenames,
                                          bool is_image_preview, bool overwrite, bool append,
                                          std::vector<std::string> const &ignoremediatypes) const
{
  for (unsigned int a = 0; a < attachment_results.rows(); ++a)
  {

    // long text body
    if (attachment_results(a, d_part_ct) == "text/x-signal-plain")
      continue;

    long long int rowid = attachment_results.getValueAs<long long int>(a, "_id");
    long long int uniqueid = attachment_results.getValueAs<long long int>(a, "unique_id");
    long long int pending_push = attachment_results.getValueAs<long long int>(a, d_part_pending);

    if (pending_push != 0)
    {
      htmloutput << std::string(indent, ' ') << "<div class=\"attachment\">\n";
      htmloutput << std::string(indent, ' ') << "  <div class=\"pending-attachment\">\n";
      htmloutput << std::string(indent, ' ') << "    (attachment not downloaded)\n";
      htmloutput << std::string(indent, ' ') << "  </div>\n";
      htmloutput << std::string(indent, ' ') << "</div>\n";
      return;
    }

    std::string content_type = attachment_results.valueAsString(a, d_part_ct);
    std::string original_filename;
    if (!attachment_results.isNull(a, "file_name") && !attachment_results(a, "file_name").empty())
    {
      original_filename = attachment_results(a, "file_name");
      HTMLescapeString(&original_filename);
    }

    std::string extension(MimeTypes::getExtension(content_type, "bin"));
    std::string attachment_filename_on_disk = bepaald::concat("Attachment_", bepaald::toString(rowid), "_", bepaald::toString(uniqueid), ".", extension);
    if (use_original_filenames)
    {
      attachment_filename_on_disk = sanitizeFilename(attachment_results(a, "file_name"), d_aggressive_filename_sanitizing);
      if (attachment_filename_on_disk.empty())    // filename was not set in database or was not impossible
      {                                           // to sanitize (eg reserved name in windows 'COM1')
        long long int datum = attachment_results.valueAsInt(a, "date_received", -1);
        std::string datestr = (datum != -1) ? bepaald::toDateString(datum / 1000, "signal-%Y-%m-%d-%H%M%S") : "signal";
        attachment_filename_on_disk = bepaald::concat(datestr, ".", extension);
      }
      if (!makeFilenameUnique(bepaald::concat(directory, "/", threaddir, "/media"), &attachment_filename_on_disk))
      {
        Logger::error("Getting unique filename for '", directory, "/", threaddir, "/media/", attachment_filename_on_disk, "'");
        continue;
      }
    }

    // write the attachment data
    if (!HTMLwriteAttachment(directory, threaddir, rowid, uniqueid, attachment_filename_on_disk, attachment_results.valueAsInt(a, "date_received", -1), overwrite, append))
      continue;

    if (use_original_filenames)
      HTMLescapeUrl(&attachment_filename_on_disk);

    // check if content type must not be handled as media:
    bool ignoremedia = false;
    if (bepaald::contains(ignoremediatypes, content_type)) [[unlikely]]
      ignoremedia = true;

    htmloutput << std::string(indent, ' ') << "<div class=\"attachment"
               << (((!STRING_STARTS_WITH(content_type, "image/") && !STRING_STARTS_WITH(content_type, "video/") && !STRING_STARTS_WITH(content_type, "audio/")) || ignoremedia) ?
                   " attachment-unknown-type" : "")
               << "\">\n";

    if (STRING_STARTS_WITH(content_type, "image/") && !ignoremedia)
    {
      htmloutput << std::string(indent, ' ') << "  <div class=\"msg-" << (is_image_preview ? "linkpreview-" : "") << "img-container\">\n";
      htmloutput << std::string(indent, ' ') << "    <input type=\"checkbox\" id=\"zoomCheck-" << rowid << "-" << uniqueid << "\">\n";
      htmloutput << std::string(indent, ' ') << "    <label for=\"zoomCheck-" << rowid << "-" << uniqueid << "\">\n";
      htmloutput << std::string(indent, ' ') << "      <img src=\"media/" << attachment_filename_on_disk << "\" alt=\"Image attachment\">\n";
      htmloutput << std::string(indent, ' ') << "    </label>\n";
      if (attachment_results.hasColumn("caption") &&
          !attachment_results.isNull(a, "caption"))
        htmloutput << std::string(indent, ' ') << "    <pre><span class=\"caption\">" << attachment_results(a, "caption") << "</span></pre>\n";
      htmloutput << std::string(indent, ' ') << "  </div>\n";
    }
    else if ((STRING_STARTS_WITH(content_type, "video/") ||
              STRING_STARTS_WITH(content_type, "audio/")) && !ignoremedia)
    {
      htmloutput << std::string(indent, ' ') << "  <div class=\"msg-vid-container\">\n";
      htmloutput << std::string(indent, ' ') << "    <" << std::string_view(content_type.data(), 5) << " controls>\n";
      htmloutput << std::string(indent, ' ') << "      <source src=\"media/" << attachment_filename_on_disk << "\" type=\"" << content_type << "\">\n";
      //htmloutput << std::string(indent, ' ') << "      Media of type " << content_type << "<span class=\"msg-dl-link\"><a href=\"media/Attachment_" << rowid
      //           << "_" << uniqueid << "." << extension << "\" type=\"" << content_type << "\">&#129055;</a></span>\n";
      htmloutput << std::string(indent, ' ') << "    </" << std::string_view(content_type.data(), 5) << ">\n";
      if (attachment_results.hasColumn("caption") &&
          !attachment_results.isNull(a, "caption"))
        htmloutput << std::string(indent, ' ') << "    <pre><span class=\"caption\">" << attachment_results(a, "caption") << "</span></pre>\n";
      htmloutput << std::string(indent, ' ') << "  </div>\n";
    }
    else
    {
      htmloutput << std::string(indent, ' ') << "  <div class=\"document-icon\"><div class=\"document-ext\">" << (extension.size() <= 5 ? extension : "") << "</div></div>\n";
      if (content_type.empty())
      {
        if (original_filename.empty())
          htmloutput << std::string(indent, ' ') << "  <div>[Attachment of unknown type]<span class=\"msg-dl-link\"><a href=\"media/"
                     << attachment_filename_on_disk << "\">&#129055;</a></span></div>\n";
        else
          htmloutput << std::string(indent, ' ') << "  <div>[Attachment '" << original_filename << "']<span class=\"msg-dl-link\"><a href=\"media/"
                     << attachment_filename_on_disk << "\">&#129055;</a></span></div>\n";
        // the following does not work, because URIs on file:// are cross-origin
        // << attachment_filename_on_disk << "\" download=\"" << original_filename << "\">&#129055;</a></span>\n";
      }
      else // content-type not empty, but not 'image/', 'audio/' or 'video/', or ignored as media on user-request
      {
        if (original_filename.empty())
          htmloutput << std::string(indent, ' ') << "  <div>[Attachment of type \"" << content_type << "\"]<span class=\"msg-dl-link\"><a href=\"media/"
                     << attachment_filename_on_disk << "\" type=\"" << content_type << "\">&#129055;</a></span></div>\n";
        else
          htmloutput << std::string(indent, ' ') << "  <div>[Attachment '" << original_filename << "']<span class=\"msg-dl-link\"><a href=\"media/"
                     << attachment_filename_on_disk << "\" type=\"" << content_type << "\">&#129055;</a></span></div>\n";
      }
    }
    htmloutput << std::string(indent, ' ') << "</div>\n";
  }
}

void SignalBackup::HTMLwriteSharedContactDiv(std::ofstream &htmloutput, std::string const &shared_contact, int indent,
                                             std::string const &directory, std::string const &threaddir, bool overwrite, bool append) const
{
  if (d_database.getSingleResultAs<long long int>("SELECT json_array_length(?, '$')", shared_contact, 0) > 0)
  {
    std::string contact_name = "Unknown contact";
    std::string contact_info;
    SqliteDB::QueryResults sc;
    d_database.exec("SELECT "
                    "json_extract('" + shared_contact + "', '$[0].name.displayName') AS display_name, "
                    "IFNULL(json_array_length('" + shared_contact + "', '$[0].phoneNumbers'), 0) AS num_numbers, "
                    "IFNULL(json_array_length('" + shared_contact + "', '$[0].emails'), 0) AS num_emails, "
                    "json_extract('" + shared_contact + "', '$[0].avatar.attachmentId.rowId') AS avatar_rowid, "
                    "json_extract('" + shared_contact + "', '$[0].avatar.attachmentId.uniqueId') AS avatar_uniqueid", &sc
                    );

    if (!sc("display_name").empty())
      contact_name = sc("display_name");

    long long int rowid = sc.valueAsInt(0, "avatar_rowid", -1);
    long long int uniqueid = sc.valueAsInt(0, "avatar_uniqueid", -1);
    std::string extension("bin");
    if (rowid >= 0 && uniqueid >= 0)
    {
      // write the attachment data
      HTMLwriteAttachment(directory, threaddir, rowid, uniqueid, extension, -1, overwrite, append);
    }

    // prefer phone number
    int phones = sc.valueAsInt(0, "num_numbers", 0);
    int emails = sc.valueAsInt(0, "num_emails", 0);
    if (phones > 0)
    {
      // prefer 'MOBILE' (-> 'HOME' -> 'WORK' ?)
      for (int i = 0; i < phones; ++i)
      {
        d_database.exec(bepaald::concat("SELECT "
                                        "json_extract('", shared_contact, "', '$[0].phoneNumbers[", bepaald::toString(i), "].number') AS number, "
                                        "json_extract('", shared_contact, "', '$[0].phoneNumbers[", bepaald::toString(i), "].type') AS type"), &sc);

        if (sc("type") == "CUSTOM" && contact_info.empty())
          contact_info = sc("number");
        else if (sc("type") == "WORK" && contact_info.empty())
          contact_info = sc("number");
        else if (sc("type") == "HOME")
          contact_info = sc("number");
        else if (sc("type") == "MOBILE")
        {
          contact_info = sc("number");
          break;
        }
      }
    }
    else if (emails > 0)
    {
      // prefer 'HOME' (-> 'WORK' -> 'MOBILE' ?)
      for (int i = 0; i < emails; ++i)
      {
        d_database.exec(bepaald::concat("SELECT "
                                        "json_extract('", shared_contact, "', '$[0].emails[", bepaald::toString(i), "].email') AS email, "
                                        "json_extract('", shared_contact, "', '$[0].emails[", bepaald::toString(i), "].type') AS type"), &sc);

        if (sc("type") == "CUSTOM" && contact_info.empty())
          contact_info = sc("email");
        else if (sc("type") == "OTHER" && contact_info.empty())
          contact_info = sc("email");
        else if (sc("type") == "WORK")
          contact_info = sc("email");
        else if (sc("type") == "HOME")
        {
          contact_info = sc("email");
          break;
        }
      }
    }

    //htmloutput << std::string(indent, ' ') << "<div class=\"attachment attachment-unknown-type\">\n";

    htmloutput << std::string(indent, ' ') << "<div class=\"shared-contact\">\n";
    if (rowid > -1 && uniqueid > -1)
    {
      htmloutput << std::string(indent, ' ') << "  <div class=\"shared-contact-avatar\" style=\"background-image: url('" << "media/Attachment_" << rowid << "_" << uniqueid << "." << extension << "');\">\n";
      htmloutput << std::string(indent, ' ') << "    <input type=\"checkbox\" id=\"zoomCheck-" << rowid << "-" << uniqueid << "\">\n";
      htmloutput << std::string(indent, ' ') << "    <label for=\"zoomCheck-" << rowid << "-" << uniqueid << "\">\n";
      htmloutput << std::string(indent, ' ') << "      <img src=\"media/Attachment_" << rowid << "_" << uniqueid << "." << extension << "\" alt=\"Shared avatar\">\n";
      htmloutput << std::string(indent, ' ') << "    </label>\n";
      htmloutput << std::string(indent, ' ') << "  </div>\n";
    }
    else
      htmloutput << std::string(indent, ' ') << "  <div class=\"shared-contact-avatar shared-contact-avatar-default\"></div>\n";

    htmloutput << std::string(indent, ' ') << "  <div class=\"shared-contact-info\">\n";
    htmloutput << std::string(indent, ' ') << "    <span class=\"shared-contact-name\">" << HTMLescapeString(contact_name) << "</span>\n";
    htmloutput << std::string(indent, ' ') << "    <pre>" << HTMLescapeString(contact_info) << "</pre>\n";
    htmloutput << std::string(indent, ' ') << "  </div>\n";
    htmloutput << std::string(indent, ' ') << "</div>\n";
  }
}

void SignalBackup::HTMLwriteMessage(std::ofstream &htmloutput, HTMLMessageInfo const &msg_info,
                                    std::map<long long int, RecipientInfo> *recipient_info,
                                    bool searchpage, bool writereceipts,
                                    std::vector<std::string> const &ignoremediatypes) const
{
  int extraindent = 0;
  // insert message

  long long int quote_author_id = bepaald::toNumber<long long int>(msg_info.messages->valueAsString(msg_info.idx, "quote_author"));

  htmloutput << "          <!-- Message: _id:" << msg_info.msg_id <<",type:" << msg_info.type << " -->\n";

  if (searchpage) // output an anchor to link to in search results
    htmloutput << "          <a id=\"" << msg_info.msg_id << "\"></a>\n";

  // for incoming group (normal) message: insert avatar with initial
  if (msg_info.isgroup && msg_info.incoming && !msg_info.is_deleted && !Types::isStatusMessage(msg_info.type))
  {
    htmloutput << "          <div class=\"incoming-group-msg\">\n";
    htmloutput << "            <div class=\"avatar avatar-" << msg_info.msg_recipient_id
               << " convo-avatar msg-sender-" << msg_info.msg_recipient_id << "\">";
    if (!getRecipientInfoFromMap(recipient_info, msg_info.msg_recipient_id).hasavatar)
    {
      htmloutput << '\n';
      htmloutput << "              <span>" << getRecipientInfoFromMap(recipient_info, msg_info.msg_recipient_id).initial << "</span>\n";
      htmloutput << "            ";
    }
    htmloutput << "</div>\n";
    extraindent = 2;
  }

  // msg bubble
  htmloutput << std::string(extraindent, ' ') << "          <div class=\"msg ";
  if (Types::isStatusMessage(msg_info.type))
    htmloutput << "msg-status\">\n";
  else
    htmloutput << "msg-" << (msg_info.incoming ? "incoming" : "outgoing")
               << (!msg_info.incoming ? " msg-sender-" + bepaald::toString(msg_info.msg_recipient_id) : "")
               << (msg_info.nobackground ? " no-bg-bubble" : "")
               << (msg_info.is_viewonce ? " msg-viewonce" : "")
               << ((msg_info.is_deleted && !msg_info.is_viewonce) ? " deleted-msg" : "")
               << (msg_info.reaction_results->rows() ? " msg-with-reaction" : "")<< "\">\n";

  // for incoming group (normal) message: Senders name before message content
  if (msg_info.isgroup && msg_info.incoming && !msg_info.is_deleted && !Types::isStatusMessage(msg_info.type))
    htmloutput << std::string(extraindent, ' ') << "            <span class=\"msg-name msg-name-"
               << msg_info.msg_recipient_id << "\">" << HTMLescapeString(getRecipientInfoFromMap(recipient_info, msg_info.msg_recipient_id).display_name) << "</span>\n";

  // for incoming story reply message: 'Reacted to your story' before message content
  if (msg_info.story_reply && msg_info.incoming && !msg_info.is_deleted && !Types::isStatusMessage(msg_info.type))
    htmloutput << std::string(extraindent, ' ') << "            <span class=\"msg-name\">Reacted to your story</span>\n";

  // insert quote
  if (msg_info.hasquote)
  {
    htmloutput << std::string(extraindent, ' ') << "            <div class=\"msg-quote\">\n";

    // quote message
    htmloutput << std::string(extraindent, ' ') << "              <div class=\"msg-quote-message\">\n";
    htmloutput << std::string(extraindent, ' ') << "                <span class=\"msg-name\">"
               << HTMLescapeString(getRecipientInfoFromMap(recipient_info, quote_author_id).display_name)
               << (msg_info.story_reply ? " &middot; Story" : "")
               << "</span>\n";
    if (!msg_info.quote_body.empty())
      htmloutput << std::string(extraindent, ' ') << "                <pre dir=\"auto\">" << msg_info.quote_body << "</pre>\n";
    if (msg_info.story_reply && msg_info.quote_missing)
      htmloutput << std::string(extraindent, ' ') << "                <pre>No longer available</pre>\n";
    htmloutput << std::string(extraindent, ' ') << "              </div>\n";

    // quote attachment
    if (msg_info.quote_attachment_results->rows())
    {
      htmloutput << std::string(extraindent, ' ') << "              <div class=\"msg-quote-attach\">\n";
      HTMLwriteAttachmentDiv(htmloutput, *msg_info.quote_attachment_results, 16 + extraindent,
                             msg_info.directory, msg_info.threaddir, msg_info.orig_filename,
                             false, msg_info.overwrite, msg_info.append, ignoremediatypes);
      htmloutput << "                </div>\n";
    }

    htmloutput << std::string(extraindent, ' ') << "            </div>\n";
  }

  // insert attachment?
  if (!msg_info.shared_contacts.empty()) [[unlikely]] // if we have an attachment with a shared contact, it's an avatar
    HTMLwriteSharedContactDiv(htmloutput, msg_info.shared_contacts, 12 + extraindent,
                              msg_info.directory, msg_info.threaddir, msg_info.overwrite, msg_info.append);
  else if (STRING_STARTS_WITH(msg_info.link_preview_url, "https://signal.link/call/#key=")) [[unlikely]]
    HTMLwriteCallLinkDiv(htmloutput, 12 + extraindent, msg_info.link_preview_url, msg_info.link_preview_title,
                         msg_info.link_preview_description/*, msg_info.directory, msg_info.threaddir, msg_info.overwrite,
                                                              msg_info.append*/);
  else
    HTMLwriteAttachmentDiv(htmloutput, *msg_info.attachment_results, 12 + extraindent,
                           msg_info.directory, msg_info.threaddir, msg_info.orig_filename,
                           (!msg_info.link_preview_title.empty() || !msg_info.link_preview_description.empty()),
                           msg_info.overwrite, msg_info.append, ignoremediatypes);


  // insert link_preview data? (if not call link)
  if ((!msg_info.link_preview_title.empty() || !msg_info.link_preview_description.empty()) &&
      !STRING_STARTS_WITH(msg_info.link_preview_url, "https://signal.link/call/#key="))
  {
    htmloutput << "            <div class=\"linkpreview\">\n";
    if (!msg_info.link_preview_title.empty())
    {
      htmloutput <<
        "              <div class=\"linkpreview_title\">\n"
        "                " << HTMLescapeString(msg_info.link_preview_title) << "\n"
        "              </div>\n";
    }
    std::string cleaned_link_preview_description = HTMLprepLinkPreviewDescription(msg_info.link_preview_description);
    if (!cleaned_link_preview_description.empty())
    {
      htmloutput <<
        "              <div class=\"linkpreview_description\">\n"
        "                " << cleaned_link_preview_description << "\n"
        "              </div>\n";
    }
    htmloutput << "            </div>\n";
  }

  //insert body
  if (!msg_info.body.empty())
  {
    htmloutput << std::string(extraindent, ' ') << "            <div"
               << (msg_info.only_emoji ? " class=\"msg-all-emoji\"" : "")
               << (Types::isStatusMessage(msg_info.type) ? " class=\"status-text" +
                   (Types::isMissedCall(msg_info.type) || Types::isMissedVideoCall(msg_info.type) ? " status-text-red"s : "") + "\"" : "")
               << ">\n";
    htmloutput << std::string(extraindent, ' ') << "              <pre dir=\"auto\">";
    if (Types::isEndSession(msg_info.type) || Types::isIdentityDefault(msg_info.type)) // info-icon
      htmloutput << "<span class=\"msg-info-icon\"></span>";
    else if (Types::isIdentityUpdate(msg_info.type))
      htmloutput << "<span class=\"msg-security-icon\"></span>";
    else if (Types::isIdentityVerified(msg_info.type))
      htmloutput << "<span class=\"msg-checkmark\"></span>";
    else if (Types::isGroupQuit(msg_info.type))
      htmloutput << "<span class=\"msg-group-quit-icon\"></span>";
    else if (Types::isProfileChange(msg_info.type))
      htmloutput << "<span class=\"msg-profile-icon\"></span>";
    else if (Types::isExpirationTimerUpdate(msg_info.type))
    {
      if (msg_info.body.find("disabled disappearing messages") != std::string::npos)
        htmloutput << "<span class=\"msg-expiration-timer-disabled\"></span>";
      else
        htmloutput << "<span class=\"msg-expiration-timer-set\"></span>";
    }
    else if (Types::isIncomingCall(msg_info.type))
      htmloutput << "<span class=\"msg-call-incoming\"></span>";
    else if (Types::isOutgoingCall(msg_info.type))
      htmloutput << "<span class=\"msg-call-outgoing\"></span>";
    else if (Types::isMissedCall(msg_info.type))
      htmloutput << "<span class=\"msg-call-missed\"></span>";
    else if (Types::isIncomingVideoCall(msg_info.type))
      htmloutput << "<span class=\"msg-video-call-incoming\"></span>";
    else if (Types::isOutgoingVideoCall(msg_info.type))
      htmloutput << "<span class=\"msg-video-call-outgoing\"></span>";
    else if (Types::isMissedVideoCall(msg_info.type))
      htmloutput << "<span class=\"msg-video-call-missed\"></span>";
    else if (Types::isGroupCall(msg_info.type))
      htmloutput << "<span class=\"msg-group-call\"></span>";
    else if (Types::isJoined(msg_info.type))
      htmloutput << "<span class=\"msg-member-add-icon\"></span>";
    else if (Types::isMessageRequestAccepted(msg_info.type))
      htmloutput << "<span class=\"msg-thread-icon\"></span>";
    else if (msg_info.type == Types::GV1_MIGRATION_TYPE)
    {
      if (msg_info.icon == IconType::MEMBER_ADD)
        htmloutput << "<span class=\"msg-member-add-icon\"></span>";
      else if (msg_info.icon == IconType::MEMBER_REMOVE)
        htmloutput << "<span class=\"msg-member-remove-icon\"></span>";
      // dont know, never seen this...
      // else if (msg_info.icon == IconType::MEMBERS)
      //   htmloutput << "<span class=\"msg-members-icon\"></span>";
      else
        htmloutput << "<span class=\"msg-megaphone-icon\"></span>";
    }
    else if (Types::isGroupUpdate(msg_info.type) && !Types::isGroupV2(msg_info.type))
      htmloutput << "<span class=\"msg-members-icon\"></span>";
    else if (Types::isNumberChange(msg_info.type))
      htmloutput << "<span class=\"msg-phone-icon\"></span>";

    // group v2 status msgs
    else if (Types::isGroupV2(msg_info.type) && msg_info.icon == IconType::TIMER_UPDATE)
      htmloutput << "<span class=\"msg-expiration-timer-set\"></span>";
    else if (Types::isGroupV2(msg_info.type) && msg_info.icon == IconType::TIMER_DISABLE)
      htmloutput << "<span class=\"msg-expiration-timer-disabled\"></span>";
    else if (Types::isGroupV2(msg_info.type) && msg_info.icon == IconType::PENCIL)
      htmloutput << "<span class=\"msg-pencil-icon\"></span>";
    else if (Types::isGroupV2(msg_info.type) && msg_info.icon == IconType::THREAD)
      htmloutput << "<span class=\"msg-thread-icon\"></span>";
    else if (Types::isGroupV2(msg_info.type) && msg_info.icon == IconType::MEGAPHONE)
      htmloutput << "<span class=\"msg-megaphone-icon\"></span>";
    else if (Types::isGroupV2(msg_info.type) && msg_info.icon == IconType::MEMBERS)
      htmloutput << "<span class=\"msg-members-icon\"></span>";
    else if (Types::isGroupV2(msg_info.type) && msg_info.icon == IconType::MEMBER_APPROVED)
      htmloutput << "<span class=\"msg-member-approved-icon\"></span>";
    else if (Types::isGroupV2(msg_info.type) && msg_info.icon == IconType::MEMBER_REJECTED)
      htmloutput << "<span class=\"msg-member-rejected-icon\"></span>";
    else if (Types::isGroupV2(msg_info.type) && msg_info.icon == IconType::MEMBER_ADD)
      htmloutput << "<span class=\"msg-member-add-icon\"></span>";
    else if (Types::isGroupV2(msg_info.type) && msg_info.icon == IconType::MEMBER_REMOVE)
      htmloutput << "<span class=\"msg-member-remove-icon\"></span>";
    else if (Types::isGroupV2(msg_info.type) && msg_info.icon == IconType::AVATAR_UPDATE)
      htmloutput << "<span class=\"msg-avatar-update-icon\"></span>";

    // else if (Types::isGroupV2(msg_info.type) && msg_info.icon == IconType::)
    //   htmloutput << "<span class=\"msg-\"></span>";


    htmloutput << msg_info.body << "</pre>\n";
    htmloutput << std::string(extraindent, ' ') << "            </div>\n";
  }
  else if (msg_info.is_viewonce)
  {
    htmloutput << "            <div class=\"viewonce\">\n";
    if (msg_info.incoming)
      htmloutput << "              <div class=\""
                 << (msg_info.is_deleted ? "viewonce_icon" : "unviewed_viewonce_icon") << "\"></div><pre>"
                 << (msg_info.is_deleted ? "Viewed" : "View-once media")
                 << "</pre>\n";
    else
      htmloutput << "              <div class=\"viewonce_icon\"></div><pre>Media</pre>\n";
    htmloutput << "            </div>\n";
  }
  else if (msg_info.is_deleted)
  {
    htmloutput << "            <div>\n";
    if (msg_info.incoming)
      htmloutput << "              <pre>This message was deleted.</pre>\n";
    else
      htmloutput << "              <pre>You deleted this message.</pre>\n";
    htmloutput << "            </div>\n";
  }

  // insert msg-footer (date & checkmarks)
  htmloutput << std::string(extraindent, ' ') << "            <div class=\"footer" << (Types::isStatusMessage(msg_info.type) ? "-status" : "") << "\">\n";
  if (msg_info.original_message_id != -1) // : is_edited = true
  {
    htmloutput << std::string(extraindent, ' ') << "              <div class=\"edited\"><span>edited</span>";
    // add edit revisions...
    if (msg_info.edit_revisions->rows())
    {
      htmloutput << "<div class=\"edited-info\">";
      for (unsigned int i = 0; i < msg_info.edit_revisions->rows() - 1; ++i) // -1, skip last one: it is the current message
      {

        if (i == 0)
          htmloutput << "<div class=\"history-header\">Edit history</div>";

        // add earlier revision
        HTMLwriteRevision(msg_info.edit_revisions->valueAsInt(i, "_id"), htmloutput, msg_info, recipient_info, false, ignoremediatypes);

        if (i < msg_info.edit_revisions->rows() - 2)
          htmloutput << "<hr>";

        // htmloutput << "<span class=\"edited-info-header\">body: </span>"
        //            << "<pre>" << msg_info.edit_revisions->valueAsString(i, "body") << "</pre>" << "<br>"
        //            << "<span class=\"edited-info-header\">date: </span>"
        //            << bepaald::toDateString(msg_info.edit_revisions->valueAsInt(i, d_mms_date_sent) / 1000,
        //                                     "%b %d, %Y %H:%M:%S");
        // if (i == 0)
        //   htmloutput << "<div class=\"history-header\">Edit history</div>";
        // else if (i < msg_info.edit_revisions->rows() - 1)
        //   htmloutput << "<hr>";
      }
      htmloutput << "</div>\n";
    }
    htmloutput << "</div>\n";
  }
  htmloutput << std::string(extraindent, ' ') << "              <span class=\"msg-data\">" << msg_info.readable_date << "</span>\n";
  if (!Types::isStatusMessage(msg_info.type))
  {
    if (msg_info.expires_in > 0)
      htmloutput << std::string(extraindent, ' ') << "              <div class=\"footer-icons is-expiring\"></div>\n";
    if (!msg_info.incoming && !Types::isCallType(msg_info.type) && !msg_info.is_deleted) // && received, read?
    {
      htmloutput << std::string(extraindent, ' ') << "              <div class=\"footer-icons checkmarks-";
      if (msg_info.messages->getValueAs<long long int>(msg_info.idx, d_mms_read_receipts) > 0)
        htmloutput << "read";
      else if (msg_info.messages->getValueAs<long long int>(msg_info.idx, d_mms_delivery_receipts) > 0)
        htmloutput << "received";
      else // if something? type != failed? -> check for failed before outputting 'checkmarks-'
        htmloutput << "sent";
      htmloutput << "\">\n";
      // msg receipt details
      if (writereceipts &&
          (msg_info.messages->valueAsInt(msg_info.idx, d_mms_delivery_receipts, -1) > 0 ||
           msg_info.messages->valueAsInt(msg_info.idx, d_mms_read_receipts, -1) > 0))
        HTMLwriteMsgReceiptInfo(htmloutput, recipient_info, msg_info.msg_id, msg_info.isgroup,
                                msg_info.messages->valueAsInt(msg_info.idx, d_mms_read_receipts, 0),
                                msg_info.messages->valueAsInt(msg_info.idx, d_mms_delivery_receipts, 0),
                                msg_info.messages->valueAsInt(msg_info.idx, "receipt_timestamp", -1),
                                extraindent);
      htmloutput << std::string(extraindent, ' ') << "              </div>\n";
    }
  }
  htmloutput << std::string(extraindent, ' ') << "            </div>\n";

  // insert reaction
  if (msg_info.reaction_results->rows())
  {
    htmloutput << std::string(extraindent, ' ') << "            <div class=\"msg-reactions\">\n";


    std::set<std::string> skip;
    for (unsigned int r = 0; r < msg_info.reaction_results->rows(); ++r)
    {

      std::string emojireaction = msg_info.reaction_results->valueAsString(r, "emoji");

      if (bepaald::contains(skip, emojireaction))
        continue;

      skip.insert(emojireaction);

      // count occurences of this emoji, and set info
      int count = 0;
      std::string reaction_info;
      for (unsigned int r2 = r; r2 < msg_info.reaction_results->rows(); ++r2)
        if (emojireaction == msg_info.reaction_results->valueAsString(r2, "emoji"))
        {
          ++count;
          reaction_info += (reaction_info.empty() ? "" : "<hr>") + "From: "s + getRecipientInfoFromMap(recipient_info, msg_info.reaction_results->getValueAs<long long int>(r2, "author_id")).display_name +
            "<br>Sent: " + msg_info.reaction_results->valueAsString(r2, "date_sent") +
            "<br>Received: " + msg_info.reaction_results->valueAsString(r2, "date_received");
        }

      htmloutput << std::string(extraindent, ' ') << "              <div class=\"msg-reaction\"><span class=\"msg-emoji\">"
                 << emojireaction << "</span>" << (count > 1 ? "<span class=\"reaction-count\">" + bepaald::toString(count) + "</span>": "")
                 << "<div class=\"msg-reaction-info\">" << reaction_info << "</div></div>\n";
    }
    htmloutput << std::string(extraindent, ' ') << "            </div>\n";
  }
  // end message
  htmloutput << std::string(extraindent, ' ') << "          </div>\n";
  if (msg_info.isgroup && msg_info.incoming && !msg_info.is_deleted && !Types::isStatusMessage(msg_info.type))
    htmloutput << "          </div>\n";
  htmloutput << '\n';

  if (d_verbose) [[unlikely]]
    htmloutput << std::flush;
}
