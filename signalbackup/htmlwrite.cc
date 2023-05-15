/*
 *     IMPORTANT LICENSE NOTICE
 *
 *   The HTML and CSS produced by these functions is modified from the template
 *   found in https://github.com/GjjvdBurg/signal2html.
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

bool SignalBackup::HTMLwriteStart(std::ofstream &file, long long int thread_recipient_id,
                                  std::string const &directory, std::string const &threaddir, bool isgroup,
                                  bool isnotetoself, std::set<long long int> const &recipient_ids,
                                  std::map<long long int, RecipientInfo> *recipient_info,
                                  std::map<long long int, std::string> *written_avatars,
                                  bool overwrite, bool append, bool light) const
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
  file << "<!-- Generated on " << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S")
       << " by signalbackup-tools (" << VERSIONDATE << "). "
       << "Input database version: " << d_databaseversion << ". -->" << std::endl;

  file << R"(<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <title>)" << (isnotetoself ? "Note to self" : getRecipientInfoFromMap(recipient_info, thread_recipient_id).display_name) << R"(</title>
    <style>

      body {
        background-color: )" << (light ? "#EDF0F6" : "#000000") << R"(;
        margin: 0px;
        display: flex;
      }

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
        height: 100%;
      }

      #message-header {
        text-align: center;
        color: )" << (light ? "black" : "white") << R"(;
        font-family: Roboto, "Noto Sans", "Liberation Sans", OpenSans, sans-serif;
        padding-top: 30px;
        padding-bottom: 30px;
      }

      #thread-title {
        font-size: x-large;
      }

      .conversation-box {
        display: flex;
        flex-direction: column;
        padding-left: 30px;
        padding-right: 30px;
        padding-bottom: 30px;
        margin-bottom: 30px;
        background-color: )" << (light ? "#FBFCFF" : "#1B1C1F") << R"(;
        color: )" << (light ? "black" : "white") << R"(;
        font-family: Roboto, "Noto Sans", "Liberation Sans", OpenSans, sans-serif;
        border-radius: 10px;
        width: calc(100% - 60px);
      }

      .incoming-group-msg {
        display: flex;
        flex-direction: row;
      }

      .msg-incoming {
        align-self: flex-start;
        background: )" << (light ? "#E7EBF3" : "#303133") << R"(;
      }

)";

  for (long long int id : recipient_ids)
    file << "      .msg-sender-" << id << " { background: #" << getRecipientInfoFromMap(recipient_info, id).color << ";}" << std::endl;
  file << std::endl;
  for (long long int id : recipient_ids)
    file << "      .msg-name-" << id << " { color: #" << getRecipientInfoFromMap(recipient_info, id).color << ";}" << std::endl;

  file << R"(
      .msg-outgoing {
        align-self: flex-end;
        background: #)" << (isgroup ? s_html_colormap.at("ULTRAMARINE") : getRecipientInfoFromMap(recipient_info, thread_recipient_id).color) << ";" << std::endl;
  if (light)
    file << "        color: white;" << std::endl;
  file << R"(      }

      .deleted-msg {
        background: rgba(0, 0, 0, 0);
        border: 1px solid )" << (light ? "black" : "white") << ";" << std::endl;
  if (light)
    file << "        color: black;" << std::endl;
  file << R"(      }

      .avatar {
        font-weight: 500;
        border-radius: 50% 50%;
        aspect-ratio: 1 / 1;
        text-align: center;)" << std::endl;
  if (light)
    file << "        color: white;" << std::endl;
file << R"(      }

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
        background-size: cover;)" << std::endl;
      if (light)
        file << "        color: white;" << std::endl;
file << R"(      }
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
        transition: 0.25s ease;
      }

      .note-to-self-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg" width="80" height="80" viewBox="0 0 80 80" fill="white"><path d="M58,7.5A6.51,6.51 0,0 1,64.5 14L64.5,66A6.51,6.51 0,0 1,58 72.5L22,72.5A6.51,6.51 0,0 1,15.5 66L15.5,14A6.51,6.51 0,0 1,22 7.5L58,7.5M58,6L22,6a8,8 0,0 0,-8 8L14,66a8,8 0,0 0,8 8L58,74a8,8 0,0 0,8 -8L66,14a8,8 0,0 0,-8 -8ZM60,24L20,24v1.5L60,25.5ZM60,34L20,34v1.5L60,35.5ZM60,44L20,44v1.5L60,45.5ZM50,54L20,54v1.5L50,55.5Z"></path></svg>');
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
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg" width="40" height="40" viewBox="0 0 40 40" fill="white"><path d="M29,16.75a6.508,6.508 0,0 1,6.5 6.5L35.5,24L37,24v-0.75a8,8 0,0 0,-6.7 -7.885,6.5 6.5,0 1,0 -8.6,0 7.941,7.941 0,0 0,-2.711 0.971A6.5,6.5 0,1 0,9.7 25.365,8 8,0 0,0 3,33.25L3,34L4.5,34v-0.75a6.508,6.508 0,0 1,6.5 -6.5h6a6.508,6.508 0,0 1,6.5 6.5L23.5,34L25,34v-0.75a8,8 0,0 0,-6.7 -7.885,6.468 6.468,0 0,0 1.508,-7.771A6.453,6.453 0,0 1,23 16.75ZM14,25.5a5,5 0,1 1,5 -5A5,5 0,0 1,14 25.5ZM21,10.5a5,5 0,1 1,5 5A5,5 0,0 1,21 10.5Z"></path></svg>');
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
  if (!thread_avatar.empty())
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
        z-index: 1;
        cursor: zoom-out;
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
        color: white;
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

      .msg-data {
        font-size: x-small;
        opacity: 75%;
        display: block;
      }

      .checkmarks {
        margin-left: 5px;
      }

      .checkmarks-sent {
          height: 14px;
          width: 14px;
          background-image: url('data:image/svg+xml;utf-8,<svg xmlns="http://www.w3.org/2000/svg" width="14" height="14" viewBox="0 0 24 24" fill="white" stroke="white"><path d="M12,2.5A9.5,9.5 0,1 1,2.5 12,9.511 9.511,0 0,1 12,2.5M12,1A11,11 0,1 0,23 12,11 11,0 0,0 12,1ZM17.834,9.4L16.773,8.338l-6.541,6.541 -3,-3 -1.061,1.06L10.232,17Z"></path></svg>');
      }

      .checkmarks-received {
          height: 12px;
          width: 19px;
          background-image: url('data:image/svg+xml;utf-8,<svg xmlns="http://www.w3.org/2000/svg" width="19" height="11.9999" viewBox="0 0 34.833624 22" fill="white" stroke="white"><path d="M 11.000092 0 A 11 11 0 1 0 15.268357 21.069186 A 12.375103 12.375103 0 0 1 13.4923 19.744305 A 9.5 9.5 0 0 1 1.500338 11.000092 A 9.511 9.511 0 0 1 11.000092 1.500338 A 9.5 9.5 0 0 1 12.729598 2.1054863 A 12.375103 12.375103 0 0 1 14.473428 0.56934069 A 11 11 0 0 0 11.000092 0 z M 6.2305207 10.878346 L 5.1706161 11.938251 L 9.2311968 15.998831 L 10.212325 15.017703 A 12.375103 12.375103 0 0 1 9.7253416 13.384877 L 9.2311968 13.879022 L 6.2305207 10.878346 z"></path><path d="M 22.183427,1.5 A 9.5,9.5 0 1 1 12.683423,11 9.511,9.511 0 0 1 22.183427,1.5 m 0,-1.5 a 11,11 0 1 0 11,11 11,11 0 0 0 -11,-11 z m 5.834,8.3999998 -1.061,-1.062 -6.541,6.5410002 -3.000002,-3 -1.061001,1.06 4.061003,4.061 z"></path></svg>');
      }

      .checkmarks-read {
          height: 12px;
          width: 19px;
          background-image: url('data:image/svg+xml;utf-8,<svg xmlns="http://www.w3.org/2000/svg" width="19" height="12" viewBox="0 0 34.833627 22.000184" fill="white" stroke="none"><path d="M 11.000092 0 A 11 11 0 1 0 16.181451 20.693208 A 12.375104 12.375104 0 0 1 11.74131 13.492301 L 9.2311973 15.998832 L 5.1670356 11.934671 L 6.2269403 10.874766 L 9.2276165 13.875442 L 11.512141 11.590918 A 12.375104 12.375104 0 0 1 11.45843 11.000092 A 12.375104 12.375104 0 0 1 16.188612 1.3033964 A 11 11 0 0 0 11.000092 0 z"></path><path d="m 23.833441,0 a 11,11 0 1 0 11,11 11,11 0 0 0 -11,-11 z m -1.768001,16 -4.066003,-4.066 1.061001,-1.06 3.000002,3 6.541001,-6.541 1.066,1.067 z"></path></svg>');
      }

      .msg-emoji {
        font-family: "Apple Color Emoji", "Noto Color Emoji", sans-serif;
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
)";
  if (light)
  {
    file << R"(
      .no-bg-bubble .footer {
        color: black;
      }

      .no-bg-bubble .checkmarks-sent,
      .no-bg-bubble .checkmarks-read,
      .no-bg-bubble .checkmarks-received {
        filter: brightness(0);
      }
)";
  }
  file << R"(
      .mention-in {
        background-color: )" << (light ? "#C6C6C6" : "#5E5E5E") << R"(;
      }

      .mention-out {
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
        color: white;
        padding-left: 5px;
      }

      .msg-name {
        font-weight: bold;
        font-size: smaller;
        margin-bottom: 5px;
        display: block;
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

      .msg-img-container input[type=checkbox],
      .msg-linkpreview-img-container input[type=checkbox],
      #thread-subtitle input[type=checkbox] {
        display: none;
      }

      .msg-img-container img {
        transition: transform 0.25s ease;
        border-radius: 0.6em;
        cursor: zoom-in;
      }

      .msg-linkpreview-img-container img {
        transition: transform 0.25s ease;
        border-top-left-radius: 0.6em;
        border-top-right-radius: 0.6em;
        border-bottom-left-radius: 0em;
        border-bottom-right-radius: 0em;
        cursor: zoom-in;
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

      .msg-img-container input[type=checkbox]:checked ~ label > img,
      .msg-linkpreview-img-container input[type=checkbox]:checked ~ label > img {
        transform: scale(2.5);
        border-radius: 0;
        cursor: zoom-out;
        z-index: 1;
        position: relative;
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
      }

      .msg-incoming .attachment-unknown-type {
        background-color: rgba(255, 255, 255, .16);
      }

      .msg-outgoing .attachment-unknown-type {
        background-color: rgba(0, 0, 0, 0.244);
      }

      .msg-with-reaction {
        margin-bottom: 20px;
        padding-bottom: 15px;
      }

      .msg-reactions {
        margin-top: 5px;
        text-align: center;
        position: absolute;
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
        background-color: )" << (light ? "#E7EBF3" : "#303133") << R"(;
        border-radius: 13px;
        border: 1px solid )" << (light ? "#FBFCFF" : "#1B1C1F") << R"(;
        line-height: 150%;
        position: relative;
      }

      .reaction-count {
        color: )" << (light ? "black" : "white") << R"(;
        margin-left: 5px;
      }

      .msg-reaction .msg-reaction-info {
        display: block;
        position: absolute;
        z-index: 1;
        visibility: hidden;
        width: 250px;
        background-color: #505050;
        color: white;
        text-align: center;
        padding: 5px;
        border: 1px solid white;
        border-radius: 3px;
        margin-left: -131px;
        bottom: 125%;
        left: 50%;
        opacity: 0;
        transition: opacity 0.2s;
      }

      /* Draw an arrow using border styles */
      .msg-reaction .msg-reaction-info::before {
        content: "";
        position: absolute;
        top: 100%;
        left: 50%;
        margin-left: -5px;
        border-width: 5px;
        border-style: solid;
        border-color: white transparent transparent transparent;
      }

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
        border-left: 5px solid white;
      }

      .msg-incoming .msg-quote,
      .msg-incoming .msg-linkpreview-img-container,
      .msg-incoming .linkpreview {
        background-color: rgba(255, 255, 255, )" << (light ? ".5" : ".16") << R"();
      }

      .msg-outgoing .msg-quote,
      .msg-outgoing .msg-linkpreview-img-container,
      .msg-outgoing .linkpreview {
        background-color: rgba(255, 255, 255, .485);
        color: black;
      }

      .msg-quote-message {
        padding-left: 5px;
      }

      .msg-quote-attach {
        flex-grow: 1;
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

      .msg-status,
      .msg-group-update-v1 {
        background: none;
        align-self: center;
      }

      .msg-status div,
      .msg-group-update-v1 div{
        background: none;
        text-align: center;
      }

      .msg-group-update-v1, .msg-group-update-v2 {
        max-width: 80%;
      }

      .msg-status {
        max-width: 70%;
      }

      .msg-status div.status-text {
        align-items: center;
        display: flex;
      }

      .msg-status div.status-text-red {
        color: red;
      }

      .msg-status .msg-video-call-missed {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg" viewBox="0 0 14.117647 14.117647" stroke="none" fill="red"><path d="m 13.288805,3.2188235 c -0.2276,-0.097 -0.4791,-0.1231 -0.7217,-0.0749 -0.2426,0.0482 -0.465,0.1684 -0.6383,0.3449 l -1.87,1.86 v -1.54 c -0.0026,-0.4633 -0.1877997,-0.9069 -0.5154997,-1.2345 -0.3276,-0.3277 -0.7712,-0.5129 -1.2345,-0.5155 h -6.5 c -0.4633,0.0026 -0.90689998,0.1878 -1.23449998,0.5155 -0.3277,0.3276 -0.5129,0.7712 -0.5155,1.2345 v 6.5000005 c 0.0026,0.4633 0.1878,0.9069 0.5155,1.2345 0.3276,0.3277 0.77119998,0.5129 1.23449998,0.5155 h 6.5 c 0.4633,-0.0026 0.9069,-0.1878 1.2345,-0.5155 0.3277,-0.3276 0.5128997,-0.7712 0.5154997,-1.2345 V 8.7688235 l 1.87,1.8600005 c 0.1146,0.1172 0.2515,0.2103 0.4026,0.2739 0.1512,0.0635 0.3135,0.0962 0.4774,0.0961 0.1652,6e-4 0.3288,-0.0334 0.48,-0.1 0.2289,-0.0923 0.4248,-0.2513 0.5621,-0.4564 0.1373,-0.2051 0.2098,-0.4468005 0.2079,-0.6936005 v -5.38 c 0.0019,-0.2468 -0.0706,-0.4885 -0.2079,-0.6936 -0.1373,-0.2051 -0.3332,-0.3641 -0.5621,-0.4564 z M 9.0588053,10.308824 c -0.0026,0.1981 -0.0824,0.3874 -0.2225,0.5275 -0.1401,0.1401 -0.3294,0.2199 -0.5275,0.2225 h -6.5 c -0.1981,-0.0026 -0.3874,-0.0824 -0.5275,-0.2225 -0.1401,-0.1401 -0.2199,-0.3294 -0.2225,-0.5275 V 3.8088235 c 0.0026,-0.1981 0.0824,-0.3874 0.2225,-0.5275 0.1401,-0.1401 0.3294,-0.2199 0.5275,-0.2225 h 6.5 c 0.1981,0.0026 0.3874,0.0824 0.5275,0.2225 0.1401,0.1401 0.2199,0.3294 0.2225,0.5275 z M 13.058805,9.7488235 c 2e-4,0.0488 -0.0139,0.0966 -0.0406,0.1374 -0.0267,0.0409 -0.0647,0.0731 -0.1094,0.0926 -0.0465,0.0198 -0.0977,0.0256 -0.1474,0.0167 -0.0498,-0.0089 -0.0958,-0.0321 -0.1326,-0.0667 l -2.57,-2.58 v -0.58 l 2.57,-2.58 c 0.0418,-0.0267 0.0904,-0.0409 0.14,-0.0409 0.0496,0 0.0982,0.0142 0.14,0.0409 0.0447,0.0195 0.0827,0.0517 0.1094,0.0926 0.0267,0.0408 0.0408,0.0886 0.0406,0.1374 z m -7.2899997,-2.69 2.14,2.15 -0.7,0.7 -2.15,-2.14 -2.15,2.14 -0.7,-0.7 2.14,-2.15 -2.14,-2.15 0.7,-0.7 2.15,2.14 2.15,-2.14 0.7,0.7 z"></path></svg>');
        display: inline-block;
        height: 18px;
        aspect-ratio: 1 / 1;
        margin-right: 10px;
        margin-bottom: 5px;
      }

      .msg-status .msg-video-call-incoming {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg" viewBox="0 0 14.117647 14.117647" stroke="none" fill=")" << (light ? "black" : "white") << R"("><path d="m 13.288805,3.2188234 c -0.2276,-0.097 -0.4791,-0.1231 -0.7217,-0.0749 -0.2426,0.0482 -0.465,0.1684 -0.6383,0.3449 l -1.87,1.86 v -1.54 c -0.0026,-0.4633 -0.1877996,-0.9069 -0.5154999,-1.2345 -0.3276,-0.3277 -0.7712,-0.5129 -1.2345,-0.5155 h -6.5 c -0.4633,0.0026 -0.90689997,0.1878 -1.23449997,0.5155 -0.32769998,0.3276 -0.51289998,0.7712 -0.51549998,1.2345 v 6.5000006 c 0.0026,0.4633 0.1878,0.9069 0.51549998,1.2345 0.3276,0.3277 0.77119997,0.5129 1.23449997,0.5155 h 6.5 c 0.4633,-0.0026 0.9069,-0.1878 1.2345,-0.5155 0.3277003,-0.3276 0.5128999,-0.7712 0.5154999,-1.2345 V 8.7688234 l 1.87,1.8600006 c 0.1146,0.1172 0.2515,0.2103 0.4026,0.2739 0.1512,0.0635 0.3135,0.0962 0.4774,0.0961 0.1652,6e-4 0.3288,-0.0334 0.48,-0.1 0.2289,-0.0923 0.4248,-0.2513 0.5621,-0.4564 0.1373,-0.2051 0.2098,-0.4468003 0.2079,-0.6936003 V 4.3688234 c 0.0019,-0.2468 -0.0706,-0.4885 -0.2079,-0.6936 -0.1373,-0.2051 -0.3332,-0.3641 -0.5621,-0.4564 z M 9.0588051,10.308824 c -0.0026,0.1981 -0.0824,0.3874 -0.2225,0.5275 -0.1401,0.1401 -0.3294,0.2199 -0.5275,0.2225 h -6.5 c -0.1981,-0.0026 -0.3874,-0.0824 -0.5275,-0.2225 -0.1401,-0.1401 -0.2199,-0.3294 -0.2225,-0.5275 V 3.8088234 c 0.0026,-0.1981 0.0824,-0.3874 0.2225,-0.5275 0.1401,-0.1401 0.3294,-0.2199 0.5275,-0.2225 h 6.5 c 0.1981,0.0026 0.3874,0.0824 0.5275,0.2225 0.1401,0.1401 0.2199,0.3294 0.2225,0.5275 z M 13.058805,9.7488237 c 2e-4,0.0488 -0.0139,0.0966 -0.0406,0.1374 -0.0267,0.0409 -0.0647,0.0731 -0.1094,0.0926 -0.0465,0.0198 -0.0977,0.0256 -0.1474,0.0167 -0.0498,-0.0089 -0.0958,-0.0321 -0.1326,-0.0667 l -2.57,-2.5800003 v -0.58 l 2.57,-2.58 c 0.0418,-0.0267 0.0904,-0.0409 0.14,-0.0409 0.0496,0 0.0982,0.0142 0.14,0.0409 0.0447,0.0195 0.0827,0.0517 0.1094,0.0926 0.0267,0.0408 0.0408,0.0886 0.0406,0.1374 z M 3.7688051,9.0588234 h 3.29 v 1.0000006 h -5 V 5.0588234 h 1 v 3.29 l 4.15,-4.14 0.7,0.7 z"></path></svg>');
      }

      .msg-status .msg-video-call-outgoing {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg" viewBox="0 0 14.117647 14.117647" stroke="none" fill=")" << (light ? "black" : "white") << R"("><path d="m 13.288805,3.2188235 c -0.2276,-0.097 -0.4791,-0.1231 -0.7217,-0.0749 -0.2426,0.0482 -0.465,0.1684 -0.6383,0.3449 l -1.87,1.86 v -1.54 c -0.0026,-0.4633 -0.1877997,-0.9069 -0.5154997,-1.2345 -0.3276,-0.3277 -0.7712,-0.5129 -1.2345,-0.5155 h -6.5 c -0.4633,0.0026 -0.90689998,0.1878 -1.23449998,0.5155 -0.3277,0.3276 -0.5129,0.7712 -0.5155,1.2345 v 6.5000005 c 0.0026,0.4633 0.1878,0.9069 0.5155,1.2345 0.3276,0.3277 0.77119998,0.5129 1.23449998,0.5155 h 6.5 c 0.4633,-0.0026 0.9069,-0.1878 1.2345,-0.5155 0.3277,-0.3276 0.5128997,-0.7712 0.5154997,-1.2345 V 8.7688235 l 1.87,1.8600005 c 0.1146,0.1172 0.2515,0.2103 0.4026,0.2739 0.1512,0.0635 0.3135,0.0962 0.4774,0.0961 0.1652,6e-4 0.3288,-0.0334 0.48,-0.1 0.2289,-0.0923 0.4248,-0.2513 0.5621,-0.4564 0.1373,-0.2051 0.2098,-0.4468005 0.2079,-0.6936005 v -5.38 c 0.0019,-0.2468 -0.0706,-0.4885 -0.2079,-0.6936 -0.1373,-0.2051 -0.3332,-0.3641 -0.5621,-0.4564 z M 9.0588053,10.308824 c -0.0026,0.1981 -0.0824,0.3874 -0.2225,0.5275 -0.1401,0.1401 -0.3294,0.2199 -0.5275,0.2225 h -6.5 c -0.1981,-0.0026 -0.3874,-0.0824 -0.5275,-0.2225 -0.1401,-0.1401 -0.2199,-0.3294 -0.2225,-0.5275 V 3.8088235 c 0.0026,-0.1981 0.0824,-0.3874 0.2225,-0.5275 0.1401,-0.1401 0.3294,-0.2199 0.5275,-0.2225 h 6.5 c 0.1981,0.0026 0.3874,0.0824 0.5275,0.2225 0.1401,0.1401 0.2199,0.3294 0.2225,0.5275 z M 13.058805,9.7488235 c 2e-4,0.0488 -0.0139,0.0966 -0.0406,0.1374 -0.0267,0.0409 -0.0647,0.0731 -0.1094,0.0926 -0.0465,0.0198 -0.0977,0.0256 -0.1474,0.0167 -0.0498,-0.0089 -0.0958,-0.0321 -0.1326,-0.0667 l -2.57,-2.58 v -0.58 l 2.57,-2.58 c 0.0418,-0.0267 0.0904,-0.0409 0.14,-0.0409 0.0496,0 0.0982,0.0142 0.14,0.0409 0.0447,0.0195 0.0827,0.0517 0.1094,0.0926 0.0267,0.0408 0.0408,0.0886 0.0406,0.1374 z m -4.9999997,-5.69 v 5 h -1 v -3.29 l -4.15,4.14 -0.7,-0.7 4.14,-4.15 h -3.29 v -1 z"></path></svg>');
      }

      .msg-status .msg-group-call {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg" viewBox="0 0 16 16" stroke="none" fill=")" << (light ? "black" : "white") << R"("><path d="M14.23,4.16a1.23,1.23 0,0 0,-1.36 0.27L11,6.29L11,4.75A1.76,1.76 0,0 0,9.25 3L2.75,3A1.76,1.76 0,0 0,1 4.75v6.5A1.76,1.76 0,0 0,2.75 13h6.5A1.76,1.76 0,0 0,11 11.25L11,9.71l1.87,1.86a1.23,1.23 0,0 0,0.88 0.37,1.18 1.18,0 0,0 0.48,-0.1A1.23,1.23 0,0 0,15 10.69L15,5.31A1.23,1.23 0,0 0,14.23 4.16ZM10,11.25a0.76,0.76 0,0 1,-0.75 0.75L2.75,12A0.76,0.76 0,0 1,2 11.25L2,4.75A0.76,0.76 0,0 1,2.75 4h6.5a0.76,0.76 0,0 1,0.75 0.75ZM14,10.69a0.25,0.25 0,0 1,-0.15 0.23,0.26 0.26,0 0,1 -0.28,-0.05L11,8.29L11,7.71l2.57,-2.58a0.26,0.26 0,0 1,0.28 0,0.25 0.25,0 0,1 0.15,0.23Z"></path></svg>');
      }

      .msg-status .msg-call-incoming {
        background-image: url('data:image/svg+xml;utf-8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke=")" << (light ? "black" : "white") << R"(" stroke-width="2" ><polyline points="16 2 16 8 22 8"></polyline><line x1="23" y1="1" x2="16" y2="8"></line><path d="M22 16.92v3a2 2 0 0 1-2.18 2 19.79 19.79 0 0 1-8.63-3.07 19.5 19.5 0 0 1-6-6 19.79 19.79 0 0 1-3.07-8.67A2 2 0 0 1 4.11 2h3a2 2 0 0 1 2 1.72 12.84 12.84 0 0 0 .7 2.81 2 2 0 0 1-.45 2.11L8.09 9.91a16 16 0 0 0 6 6l1.27-1.27a2 2 0 0 1 2.11-.45 12.84 12.84 0 0 0 2.81.7A2 2 0 0 1 22 16.92z"></path></svg>');
      }

      .msg-status .msg-call-missed {
        background-image: url('data:image/svg+xml;utf-8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke="red" stroke-width="2"><line x1="23" y1="1" x2="17" y2="7"></line><line x1="17" y1="1" x2="23" y2="7"></line><path d="M22 16.92v3a2 2 0 0 1-2.18 2 19.79 19.79 0 0 1-8.63-3.07 19.5 19.5 0 0 1-6-6 19.79 19.79 0 0 1-3.07-8.67A2 2 0 0 1 4.11 2h3a2 2 0 0 1 2 1.72 12.84 12.84 0 0 0 .7 2.81 2 2 0 0 1-.45 2.11L8.09 9.91a16 16 0 0 0 6 6l1.27-1.27a2 2 0 0 1 2.11-.45 12.84 12.84 0 0 0 2.81.7A2 2 0 0 1 22 16.92z"></path></svg>');
      }

      .msg-status .msg-call-outgoing {
        background-image: url('data:image/svg+xml;utf-8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke=")" << (light ? "black" : "white") << R"(" stroke-width="2"><polyline points="23 7 23 1 17 1"></polyline><line x1="16" y1="8" x2="23" y2="1"></line><path d="M22 16.92v3a2 2 0 0 1-2.18 2 19.79 19.79 0 0 1-8.63-3.07 19.5 19.5 0 0 1-6-6 19.79 19.79 0 0 1-3.07-8.67A2 2 0 0 1 4.11 2h3a2 2 0 0 1 2 1.72 12.84 12.84 0 0 0 .7 2.81 2 2 0 0 1-.45 2.11L8.09 9.91a16 16 0 0 0 6 6l1.27-1.27a2 2 0 0 1 2.11-.45 12.84 12.84 0 0 0 2.81.7A2 2 0 0 1 22 16.92z"></path></svg>');
        transform: scale(-1, 1);
      }

      .msg-status .msg-info-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg"  viewBox="0 0 24 24" fill=")" << (light ? "black" : "white") << R"(" stroke="none"><path d="M12,2.5A9.5,9.5 0,1 1,2.5 12,9.511 9.511,0 0,1 12,2.5M12,1A11,11 0,1 0,23 12,11 11,0 0,0 12,1ZM12,8.5A1.5,1.5 0,0 0,13.5 7a1.5,1.5 0,1 0,-2.56 1.06A1.435,1.435 0,0 0,12 8.5ZM13,16.5L13,10L9.5,10v1.5h2v5L9,16.5L9,18h6L15,16.5Z"></path></svg>');
      }

      .msg-status .msg-security-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill=")" << (light ? "black" : "white") << R"(" stroke="none"><path d="M21.793,7.888A19.35,19.35 0,0 1,12 23C7.6,20.4 2,15.5 2,4.5 9,4.5 12,1 12,1s2.156,2.5 7.05,3.268L17.766,5.553A14.7,14.7 0,0 1,12 3,15.653 15.653,0 0,1 3.534,5.946c0.431,8.846 4.8,12.96 8.458,15.29A17.39,17.39 0,0 0,19.983 9.7ZM22.53,5.03 L21.47,3.97 12,13.439 8.53,9.97 7.47,11.03 12,15.561Z"></path></svg>');
      }

      .msg-status .msg-profile-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg" viewBox="0 0 20 20" fill=")" << (light ? "black" : "white") << R"(" stroke="none"><path d="M13.653,9.893a5,5 0,1 0,-7.306 0A5.589,5.589 0,0 0,2 15.333V17H3.5V15.333A4.088,4.088 0,0 1,7.583 11.25h4.834A4.088,4.088 0,0 1,16.5 15.333V17H18V15.333A5.589,5.589 0,0 0,13.653 9.893ZM10,10a3.5,3.5 0,1 1,3.5 -3.5A3.5,3.5 0,0 1,10 10Z"></path></svg>');
      }

      .msg-status .msg-checkmark {
        background-image: url('data:image/svg+xml;utf-8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill=")" << (light ? "black" : "white") << R"(" stroke="none"><path d="M9.172,18.5l-6.188,-6.187l1.061,-1.061l5.127,5.127l10.783,-10.784l1.061,1.061l-11.844,11.844z"></path></svg>');
      }

      .msg-status .msg-expiration-timer-disabled {
        background-image: url('data:image/svg+xml;utf-8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 16 16" fill=")" << (light ? "black" : "white") << R"(" stroke="none"><path d="M11.47,13.89C10.4585,14.6149 9.2445,15.0033 8,15C6.4087,15 4.8826,14.3679 3.7574,13.2426C2.6321,12.1174 2,10.5913 2,9C2.0004,7.7576 2.3923,6.5469 3.12,5.54L3.83,6.25C3.2027,7.2109 2.9251,8.3584 3.0437,9.4999C3.1623,10.6413 3.6699,11.7072 4.4814,12.5186C5.2928,13.3301 6.3587,13.8377 7.5001,13.9563C8.6416,14.0749 9.7891,13.7973 10.75,13.17L11.47,13.89ZM14.71,14.29L14,15L2,3L2.71,2.29L4.53,4.12C5.3872,3.4951 6.3948,3.1086 7.45,3L7,1H9L8.55,3C9.7326,3.1074 10.8567,3.5633 11.78,4.31C11.8082,4.1801 11.8708,4.0602 11.9613,3.9628C12.0519,3.8655 12.1669,3.7943 12.2945,3.7569C12.422,3.7194 12.5573,3.7169 12.6861,3.7498C12.8149,3.7826 12.9325,3.8496 13.0265,3.9435C13.1204,4.0375 13.1874,4.1551 13.2202,4.2839C13.2531,4.4127 13.2506,4.548 13.2131,4.6755C13.1757,4.8031 13.1045,4.9181 13.0072,5.0087C12.9098,5.0992 12.7899,5.1618 12.66,5.19C13.4844,6.2077 13.9531,7.4672 13.9946,8.7763C14.0362,10.0854 13.6482,11.3721 12.89,12.44L14.71,14.29ZM13,9C12.999,8.0979 12.7539,7.2129 12.2907,6.4387C11.8275,5.6646 11.1636,5.0302 10.3691,4.6027C9.5747,4.1753 8.6795,3.9707 7.7783,4.0107C6.877,4.0507 6.0034,4.3338 5.25,4.83L7.49,7.08L7.75,5H8.25L8.66,8.24L12.17,11.75C12.7097,10.9342 12.9983,9.9781 13,9Z"></path></svg>');
      }

      .msg-status .msg-expiration-timer-set {
        background-image: url('data:image/svg+xml;utf-8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 16 16" fill=")" << (light ? "black" : "white") << R"(" stroke="none"><path d="M12.66,5.22C12.7899,5.1918 12.9098,5.1292 13.0072,5.0387C13.1045,4.9481 13.1757,4.8331 13.2132,4.7055C13.2506,4.578 13.2531,4.4427 13.2202,4.3139C13.1874,4.1851 13.1205,4.0675 13.0265,3.9735C12.9325,3.8795 12.8149,3.8126 12.6861,3.7798C12.5573,3.7469 12.422,3.7494 12.2945,3.7868C12.1669,3.8243 12.0519,3.8955 11.9613,3.9928C11.8708,4.0902 11.8082,4.2101 11.78,4.34C10.8603,3.5825 9.7358,3.1161 8.55,3L9,1H7L7.45,3C6.2642,3.1161 5.1397,3.5825 4.22,4.34C4.1918,4.2101 4.1292,4.0902 4.0387,3.9928C3.9481,3.8955 3.8331,3.8243 3.7055,3.7868C3.578,3.7494 3.4427,3.7469 3.3139,3.7798C3.1851,3.8126 3.0675,3.8795 2.9736,3.9735C2.8795,4.0675 2.8126,4.1851 2.7798,4.3139C2.7469,4.4427 2.7494,4.578 2.7868,4.7055C2.8244,4.8331 2.8955,4.9481 2.9928,5.0387C3.0902,5.1292 3.2101,5.1918 3.34,5.22C2.6259,6.1004 2.1759,7.1652 2.042,8.2908C1.9081,9.4165 2.0959,10.5571 2.5835,11.5805C3.0712,12.6038 3.8387,13.4681 4.7973,14.0732C5.756,14.6783 6.8664,14.9995 8,14.9995C9.1336,14.9995 10.244,14.6783 11.2027,14.0732C12.1613,13.4681 12.9289,12.6038 13.4165,11.5805C13.9041,10.5571 14.0919,9.4165 13.958,8.2908C13.8241,7.1652 13.3741,6.1004 12.66,5.22ZM8,14C7.0111,14 6.0444,13.7068 5.2221,13.1573C4.3999,12.6079 3.759,11.827 3.3806,10.9134C3.0022,9.9998 2.9032,8.9944 3.0961,8.0246C3.289,7.0546 3.7652,6.1637 4.4645,5.4645C5.1637,4.7652 6.0546,4.289 7.0245,4.0961C7.9945,3.9032 8.9998,4.0022 9.9134,4.3806C10.8271,4.759 11.6079,5.3999 12.1574,6.2221C12.7068,7.0444 13,8.0111 13,9C13,9.6566 12.8707,10.3068 12.6194,10.9134C12.3681,11.52 11.9998,12.0712 11.5355,12.5355C11.0712,12.9998 10.52,13.3681 9.9134,13.6194C9.3068,13.8707 8.6566,14 8,14ZM8.75,9C8.75,9.1989 8.671,9.3897 8.5303,9.5303C8.3897,9.671 8.1989,9.75 8,9.75C7.8011,9.75 7.6103,9.671 7.4697,9.5303C7.329,9.3897 7.25,9.1989 7.25,9L7.75,5H8.25L8.75,9Z"></path></svg>');
      }

      .msg-status .msg-video-call-incoming,
      .msg-status .msg-video-call-outgoing,
      .msg-status .msg-group-call,
      .msg-status .msg-call-incoming,
      .msg-status .msg-call-missed,
      .msg-status .msg-call-outgoing,
      .msg-status .msg-info-icon,
      .msg-status .msg-security-icon,
      .msg-status .msg-profile-icon,
      .msg-status .msg-checkmark,
      .msg-status .msg-expiration-timer-disabled,
      .msg-status .msg-expiration-timer-set {
        display: inline-block;
        height: 18px;
        aspect-ratio: 1 / 1;
        margin-right: 8px;
        top: 2px;
        position: relative;
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
        color: white;
        text-decoration: none;
      }

      .menu-item {
        display: flex;
        flex-direction: row;
        color: )" << (light ? "black" : "white") << R"(;
        align-items: center;
        font-family: Roboto, "Noto Sans", "Liberation Sans", OpenSans, sans-serif;
        padding: 5px;
      }

      .menu-item > div {
        margin-right: 5px;
      }

      .menu-icon {
        margin-right: 0px;
        width: 30px;
        aspect-ratio: 1 / 1;
        background-position: center;
        background-repeat: no-repeat;
        background-size: cover;
      }

      .nav-up {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill=")" << (light ? "black" : "white") << R"(" stroke=")" << (light ? "black" : "white") << R"("><path d="M9.5,17.5l1.1,-1.1l-4.9,-4.9l-1.1,-0.8H17V9.2H4.6l1.1,-0.8l4.9,-5L9.5,2.5L2,10L9.5,17.5z"></path></svg>');
      }

      .nav-one {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none" stroke=")" << (light ? "black" : "white") << R"("><path style="stroke-width: 3;" d="M 13.796428,2.9378689 6.7339026,10.000394 13.795641,17.062131"></path></svg>');
      }

      .nav-max {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none" stroke=")" << (light ? "black" : "white") << R"("><path style="stroke-width: 3;" d="M 10.746186,2.9378689 3.6836603,10.000394 10.745399,17.062131"></path><path style="stroke-width: 3;" d="M 16.846186,2.9378689 9.7836603,10.000394 16.845399,17.062131"></path></svg>');
      }

      .nav-fwd {
        transform: scaleX(-1);
      }

      .nav-disabled {
        filter: brightness(50%);
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
      }

      .left-column,
      .right-column {
        flex: 0 0 49%;
      }

      .left-column {
        padding-right: 1%;
        text-align: right;
      }

      .right-column {
        padding-left: 1%;
        text-align: left;
      }

      .columnview-header {
        flex: 0 0 100%;
        text-align: center;
        font-style: italic;
      }

      #thread-subtitle input[type=checkbox]:checked ~ label > .groupdetails {
        max-height: none;
        padding-top: 5px;
        padding-bottom: 5px;
        overflow: visible;
        transition: padding-top 0.2s ease, padding-bottom 0.2s ease, max-height 0.4s ease;
      }

      @media print {
        #menu {
          display: none;
        }

        .msg {
          break-inside: avoid;
          /* both fit-content and max-content seem fine here, so just including both as fall back */
          width: -webkit-fit-content;
          width: -moz-fit-content;
          width: fit-content;
          width: -webkit-max-content;
          width: -moz-max-content;
          width: max-content;

         /*leave it up to print settings */
         /*background-color: transparent;*/
        }

        .msg-incoming, .msg-outgoing {
          border: 1px solid black;
          display: block;
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

        .msg-status, .msg-date-change, .msg-group-update-v1 {
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

        .status-text > div, .checkmarks {
          -webkit-print-color-adjust: exact;
          color-adjust: exact;
          print-color-adjust: exact;
          filter: brightness(0);
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
        /* todo: print style for audio, video and attachment previews */
      } /* end @media print */

    </style>
  </head>
  <body>
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
          <div class="avatar header-avatar)" << (getRecipientInfoFromMap(recipient_info, thread_recipient_id).initial_is_emoji ? " avatar-emoji-initial" : "") << R"( msg-sender-)" << thread_recipient_id << R"(">
            )" << getRecipientInfoFromMap(recipient_info, thread_recipient_id).initial << R"(
          </div>)";
    }
  }
  else
  {
    file << R"(
          <input type="checkbox" id="zoomCheck-avatar">
          <label for="zoomCheck-avatar">
            <img class="avatar avatar-)" << thread_recipient_id
             << R"( header-avatar msg-sender-)" << thread_recipient_id << R"(" src="media/Avatar_)" << thread_recipient_id << R"(.bin" alt="thread avatar">
          </label>)";
  }
  file << R"(
          <div id="thread-title">
            )" << (isnotetoself ? "Note to self" : getRecipientInfoFromMap(recipient_info, thread_recipient_id).display_name) << R"(
          </div>
          <div id="thread-subtitle">
            )";
  if (isgroup)
  {
    file << groupmembers.size() << " member" << (groupmembers.size() != 1 ? "s" : "") << std::endl;
    file << "            <input type=\"checkbox\" id=\"showmembers\">" << std::endl;
    file << "            <label for=\"showmembers\">" << std::endl;
    file << "              <small>(details)</small>" << std::endl;
    file << "              <span class=\"groupdetails\">" << std::endl;
    file << "                <span class=\"columnview\">" << std::endl;

    // group description
    if (!groupinfo.description.empty())
    {
      file << "                  <span class=\"left-column\">Description:</span>" << std::endl;
      file << "                  <span class=\"right-column\">" << groupinfo.description << "</span>" << std::endl;
    }

    // group members
    file << "                  <span class=\"left-column\">Members:</span>" << std::endl;
    file << "                  <span class=\"right-column\">";
    for (uint gm = 0; gm < groupmembers.size(); ++gm)
      file << getRecipientInfoFromMap(recipient_info, groupmembers[gm]).display_name
           << (bepaald::contains(groupinfo.admin_ids, groupmembers[gm]) ? " <i>(admin)</i>" : "") << ((gm < groupmembers.size() - 1) ? ", " : "");
    file << "</span>" << std::endl;

    // pending members
    file << "                  <span class=\"left-column\">Pending members:</span>" << std::endl;
    file << "                  <span class=\"right-column\">";
    if (groupinfo.pending_members.size() == 0)
      file << "(none)";
    else
      for (uint pm = 0; pm < groupinfo.pending_members.size(); ++pm)
        file << getRecipientInfoFromMap(recipient_info, groupinfo.pending_members[pm]).display_name
             << ((pm < groupinfo.pending_members.size() - 1) ? ", " : "");
    file << "</span>" << std::endl;

    // 'requesting' members
    file << "                  <span class=\"left-column\">Requesting members:</span>" << std::endl;
    file << "                  <span class=\"right-column\">";
    if (groupinfo.requesting_members.size() == 0)
      file << "(none)";
    else
      for (uint rm = 0; rm < groupinfo.requesting_members.size(); ++rm)
        file << getRecipientInfoFromMap(recipient_info, groupinfo.requesting_members[rm]).display_name
             << ((rm < groupinfo.requesting_members.size() - 1) ? ", " : "");
    file << "</span>" << std::endl;

    // banned members
    file << "                  <span class=\"left-column\">Banned members:</span>" << std::endl;
    file << "                  <span class=\"right-column\">";
    if (groupinfo.banned_members.size() == 0)
      file << "(none)";
    else
      for (uint bm = 0; bm < groupinfo.banned_members.size(); ++bm)
        file << getRecipientInfoFromMap(recipient_info, groupinfo.banned_members[bm]).display_name
             << ((bm < groupinfo.banned_members.size() - 1) ? ", " : "");
    file << "</span>" << std::endl;

    file << "                  <span class=\"columnview-header\">Options</span>" << std::endl;

    // expiration timer
    file << "                  <span class=\"left-column\">Disappearing messages:</span>" << std::endl;
    std::string exptimer = "Off";
    if (groupinfo.expiration_timer)
    {
      if (groupinfo.expiration_timer < 60) // less than full minute
        exptimer = bepaald::toString(groupinfo.expiration_timer) + " seconds";
      else if (groupinfo.expiration_timer < 60 * 60) // less than full hour
        exptimer = bepaald::toString(groupinfo.expiration_timer / 60) + " minutes";
      else if (groupinfo.expiration_timer < 24 * 60 * 60) // less than full day
        exptimer = bepaald::toString(groupinfo.expiration_timer / (60 * 60)) + " hours";
      else if (groupinfo.expiration_timer < 7 * 24 * 60 * 60) // less than full week
        exptimer = bepaald::toString(groupinfo.expiration_timer / (24 * 60 * 60)) + " days";
      else // show groupinfo.expiration_timer in number of weeks
        exptimer = bepaald::toString(groupinfo.expiration_timer / (7 * 24 * 60 * 60)) + " weeks";
    }
    file << "                  <span class=\"right-column\">" << exptimer << "</span>" << std::endl;

    // link enabled?
    file << "                  <span class=\"left-column\">Group link:</span>" << std::endl;
    file << "                  <span class=\"right-column\">" << (groupinfo.link_invite_enabled ? "Enabled" : "Off") <<  "</span>" << std::endl;

    // access control
    file << "                  <span class=\"columnview-header\">Permissions</span>" << std::endl;
    file << "                  <span class=\"left-column\">Add members:</span>" << std::endl;
    file << "                  <span class=\"right-column\">" << groupinfo.access_control_members << "</span>" << std::endl;
    file << "                  <span class=\"left-column\">Edit group info:</span>" << std::endl;
    file << "                  <span class=\"right-column\">" << groupinfo.access_control_attributes << "</span>" << std::endl;
    file << "                  <span class=\"left-column\">Send messages:</span>" << std::endl;
    file << "                  <span class=\"right-column\">" << (groupinfo.isannouncementgroup ? "Only admins" : "All members") << "</span>" << std::endl;
    file << "                  <span class=\"left-column\">Approve members from invite link:</span>" << std::endl;
    file << "                  <span class=\"right-column\">" << groupinfo.access_control_addfromlinkinvite << "</span>" << std::endl;

    file << "                </span>" << std::endl;
    file << "              </span>" << std::endl;
    file << "            </label>" << std::endl;
  }
  else
    file << (getRecipientInfoFromMap(recipient_info, thread_recipient_id).display_name == getRecipientInfoFromMap(recipient_info, thread_recipient_id).phone ? "" :
             getRecipientInfoFromMap(recipient_info, thread_recipient_id).phone);
  file << R"(          </div>
        </div>
        <div class="conversation-box">

)";

  return true;

}

