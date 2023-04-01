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
                                  std::set<long long int> const &recipient_ids,
                                  std::map<long long int, RecipientInfo> *recipient_info,
                                  std::map<long long int, std::string> *written_avatars,
                                  bool overwrite, bool append) const
{

  std::vector<long long int> groupmembers;
  if (isgroup)
  {
    SqliteDB::QueryResults results;
    d_database.exec("SELECT group_id from recipient WHERE _id IS ?", thread_recipient_id, &results);
    if (results.rows() == 1)
      getGroupMembersOld(&groupmembers, results.valueAsString(0, "group_id"));
  }

  std::string thread_avatar = bepaald::contains(written_avatars, thread_recipient_id) ?
    (*written_avatars)[thread_recipient_id] :
    ((*written_avatars)[thread_recipient_id] = HTMLwriteAvatar(thread_recipient_id, directory, threaddir, overwrite, append));

  file << R"(<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <title>)" << getRecipientInfoFromMap(recipient_info, thread_recipient_id).display_name << R"(</title>
    <style>

      body {
        background-color: #000000;
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
        color: white;
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
        background-color: #1B1C1F;
        color: white;
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
        background: #303133;
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
        background: #)" << (isgroup ? s_html_colormap.at("ULTRAMARINE") : getRecipientInfoFromMap(recipient_info, thread_recipient_id).color) << R"(;
      }

      .deleted-msg {
        background: rgba(0, 0, 0, 0);
        border: 1px solid white;
      }

      .avatar {
        font-weight: 500;
        border-radius: 50% 50%;
        aspect-ratio: 1 / 1;
        text-align: center;
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
      }
)";
    }
  }
  file << R"(
      .convo-avatar {
        font-size: x-large;
        margin: auto 15px 15px 0px;
        padding: 2px;
        height: 33px;
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
        /*border: 1px solid white;*/
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
        font-family: "Noto Color Emoji", sans-serif;
      }

      .msg-all-emoji {
        font-size: xxx-large;
      }

      .no-bg-bubble {
        background: rgba(0, 0, 0, 0);
      }

      .mention-in {
        background-color: #5E5E5E;
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
        font-size: xx-large;
        text-decoration: none;
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

      .msg-img-container input[type=checkbox] {
        display: none;
      }

      .msg-img-container img {
        transition: transform 0.25s ease;
        border-radius: 0.6em;
        cursor: zoom-in;
      }

      .msg-img-container input[type=checkbox]:checked ~ label > img {
        transform: scale(2.5);
        border-radius: 0;
        cursor: zoom-out;
        z-index: 1;
        position: relative;
      }

      .msg-outgoing .pending-attachment {
        padding: 5px;
        background-color: rgba(0, 0, 0, 0.244);
        text-align: center;
      }
      .msg-incoming .pending-attachment {
        padding: 5px;
        background-color: #5E5E5E;
        text-align: center;
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
        background-color: #303133;
        border-radius: 11px;
        border: 1px solid #1B1C1F;
        line-height: 150%;
        position: relative;
      }

      .msg-reaction-self {
        background-color: #aaaaaa;
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

      .msg-incoming .msg-quote {
        background-color: rgba(255, 255, 255, .16);
        }

      .msg-outgoing .msg-quote {
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

      .msg-call-incoming,
      .msg-call-outgoing,
      .msg-call-missed,
      .msg-video-call-incoming,
      .msg-video-call-outgoing,
      .msg-video-call-missed,
      .msg-status,
      .msg-group-update-v1,
      .msg-group-update-v2,
      .msg-group-call {
        background: none;
        align-self: center;
      }

      .msg-call-incoming div,
      .msg-call-outgoing div,
      .msg-call-missed div,
      .msg-video-call-incoming div,
      .msg-video-call-outgoing div,
      .msg-video-call-missed div,
      .msg-status div,
      .msg-group-update-v1 div,
      .msg-group-update-v2 div,
      .msg-group-call div {
        background: none;
        align-self: center;
        text-align: center;
      }
      .msg-call-incoming .msg-data,
      .msg-call-outgoing .msg-data,
      .msg-call-missed .msg-data,
      .msg-video-call-incoming .msg-data,
      .msg-video-call-outgoing .msg-data,
      .msg-video-call-missed .msg-data,
      .msg-status .msg-data,
      .msg-group-update-v1 .msg-data,
      .msg-group-update-v2 .msg-data,
      .msg-group-call .msg-data {
        display: block;
        text-align: center;
      }

      .msg-group-update-v1, .msg-group-update-v2 {
        max-width: 80%;
      }

      .msg-icon {
        background-repeat: no-repeat;
        background-size: cover;
        width: 24px;
        height: 24px;
        opacity: 50%;
        margin: 0 auto;
        filter: invert(100%);
      }

      .msg-call-missed .msg-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="feather feather-phone-missed"><line x1="23" y1="1" x2="17" y2="7"></line><line x1="17" y1="1" x2="23" y2="7"></line><path d="M22 16.92v3a2 2 0 0 1-2.18 2 19.79 19.79 0 0 1-8.63-3.07 19.5 19.5 0 0 1-6-6 19.79 19.79 0 0 1-3.07-8.67A2 2 0 0 1 4.11 2h3a2 2 0 0 1 2 1.72 12.84 12.84 0 0 0 .7 2.81 2 2 0 0 1-.45 2.11L8.09 9.91a16 16 0 0 0 6 6l1.27-1.27a2 2 0 0 1 2.11-.45 12.84 12.84 0 0 0 2.81.7A2 2 0 0 1 22 16.92z"></path></svg>');
      }

      .msg-call-incoming .msg-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="feather feather-phone-incoming"><polyline points="16 2 16 8 22 8"></polyline><line x1="23" y1="1" x2="16" y2="8"></line><path d="M22 16.92v3a2 2 0 0 1-2.18 2 19.79 19.79 0 0 1-8.63-3.07 19.5 19.5 0 0 1-6-6 19.79 19.79 0 0 1-3.07-8.67A2 2 0 0 1 4.11 2h3a2 2 0 0 1 2 1.72 12.84 12.84 0 0 0 .7 2.81 2 2 0 0 1-.45 2.11L8.09 9.91a16 16 0 0 0 6 6l1.27-1.27a2 2 0 0 1 2.11-.45 12.84 12.84 0 0 0 2.81.7A2 2 0 0 1 22 16.92z"></path></svg>');
      }

      .msg-call-outgoing .msg-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="feather feather-phone-outgoing"><polyline points="23 7 23 1 17 1"></polyline><line x1="16" y1="8" x2="23" y2="1"></line><path d="M22 16.92v3a2 2 0 0 1-2.18 2 19.79 19.79 0 0 1-8.63-3.07 19.5 19.5 0 0 1-6-6 19.79 19.79 0 0 1-3.07-8.67A2 2 0 0 1 4.11 2h3a2 2 0 0 1 2 1.72 12.84 12.84 0 0 0 .7 2.81 2 2 0 0 1-.45 2.11L8.09 9.91a16 16 0 0 0 6 6l1.27-1.27a2 2 0 0 1 2.11-.45 12.84 12.84 0 0 0 2.81.7A2 2 0 0 1 22 16.92z"></path></svg>');
        transform: scale(-1, 1);
      }

      .msg-video-call-missed .msg-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 14.117647 14.117647"><path d="m 13.288805,3.2188235 c -0.2276,-0.097 -0.4791,-0.1231 -0.7217,-0.0749 -0.2426,0.0482 -0.465,0.1684 -0.6383,0.3449 l -1.87,1.86 v -1.54 c -0.0026,-0.4633 -0.1877997,-0.9069 -0.5154997,-1.2345 -0.3276,-0.3277 -0.7712,-0.5129 -1.2345,-0.5155 h -6.5 c -0.4633,0.0026 -0.90689998,0.1878 -1.23449998,0.5155 -0.3277,0.3276 -0.5129,0.7712 -0.5155,1.2345 v 6.5000005 c 0.0026,0.4633 0.1878,0.9069 0.5155,1.2345 0.3276,0.3277 0.77119998,0.5129 1.23449998,0.5155 h 6.5 c 0.4633,-0.0026 0.9069,-0.1878 1.2345,-0.5155 0.3277,-0.3276 0.5128997,-0.7712 0.5154997,-1.2345 V 8.7688235 l 1.87,1.8600005 c 0.1146,0.1172 0.2515,0.2103 0.4026,0.2739 0.1512,0.0635 0.3135,0.0962 0.4774,0.0961 0.1652,6e-4 0.3288,-0.0334 0.48,-0.1 0.2289,-0.0923 0.4248,-0.2513 0.5621,-0.4564 0.1373,-0.2051 0.2098,-0.4468005 0.2079,-0.6936005 v -5.38 c 0.0019,-0.2468 -0.0706,-0.4885 -0.2079,-0.6936 -0.1373,-0.2051 -0.3332,-0.3641 -0.5621,-0.4564 z M 9.0588053,10.308824 c -0.0026,0.1981 -0.0824,0.3874 -0.2225,0.5275 -0.1401,0.1401 -0.3294,0.2199 -0.5275,0.2225 h -6.5 c -0.1981,-0.0026 -0.3874,-0.0824 -0.5275,-0.2225 -0.1401,-0.1401 -0.2199,-0.3294 -0.2225,-0.5275 V 3.8088235 c 0.0026,-0.1981 0.0824,-0.3874 0.2225,-0.5275 0.1401,-0.1401 0.3294,-0.2199 0.5275,-0.2225 h 6.5 c 0.1981,0.0026 0.3874,0.0824 0.5275,0.2225 0.1401,0.1401 0.2199,0.3294 0.2225,0.5275 z M 13.058805,9.7488235 c 2e-4,0.0488 -0.0139,0.0966 -0.0406,0.1374 -0.0267,0.0409 -0.0647,0.0731 -0.1094,0.0926 -0.0465,0.0198 -0.0977,0.0256 -0.1474,0.0167 -0.0498,-0.0089 -0.0958,-0.0321 -0.1326,-0.0667 l -2.57,-2.58 v -0.58 l 2.57,-2.58 c 0.0418,-0.0267 0.0904,-0.0409 0.14,-0.0409 0.0496,0 0.0982,0.0142 0.14,0.0409 0.0447,0.0195 0.0827,0.0517 0.1094,0.0926 0.0267,0.0408 0.0408,0.0886 0.0406,0.1374 z m -7.2899997,-2.69 2.14,2.15 -0.7,0.7 -2.15,-2.14 -2.15,2.14 -0.7,-0.7 2.14,-2.15 -2.14,-2.15 0.7,-0.7 2.15,2.14 2.15,-2.14 0.7,0.7 z"></path></svg>');
      }

      .msg-video-call-incoming .msg-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 14.117647 14.117647"><path d="m 13.288805,3.2188234 c -0.2276,-0.097 -0.4791,-0.1231 -0.7217,-0.0749 -0.2426,0.0482 -0.465,0.1684 -0.6383,0.3449 l -1.87,1.86 v -1.54 c -0.0026,-0.4633 -0.1877996,-0.9069 -0.5154999,-1.2345 -0.3276,-0.3277 -0.7712,-0.5129 -1.2345,-0.5155 h -6.5 c -0.4633,0.0026 -0.90689997,0.1878 -1.23449997,0.5155 -0.32769998,0.3276 -0.51289998,0.7712 -0.51549998,1.2345 v 6.5000006 c 0.0026,0.4633 0.1878,0.9069 0.51549998,1.2345 0.3276,0.3277 0.77119997,0.5129 1.23449997,0.5155 h 6.5 c 0.4633,-0.0026 0.9069,-0.1878 1.2345,-0.5155 0.3277003,-0.3276 0.5128999,-0.7712 0.5154999,-1.2345 V 8.7688234 l 1.87,1.8600006 c 0.1146,0.1172 0.2515,0.2103 0.4026,0.2739 0.1512,0.0635 0.3135,0.0962 0.4774,0.0961 0.1652,6e-4 0.3288,-0.0334 0.48,-0.1 0.2289,-0.0923 0.4248,-0.2513 0.5621,-0.4564 0.1373,-0.2051 0.2098,-0.4468003 0.2079,-0.6936003 V 4.3688234 c 0.0019,-0.2468 -0.0706,-0.4885 -0.2079,-0.6936 -0.1373,-0.2051 -0.3332,-0.3641 -0.5621,-0.4564 z M 9.0588051,10.308824 c -0.0026,0.1981 -0.0824,0.3874 -0.2225,0.5275 -0.1401,0.1401 -0.3294,0.2199 -0.5275,0.2225 h -6.5 c -0.1981,-0.0026 -0.3874,-0.0824 -0.5275,-0.2225 -0.1401,-0.1401 -0.2199,-0.3294 -0.2225,-0.5275 V 3.8088234 c 0.0026,-0.1981 0.0824,-0.3874 0.2225,-0.5275 0.1401,-0.1401 0.3294,-0.2199 0.5275,-0.2225 h 6.5 c 0.1981,0.0026 0.3874,0.0824 0.5275,0.2225 0.1401,0.1401 0.2199,0.3294 0.2225,0.5275 z M 13.058805,9.7488237 c 2e-4,0.0488 -0.0139,0.0966 -0.0406,0.1374 -0.0267,0.0409 -0.0647,0.0731 -0.1094,0.0926 -0.0465,0.0198 -0.0977,0.0256 -0.1474,0.0167 -0.0498,-0.0089 -0.0958,-0.0321 -0.1326,-0.0667 l -2.57,-2.5800003 v -0.58 l 2.57,-2.58 c 0.0418,-0.0267 0.0904,-0.0409 0.14,-0.0409 0.0496,0 0.0982,0.0142 0.14,0.0409 0.0447,0.0195 0.0827,0.0517 0.1094,0.0926 0.0267,0.0408 0.0408,0.0886 0.0406,0.1374 z M 3.7688051,9.0588234 h 3.29 v 1.0000006 h -5 V 5.0588234 h 1 v 3.29 l 4.15,-4.14 0.7,0.7 z"></path></svg>');
      }

      .msg-video-call-outgoing .msg-icon {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 14.117647 14.117647"><path d="m 13.288805,3.2188235 c -0.2276,-0.097 -0.4791,-0.1231 -0.7217,-0.0749 -0.2426,0.0482 -0.465,0.1684 -0.6383,0.3449 l -1.87,1.86 v -1.54 c -0.0026,-0.4633 -0.1877997,-0.9069 -0.5154997,-1.2345 -0.3276,-0.3277 -0.7712,-0.5129 -1.2345,-0.5155 h -6.5 c -0.4633,0.0026 -0.90689998,0.1878 -1.23449998,0.5155 -0.3277,0.3276 -0.5129,0.7712 -0.5155,1.2345 v 6.5000005 c 0.0026,0.4633 0.1878,0.9069 0.5155,1.2345 0.3276,0.3277 0.77119998,0.5129 1.23449998,0.5155 h 6.5 c 0.4633,-0.0026 0.9069,-0.1878 1.2345,-0.5155 0.3277,-0.3276 0.5128997,-0.7712 0.5154997,-1.2345 V 8.7688235 l 1.87,1.8600005 c 0.1146,0.1172 0.2515,0.2103 0.4026,0.2739 0.1512,0.0635 0.3135,0.0962 0.4774,0.0961 0.1652,6e-4 0.3288,-0.0334 0.48,-0.1 0.2289,-0.0923 0.4248,-0.2513 0.5621,-0.4564 0.1373,-0.2051 0.2098,-0.4468005 0.2079,-0.6936005 v -5.38 c 0.0019,-0.2468 -0.0706,-0.4885 -0.2079,-0.6936 -0.1373,-0.2051 -0.3332,-0.3641 -0.5621,-0.4564 z M 9.0588053,10.308824 c -0.0026,0.1981 -0.0824,0.3874 -0.2225,0.5275 -0.1401,0.1401 -0.3294,0.2199 -0.5275,0.2225 h -6.5 c -0.1981,-0.0026 -0.3874,-0.0824 -0.5275,-0.2225 -0.1401,-0.1401 -0.2199,-0.3294 -0.2225,-0.5275 V 3.8088235 c 0.0026,-0.1981 0.0824,-0.3874 0.2225,-0.5275 0.1401,-0.1401 0.3294,-0.2199 0.5275,-0.2225 h 6.5 c 0.1981,0.0026 0.3874,0.0824 0.5275,0.2225 0.1401,0.1401 0.2199,0.3294 0.2225,0.5275 z M 13.058805,9.7488235 c 2e-4,0.0488 -0.0139,0.0966 -0.0406,0.1374 -0.0267,0.0409 -0.0647,0.0731 -0.1094,0.0926 -0.0465,0.0198 -0.0977,0.0256 -0.1474,0.0167 -0.0498,-0.0089 -0.0958,-0.0321 -0.1326,-0.0667 l -2.57,-2.58 v -0.58 l 2.57,-2.58 c 0.0418,-0.0267 0.0904,-0.0409 0.14,-0.0409 0.0496,0 0.0982,0.0142 0.14,0.0409 0.0447,0.0195 0.0827,0.0517 0.1094,0.0926 0.0267,0.0408 0.0408,0.0886 0.0406,0.1374 z m -4.9999997,-5.69 v 5 h -1 v -3.29 l -4.15,4.14 -0.7,-0.7 4.14,-4.15 h -3.29 v -1 z"></path></svg>');
      }

      #menu {
        display: flex;
        flex-direction: column;
        position: fixed;
        top: 30px;
        left: 30px;
      }

      #menu a:link,
      #menu a:visited,
      #menu a:hover,
      #menu a:active {
        color: white;
        text-decoration: none;
      }

      .menu-button {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="white" stroke="white"><path d="M17,9.25l-6.25,0l0,-6.25l-1.5,0l0,6.25l-6.25,0l0,1.5l6.25,0l0,6.25l1.5,0l0,-6.25l6.25,0l0,-1.5z"></path></svg>');
      }

      .menu-item {
        display: flex;
        flex-direction: row;
        color: white;
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
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="white" stroke="white"><path d="M9.5,17.5l1.1,-1.1l-4.9,-4.9l-1.1,-0.8H17V9.2H4.6l1.1,-0.8l4.9,-5L9.5,2.5L2,10L9.5,17.5z"></path></svg>');
      }

      .nav-one {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none" stroke="white"><path style="stroke-width: 3;" d="M 13.796428,2.9378689 6.7339026,10.000394 13.795641,17.062131"></path></svg>');
      }

      .nav-max {
        background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none" stroke="white"><path style="stroke-width: 3;" d="M 10.746186,2.9378689 3.6836603,10.000394 10.745399,17.062131"></path><path style="stroke-width: 3;" d="M 16.846186,2.9378689 9.7836603,10.000394 16.845399,17.062131"></path></svg>');
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

    </style>
  </head>
  <body>
    <div class="controls-wrapper">
      <div class="conversation-wrapper">
        <div id="message-header">)";
  if (thread_avatar.empty())
  {
    if (isgroup)
    {
      file << R"(
          <div class="avatar header-avatar msg-sender-)" << thread_recipient_id << R"(">
            <div class="group-avatar-icon"></div>
          </div>)";
    }
    else
    {
      file << R"(
          <div class="avatar header-avatar msg-sender-)" << thread_recipient_id << R"(">
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
            )" << getRecipientInfoFromMap(recipient_info, thread_recipient_id).display_name << R"(
          </div>
          <div id="thread-subtitle">
            )" << (isgroup ? bepaald::toString(groupmembers.size()) + " members" : getRecipientInfoFromMap(recipient_info, thread_recipient_id).phone) << R"(
          </div>
        </div>
        <div class="conversation-box">

)";

  return true;

}

