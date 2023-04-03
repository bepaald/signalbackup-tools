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

#include "signalbackup.ih"

void SignalBackup::HTMLwriteIndex(std::vector<long long int> const &threads, std::string const &directory,
                                  std::map<long long int, RecipientInfo> *recipient_info, bool overwrite, bool append) const
{

  std::cout << "Writing index.html..." << std::endl;

  if (bepaald::fileOrDirExists(directory + "/index.html"))
  {
    if (!overwrite && ! append)
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
                << ": '" << directory << "/index.html' exists. Use --overwrite to overwrite." << std::endl;
      return;
    }
  }
  std::ofstream outputfile(directory + "/index.html", std::ios_base::binary);
  if (!outputfile.is_open())
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
              << ": Failed to open '" << directory << "/index.html' for writing." << std::endl;
    return;
  }

  std::string threadlist;
  for (uint i = 0; i < threads.size(); ++i)
  {
    threadlist += bepaald::toString(threads[i]);
    if (i < threads.size() - 1)
      threadlist += ",";
  }

  SqliteDB::QueryResults results;
  if (!d_database.exec("SELECT "
                       "thread._id, "
                       "thread." + d_thread_recipient_id + ", "
                       "thread.snippet, "
                       "thread.snippet_type, "
                       "json_extract(thread.snippet_extras, '$.individualRecipientId') AS 'group_sender_id', "
                       + (d_database.tableContainsColumn("thread", "pinned") ? "pinned," : "") +
                       + (d_database.tableContainsColumn("thread", "archived") ? "archived," : "") +
                       "recipient.group_id "
                       "FROM thread "
                       "LEFT JOIN recipient ON recipient._id IS thread." + d_thread_recipient_id + " "
                       "WHERE thread._id IN (" + threadlist +") ORDER BY "
                       + (d_database.tableContainsColumn("thread", "pinned") ? "(pinned != 0) DESC, " : "") +
                       + (d_database.tableContainsColumn("thread", "archived") ? "archived ASC, " : "") +
                       "date DESC", &results)) // order by pinned DESC archived ASC date DESC??
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
              << ": Failed to query database for thread snippets." << std::endl;
    return;
  }
  //results.prettyPrint();

  outputfile
    << "<!DOCTYPE html>" << std::endl
    << "<html lang=\"en\">" << std::endl
    << "  <head>" << std::endl
    << "    <meta charset=\"utf-8\">" << std::endl
    << "    <title>Signal</title>" << std::endl
    << "    <style>" << std::endl
    << "      body {" << std::endl
    << "        background-color: #000000;" << std::endl
    << "      }" << std::endl
    << "" << std::endl
    << "      .conversation-list-header {" << std::endl
    << "        text-align: center;" << std::endl
    << "        font-size: xx-large;" << std::endl
    << "        color: white;" << std::endl
    << "        padding: 10px;" << std::endl
    << "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;" << std::endl
    << "      }" << std::endl
    << "" << std::endl
    << "      .header {" << std::endl
    << "        margin-top: 5px;" << std::endl
    << "        margin-bottom: 5px;" << std::endl
    << "        margin-left: 10px;" << std::endl
    << "        font-weight: bold;" << std::endl
    << "      }" << std::endl
    << "" << std::endl
    << "      .conversation-list {" << std::endl
    << "        display: flex;" << std::endl
    << "        flex-direction: column;" << std::endl
    << "        width: fit-content;" << std::endl
    << "        margin-top: 10px;" << std::endl
    << "        margin-bottom: 100px;" << std::endl
    << "        margin-right: auto;" << std::endl
    << "        margin-left: auto;" << std::endl
    << "        padding: 30px;" << std::endl
    << "        background-color: #1B1C1F;" << std::endl
    << "        color: white;" << std::endl
    << "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;" << std::endl
    << "        border-radius: 10px;" << std::endl
    << "      }" << std::endl
    << "" << std::endl
    << "      .conversation-list-item {" << std::endl
    << "        display: flex;" << std::endl
    << "        flex-direction: row;" << std::endl
    << "        padding: 10px;" << std::endl
    << "        margin: auto;" << std::endl
    << "        justify-content: center;" << std::endl
    << "        align-items: center;" << std::endl
    << "        align-content: center;" << std::endl
    << "      }" << std::endl
    << "" << std::endl
    << "      .avatar {" << std::endl
    << "        position: relative;" << std::endl
    << "        display: flex;" << std::endl
    << "        border-radius: 50%;" << std::endl
    << "        width: 60px;" << std::endl
    << "        height: 60px;" << std::endl
    << "        line-height: 60px;" << std::endl
    << "        text-align: center;" << std::endl
    << "        justify-content: center;" << std::endl
    << "        font-size: 38px;" << std::endl
    << "      }" << std::endl
    << "" << std::endl
    << "      .group-avatar-icon {" << std::endl
    << "        background: #315FF4;" << std::endl
    << "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:svg=\"http://www.w3.org/2000/svg\" width=\"40\" height=\"40\" viewBox=\"0 0 40 40\" fill=\"white\"><path d=\"M29,16.75a6.508,6.508 0,0 1,6.5 6.5L35.5,24L37,24v-0.75a8,8 0,0 0,-6.7 -7.885,6.5 6.5,0 1,0 -8.6,0 7.941,7.941 0,0 0,-2.711 0.971A6.5,6.5 0,1 0,9.7 25.365,8 8,0 0,0 3,33.25L3,34L4.5,34v-0.75a6.508,6.508 0,0 1,6.5 -6.5h6a6.508,6.508 0,0 1,6.5 6.5L23.5,34L25,34v-0.75a8,8 0,0 0,-6.7 -7.885,6.468 6.468,0 0,0 1.508,-7.771A6.453,6.453 0,0 1,23 16.75ZM14,25.5a5,5 0,1 1,5 -5A5,5 0,0 1,14 25.5ZM21,10.5a5,5 0,1 1,5 5A5,5 0,0 1,21 10.5Z\"></path></svg>');" << std::endl
    << "        background-position: center;" << std::endl
    << "        background-repeat: no-repeat;" << std::endl
    << "        background-size: 80%;" << std::endl
    << "      }" << std::endl
    << "" << std::endl;

  for (uint i = 0; i < results.rows(); ++i)
  {
    if (!results.valueHasType<long long int>(i, d_thread_recipient_id))
      continue;
    long long int rec_id = results.getValueAs<long long int>(i, d_thread_recipient_id);

    if (getRecipientInfoFromMap(recipient_info, rec_id).hasavatar)
    {
      outputfile
        << "      .avatar-" << rec_id << " {" << std::endl
        << "        background-image: url(\"" << sanitizeFilename(getRecipientInfoFromMap(recipient_info, rec_id).display_name) << " (_id" << results.getValueAs<long long int>(i, "_id") << ")/media/Avatar_" << rec_id << ".bin\");" << std::endl
        << "        background-position: center;" << std::endl
        << "        background-repeat: no-repeat;" << std::endl
        << "        background-size: cover;" << std::endl
        << "      }" << std::endl
        << "" << std::endl;
    }
    else if (results.isNull(i, "group_id")) // no avatar, no group
    {
      outputfile
        << "      .avatar-" << rec_id << " {" << std::endl
        << "        background: #" << getRecipientInfoFromMap(recipient_info, rec_id).color << ";" << std::endl
        << "        padding-bottom: 2px;" << std::endl
        << "      }" << std::endl
        << "" << std::endl;
    }
  }

  outputfile
    << "      .name-and-snippet {" << std::endl
    << "        position: relative;" << std::endl
    << "        display: flex;" << std::endl
    << "        flex-direction: column;" << std::endl
    << "        margin-left: 30px;" << std::endl
    << "        justify-content: center;" << std::endl
    << "        align-content: center;" << std::endl
    << "        width: 350px;" << std::endl
    << "      }" << std::endl
    << "" << std::endl
    << "      .name {" << std::endl
    << "        font-weight: bold;" << std::endl
    << "      }" << std::endl
    << "" << std::endl
    << "      .snippet {" << std::endl
    << "        display: -webkit-box;" << std::endl
    << "        -webkit-line-clamp: 2;" << std::endl
    << "/*        line-clamp: 2; This is still in working draft, though the vendor extension version is well supported */" << std::endl
    << "        -webkit-box-orient: vertical;" << std::endl
    << "/*        box-orient: vertical; */" << std::endl
    << "        overflow: hidden;" << std::endl
    << "        text-overflow: ellipsis;" << std::endl
    << "      }" << std::endl
    << "" << std::endl
    << "      .main-link::before {" << std::endl
    << "        content: \" \";" << std::endl
    << "        position: absolute;" << std::endl
    << "        top: 0;" << std::endl
    << "        left: 0;" << std::endl
    << "        width: 100%;" << std::endl
    << "        height: 100%;" << std::endl
    << "      }" << std::endl
    << "    </style>" << std::endl
    << "  </head>" << std::endl
    << "" << std::endl
    << "    <div class=\"conversation-list-header\">" << std::endl
    << "      Signal conversation list" << std::endl
    << "    </div>" << std::endl
    << "" << std::endl
    << "    <div class=\"conversation-list\">" << std::endl
    << "" << std::endl;

  // for item in threads
  bool pinnedheader = false;
  bool archivedheader = false;
  bool chatsheader = false;
  for (uint i = 0; i < results.rows(); ++i)
  {
    bool archived = false;
    if (d_database.tableContainsColumn("thread", "archived"))
      archived = (results.getValueAs<long long int>(i, "archived") != 0);
    if (archived && !archivedheader)
    {
      outputfile << "      <div class=\"header\">Archived conversations</div>" << std::endl;
      archivedheader = true;
    }

    bool pinned = false;
    if (d_database.tableContainsColumn("thread", "pinned"))
      pinned = (results.getValueAs<long long int>(i, "pinned") != 0);
    if (pinned && !pinnedheader)
    {
      outputfile << "      <div class=\"header\">Pinned</div>" << std::endl;
      pinnedheader = true;
    }

    if (pinnedheader && !pinned && !chatsheader && !archived) // this message is not pinned, but pinnedheader was previously shown
    {
      outputfile << "      <div class=\"header\">Chats</div>" << std::endl;
      chatsheader = true;
    }

    if (!results.valueHasType<long long int>(i, d_thread_recipient_id))
      continue;
    long long int rec_id = results.getValueAs<long long int>(i, d_thread_recipient_id);

    if (!results.valueHasType<long long int>(i, "_id"))
      continue;
    long long int _id = results.getValueAs<long long int>(i, "_id");

    if (!results.valueHasType<long long int>(i, "snippet_type"))
      continue;
    long long int snippet_type = results.getValueAs<long long int>(i, "snippet_type");

    bool isgroup = !results.isNull(i, "group_id");

    long long int groupsender = -1;
    if (results.valueHasType<std::string>(i, "group_sender_id"))
      groupsender = bepaald::toNumber<long long int>(results.valueAsString(i, "group_sender_id"));

    std::string snippet = results.valueAsString(i, "snippet");
    HTMLescapeString(&snippet);

    if (Types::isStatusMessage(snippet_type))
      snippet = "(status message)";

    std::string convo_url_path = sanitizeFilename(getRecipientInfoFromMap(recipient_info, rec_id).display_name) + " (_id" + bepaald::toString(_id) + ")";
    HTMLescapeUrl(&convo_url_path);
    std::string convo_url_location = sanitizeFilename(getRecipientInfoFromMap(recipient_info, rec_id).display_name) + ".html";
    HTMLescapeUrl(&convo_url_location);

    outputfile
      << "      <div class=\"conversation-list-item\">" << std::endl
      << "        <div class=\"avatar " << ((getRecipientInfoFromMap(recipient_info, rec_id).hasavatar || !isgroup) ? "avatar-" + bepaald::toString(rec_id) : "group-avatar-icon") << "\">" << std::endl
      << "          <a href=\"" << convo_url_path << "/" << convo_url_location << "\" class=\"main-link\"></a>" << std::endl
      << ((!getRecipientInfoFromMap(recipient_info, rec_id).hasavatar && !isgroup) ? "          <span>" + getRecipientInfoFromMap(recipient_info, rec_id).initial + "</span>\n" : "")
      << "        </div>" << std::endl
      << "        <div class=\"name-and-snippet\">" << std::endl
      << "          <a href=\"" << convo_url_path << "/" << convo_url_location << "\" class=\"main-link\"></a>" << std::endl
      << "          <span class=\"name\">" << getRecipientInfoFromMap(recipient_info, rec_id).display_name << "</span>" << std::endl
      << "          <span class=\"snippet\">" << ((isgroup && groupsender > 0) ? getRecipientInfoFromMap(recipient_info, groupsender).display_name + ": " : "") << snippet << "</span>" << std::endl
      << "        </div>" << std::endl
      << "      </div>" << std::endl
      << "" << std::endl;
  }

  outputfile
    << "    </div>" << std::endl
    << "  </body>" << std::endl
    << "</html>" << std::endl;
}