void SignalBackup::HTMLwriteAttachmentDiv(std::ofstream &htmloutput, SqliteDB::QueryResults const &attachment_results, int indent,
                                          std::string const &directory, std::string const &threaddir,
                                          bool is_image_preview, bool overwrite, bool append) const
{
  for (uint a = 0; a < attachment_results.rows(); ++a)
  {

    long long int rowid = attachment_results.getValueAs<long long int>(a, "_id");
    long long int uniqueid = attachment_results.getValueAs<long long int>(a, "unique_id");
    long long int pending_push = attachment_results.getValueAs<long long int>(a, "pending_push");

    if (pending_push != 0)
    {
      htmloutput << std::string(indent, ' ') << "<div class=\"attachment\">" << std::endl;
      htmloutput << std::string(indent, ' ') << "  <div class=\"pending-attachment\">" << std::endl;
      htmloutput << std::string(indent, ' ') << "    (attachment not downloaded)" << std::endl;
      htmloutput << std::string(indent, ' ') << "  </div>" << std::endl;
      htmloutput << std::string(indent, ' ') << "</div>" << std::endl;
      return;
    }

    // write the attachment data
    if (!HTMLwriteAttachment(directory, threaddir, rowid, uniqueid, overwrite, append))
      continue;

    std::string content_type = attachment_results.valueAsString(a, "ct");
    std::string original_filename;
    if (!attachment_results.isNull(a, "file_name") && !attachment_results(a, "file_name").empty())
    {
      original_filename = attachment_results(a, "file_name");
      HTMLescapeString(&original_filename);
    }

    htmloutput << std::string(indent, ' ') << "<div class=\"attachment"
               << ((!STRING_STARTS_WITH(content_type, "image/") && !STRING_STARTS_WITH(content_type, "video/") && !STRING_STARTS_WITH(content_type, "audio/")) ?
                   " attachment-unknown-type" : "")
               << "\">" << std::endl;

    if (STRING_STARTS_WITH(content_type, "image/"))
    {
      htmloutput << std::string(indent, ' ') << "  <div class=\"msg-" << (is_image_preview ? "linkpreview-" : "") << "img-container\">" << std::endl;
      htmloutput << std::string(indent, ' ') << "    <input type=\"checkbox\" id=\"zoomCheck-" << rowid << "-" << uniqueid << "\">" << std::endl;
      htmloutput << std::string(indent, ' ') << "    <label for=\"zoomCheck-" << rowid << "-" << uniqueid << "\">" << std::endl;
      htmloutput << std::string(indent, ' ') << "      <img src=\"media/Attachment_" << rowid
                 << "_" << uniqueid << ".bin\" alt=\"Image attachment\">" << std::endl;
      htmloutput << std::string(indent, ' ') << "    </label>" << std::endl;
      htmloutput << std::string(indent, ' ') << "  </div>" << std::endl;
    }
    else if (STRING_STARTS_WITH(content_type, "video/") ||
             STRING_STARTS_WITH(content_type, "audio/"))
    {
      htmloutput << std::string(indent, ' ') << "  <" << content_type.substr(0, 5) << " controls>" << std::endl;
      htmloutput << std::string(indent, ' ') << "    <source src=\"media/Attachment_" << rowid
                 << "_" << uniqueid << ".bin\" type=\"" << content_type << "\">" << std::endl;
      htmloutput << std::string(indent, ' ') << "    Media of type " << content_type << "<span class=\"msg-dl-link\"><a href=\"media/Attachment_" << rowid
                 << "_" << uniqueid << ".bin\" type=\"" << content_type << "\">&#129055;</a></span>" << std::endl;
      htmloutput << std::string(indent, ' ') << "  </" << content_type.substr(0, 5) << ">" << std::endl;
    }
    else if (content_type.empty())
    {
      if (original_filename.empty())
        htmloutput << std::string(indent, ' ') << "  Attachment of unknown type <span class=\"msg-dl-link\"><a href=\"media/Attachment_" << rowid
                   << "_" << uniqueid << ".bin\">&#129055;</a></span>" << std::endl;
      else
        htmloutput << std::string(indent, ' ') << "  Attachment '" << original_filename << "' <span class=\"msg-dl-link\"><a href=\"media/Attachment_" << rowid
          //<< "_" << uniqueid << ".bin\" download=\"" << original_filename << "\">&#129055;</a></span>" << std::endl; // does not work
                   << "_" << uniqueid << ".bin\">&#129055;</a></span>" << std::endl;
    }
    else // other
    {
      if (original_filename.empty())
        htmloutput << std::string(indent, ' ') << "  Attachment of type " << content_type << "<span class=\"msg-dl-link\"><a href=\"media/Attachment_" << rowid
                   << "_" << uniqueid << ".bin\" type=\"" << content_type << "\">&#129055;</a></span>" << std::endl;
      else
        htmloutput << std::string(indent, ' ') << "  Attachment '" << original_filename << "'<span class=\"msg-dl-link\"><a href=\"media/Attachment_" << rowid
          //<< "_" << uniqueid << ".bin\" type=\"" << content_type << "\" download=\"" << original_filename << "\">&#129055;</a></span>" << std::endl; // does not work
                   << "_" << uniqueid << ".bin\" type=\"" << content_type << "\">&#129055;</a></span>" << std::endl;
    }

    htmloutput << std::string(indent, ' ') << "</div>" << std::endl;

  }
}