void SignalBackup::HTMLwriteAttachmentDiv(std::ofstream &htmloutput, SqliteDB::QueryResults const &attachment_results, int indent,
                                          std::string const &directory, std::string const &threaddir, bool overwrite, bool append) const
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

    htmloutput << std::string(indent, ' ') << "<div class=\"attachment\">" << std::endl;

    std::string content_type = attachment_results.valueAsString(a, "ct");
    if (STRING_STARTS_WITH(content_type, "image/"))
    {
      htmloutput << std::string(indent, ' ') << "  <div class=\"msg-img-container\">" << std::endl;
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
                 << "_" << uniqueid << ".bin\" type=\"" << content_type << "\">&#x2913;</a></span>" << std::endl;
      htmloutput << std::string(indent, ' ') << "  </" << content_type.substr(0, 5) << ">" << std::endl;
    }
    else // other
    {
      htmloutput << std::string(indent, ' ') << "  Attachment of type " << content_type << "<span class=\"msg-dl-link\"><a href=\"media/Attachment_" << rowid
                 << "_" << uniqueid << ".bin\" type=\"" << content_type << "\" download>&#x2913;</a></span>" << std::endl;
    }

    htmloutput << std::string(indent, ' ') << "</div>" << std::endl;

  }
}

void SignalBackup::HTMLwriteMessage(std::ofstream &htmloutput, HTMLMessageInfo const &msg_info,
                                    std::map<long long int, RecipientInfo> *recipient_info) const
{
  int extraindent = 0;
  // insert message

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
  htmloutput << std::string(extraindent, ' ') << "          <div id=\"msg-" << msg_info.msg_id
             << "\" class=\"type-" << msg_info.type << " msg ";
  if (msg_info.isgroupupdatev1)
    htmloutput << "msg-group-update-v1\">" << std::endl;
  else if (Types::isCallType(msg_info.type)) // some call type
  {
    htmloutput << "msg-" << (Types::isIncomingCall(msg_info.type) ? "call-incoming" : (Types::isOutgoingCall(msg_info.type) ? "call-outgoing" : (Types::isMissedCall(msg_info.type) ? "call-missed" : (Types::isIncomingVideoCall(msg_info.type) ? "video-call-incoming" : (Types::isOutgoingVideoCall(msg_info.type) ? "video-call-outgoing" : (Types::isMissedVideoCall(msg_info.type) ? "video-call-missed" : (Types::isGroupCall(msg_info.type) ? "group-call" : "")))))));
    htmloutput << "\">" << std::endl;
    htmloutput << "            <div class=\"msg-icon\"></div>" << std::endl;
  }
  else if (Types::isStatusMessage(msg_info.type) && !Types::isCallType(msg_info.type) && !msg_info.isgroupupdatev1)
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
                             msg_info.directory, msg_info.threaddir, msg_info.overwrite, msg_info.append);
      htmloutput << "                </div>" << std::endl;
    }

    htmloutput << std::string(extraindent, ' ') << "            </div>" << std::endl;
  }

  // insert attachment?
  HTMLwriteAttachmentDiv(htmloutput, *msg_info.attachment_results, 12 + extraindent,
                         msg_info.directory, msg_info.threaddir, msg_info.overwrite, msg_info.append);

  //insert body
  if (!msg_info.body.empty())
  {
    htmloutput << std::string(extraindent, ' ') << "            <div"
               << (msg_info.only_emoji ? " class=\"msg-all-emoji\"" : "") << ">" << std::endl;
    if (!Types::isCallType(msg_info.type))
      htmloutput << std::string(extraindent, ' ') << "              <pre>" << msg_info.body << "</pre>" << std::endl;
    else
      htmloutput << std::string(extraindent, ' ') << "            " << msg_info.body << std::endl;
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
  if (!msg_info.incoming && !Types::isCallType(msg_info.type)) // && received, read?
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
    for (uint r = 0; r < msg_info.reaction_results->rows(); ++r)
      htmloutput << std::string(extraindent, ' ') << "              <span class=\"msg-reaction\"><span class=\"msg-emoji\">"
                 << msg_info.reaction_results->valueAsString(r, "emoji") << "</span>"
                 << "<span class=\"msg-reaction-info\">From "
                 << getRecipientInfoFromMap(recipient_info, msg_info.reaction_results->getValueAs<long long int>(r, "author_id")).display_name
                 << "<br>Sent: " << msg_info.reaction_results->valueAsString(r, "date_sent")
                 << "<br>Received: " << msg_info.reaction_results->valueAsString(r, "date_received") << "</span></span>" << std::endl;
    htmloutput << std::string(extraindent, ' ') << "            </div>" << std::endl;
  }
  // end message
  htmloutput << std::string(extraindent, ' ') << "          </div>" << std::endl;
  if (msg_info.isgroup && msg_info.incoming && !msg_info.is_deleted && !Types::isStatusMessage(msg_info.type))
    htmloutput << "          </div>" << std::endl;
  htmloutput << std::endl;
}