void SignalBackup::HTMLwriteMessage(std::ofstream &htmloutput, HTMLMessageInfo const &msg_info,
                                    std::map<long long int, RecipientInfo> *recipient_info) const
{
  int extraindent = 0;
  // insert message

  htmloutput << "          <!-- Message: _id:" << msg_info.msg_id <<",type:" << msg_info.type << " -->" << std::endl;

  // for incoming group (normal) message: insert avatar with initial
  if (msg_info.isgroup && msg_info.incoming && !msg_info.is_deleted && !Types::isStatusMessage(msg_info.type))
  {
    htmloutput << "          <div class=\"incoming-group-msg\">" << std::endl;
    htmloutput << "            <div class=\"avatar avatar-" << msg_info.msg_recipient_id
               << " convo-avatar msg-sender-" << msg_info.msg_recipient_id << "\">";
    if (!getRecipientInfoFromMap(recipient_info, msg_info.msg_recipient_id).hasavatar)
    {
      htmloutput << std::endl;
      htmloutput << "              <span>" << getRecipientInfoFromMap(recipient_info, msg_info.msg_recipient_id).initial << "</span>" << std::endl;
      htmloutput << "            ";
    }
    htmloutput << "</div>" << std::endl;
    extraindent = 2;
  }

  // msg bubble
  htmloutput << std::string(extraindent, ' ') << "          <div class=\"msg ";
  if (msg_info.isgroupupdatev1)
    htmloutput << "msg-group-update-v1\">" << std::endl;
  else if (Types::isStatusMessage(msg_info.type) && !msg_info.isgroupupdatev1)
    htmloutput << "msg-status\">" << std::endl;
  else
    htmloutput << "msg-" << (msg_info.incoming ? "incoming" : "outgoing")
               << (!msg_info.incoming ? " msg-sender-" + bepaald::toString(msg_info.msg_recipient_id) : "")
               << (msg_info.nobackground ? " no-bg-bubble" : "")
               << (msg_info.is_deleted ? " deleted-msg" : "")
               << (msg_info.reaction_results->rows() ? " msg-with-reaction" : "")<< "\">" << std::endl;

  // for incoming group (normal) message: Senders name before message content
  if (msg_info.isgroup && msg_info.incoming && !msg_info.is_deleted && !Types::isStatusMessage(msg_info.type))
    htmloutput << std::string(extraindent, ' ') << "            <span class=\"msg-name msg-name-"
               << msg_info.msg_recipient_id << "\">" << getRecipientInfoFromMap(recipient_info, msg_info.msg_recipient_id).display_name << "</span>" << std::endl;

  // insert quote
  if (msg_info.hasquote)
  {
    htmloutput << std::string(extraindent, ' ') << "            <div class=\"msg-quote\">" << std::endl;

    // quote message
    htmloutput << std::string(extraindent, ' ') << "              <div class=\"msg-quote-message\">" << std::endl;
    htmloutput << std::string(extraindent, ' ') << "                <span class=\"msg-name\">"
               << getRecipientInfoFromMap(recipient_info, msg_info.messages->getValueAs<long long int>(msg_info.idx, "quote_author")).display_name
               << "</span>" << std::endl;
    htmloutput << std::string(extraindent, ' ') << "                <pre>" << msg_info.quote_body << "</pre>" << std::endl;
    htmloutput << std::string(extraindent, ' ') << "              </div>" << std::endl;

    // quote attachment
    if (msg_info.quote_attachment_results->rows())
    {
      htmloutput << std::string(extraindent, ' ') << "              <div class=\"msg-quote-attach\">" << std::endl;
      HTMLwriteAttachmentDiv(htmloutput, *msg_info.quote_attachment_results, 16 + extraindent,
                             msg_info.directory, msg_info.threaddir, false, msg_info.overwrite, msg_info.append);
      htmloutput << "                </div>" << std::endl;
    }

    htmloutput << std::string(extraindent, ' ') << "            </div>" << std::endl;
  }

  // insert attachment?
  HTMLwriteAttachmentDiv(htmloutput, *msg_info.attachment_results, 12 + extraindent,
                         msg_info.directory, msg_info.threaddir,
                         (!msg_info.link_preview_title.empty() || !msg_info.link_preview_description.empty()),
                         msg_info.overwrite, msg_info.append);

  // insert link_preview data?
  if (!msg_info.link_preview_title.empty() || !msg_info.link_preview_description.empty())
  {
    htmloutput << "            <div class=\"linkpreview\">" << std::endl;
    if (!msg_info.link_preview_title.empty())
    {
      htmloutput << "              <div class=\"linkpreview_title\">" << std::endl;
      htmloutput << "                " << msg_info.link_preview_title << std::endl;
      htmloutput << "              </div>" << std::endl;
    }
    if (!msg_info.link_preview_description.empty())
    {
      htmloutput << "              <div class=\"linkpreview_description\">" << std::endl;
      htmloutput << "                " << msg_info.link_preview_description << std::endl;
      htmloutput << "              </div>" << std::endl;
    }
    htmloutput << "            </div>" << std::endl;
  }

  //insert body
  if (!msg_info.body.empty())
  {
    htmloutput << std::string(extraindent, ' ') << "            <div"
               << (msg_info.only_emoji ? " class=\"msg-all-emoji\"" : "")
               << (Types::isStatusMessage(msg_info.type) && !msg_info.isgroupupdatev1 ? " class=\"status-text" +
                   (Types::isMissedCall(msg_info.type) || Types::isMissedVideoCall(msg_info.type) ? " status-text-red"s : "") + "\"" : "")
               << ">" << std::endl;
    htmloutput << std::string(extraindent, ' ') << "              <pre>";
    if (Types::isEndSession(msg_info.type) || Types::isIdentityDefault(msg_info.type)) // info-icon
      htmloutput << "<span class=\"msg-info-icon\"></span>";
    else if (Types::isIdentityUpdate(msg_info.type))
      htmloutput << "<span class=\"msg-security-icon\"></span>";
    else if (Types::isIdentityVerified(msg_info.type))
      htmloutput << "<span class=\"msg-checkmark\"></span>";
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
    //else if (Types::isProfileChange(msg_info.type))
    //  htmloutput << "<span class=\"msg-profile-icon\"></span>";
    //else if
    htmloutput << std::string(extraindent, ' ') << msg_info.body << "</pre>" << std::endl;
    htmloutput << std::string(extraindent, ' ') << "            </div>" << std::endl;
  }
  else if (msg_info.is_deleted)
  {
    htmloutput << "            <div>" << std::endl;
    if (msg_info.incoming)
      htmloutput << "              <pre>This message was deleted.</pre>" << std::endl;
    else
      htmloutput << "              <pre>You deleted this message.</pre>" << std::endl;
    htmloutput << "            </div>" << std::endl;
  }

  // insert msg-footer (date & checkmarks)
  htmloutput << std::string(extraindent, ' ') << "            <div class=\"footer" << (Types::isStatusMessage(msg_info.type) ? "-status" : "") << "\">" << std::endl;
  htmloutput << std::string(extraindent, ' ') << "              <span class=\"msg-data\">" << msg_info.readable_date << "</span>" << std::endl;
  if (!msg_info.incoming && !Types::isCallType(msg_info.type) && !msg_info.is_deleted) // && received, read?
  {
    htmloutput << std::string(extraindent, ' ') << "              <div class=\"checkmarks checkmarks-";
    if (msg_info.messages->getValueAs<long long int>(msg_info.idx, "read_receipt_count") > 0)
      htmloutput << "read";
    else if (msg_info.messages->getValueAs<long long int>(msg_info.idx, "delivery_receipt_count") > 0)
      htmloutput << "received";
    else // if something? type != failed? -> check for failed before outputting 'checkmarks-'
      htmloutput << "sent";
    htmloutput << "\"></div>" << std::endl;
  }
  htmloutput << std::string(extraindent, ' ') << "            </div>" << std::endl;

  // insert reaction
  if (msg_info.reaction_results->rows())
  {
    htmloutput << std::string(extraindent, ' ') << "            <div class=\"msg-reactions\">" << std::endl;


    std::set<std::string> skip;
    for (uint r = 0; r < msg_info.reaction_results->rows(); ++r)
    {

      std::string emojireaction = msg_info.reaction_results->valueAsString(r, "emoji");

      if (bepaald::contains(skip, emojireaction))
        continue;

      skip.insert(emojireaction);

      // count occurences of this emoji, and set info
      int count = 0;
      std::string reaction_info;
      for (uint r2 = r; r2 < msg_info.reaction_results->rows(); ++r2)
        if (emojireaction == msg_info.reaction_results->valueAsString(r2, "emoji"))
        {
          ++count;
          reaction_info += (reaction_info.empty() ? "" : "<hr>") + "From "s + getRecipientInfoFromMap(recipient_info, msg_info.reaction_results->getValueAs<long long int>(r2, "author_id")).display_name +
            "<br>Sent: " + msg_info.reaction_results->valueAsString(r2, "date_sent") +
            "<br>Received: " + msg_info.reaction_results->valueAsString(r2, "date_received");
        }

      htmloutput << std::string(extraindent, ' ') << "              <span class=\"msg-reaction\"><span class=\"msg-emoji\">"
                 << emojireaction << "</span>" << (count > 1 ? "<span class=\"reaction-count\">" + bepaald::toString(count) + "</span>": "")
                 << "<span class=\"msg-reaction-info\">" << reaction_info << "</span></span>" << std::endl;
    }
    htmloutput << std::string(extraindent, ' ') << "            </div>" << std::endl;
  }
  // end message
  htmloutput << std::string(extraindent, ' ') << "          </div>" << std::endl;
  if (msg_info.isgroup && msg_info.incoming && !msg_info.is_deleted && !Types::isStatusMessage(msg_info.type))
    htmloutput << "          </div>" << std::endl;
  htmloutput << std::endl;
}
