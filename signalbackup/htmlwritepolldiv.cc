/*
  Copyright (C) 2025  Selwin van Dijk

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

/*
  To-do

  v htmlescape & emojify poll title + poll_options
  v check colors
  - add vote details
  v check with removed votes (vote_state)
  - show own votes? (only when not closed?) (no, also after close...)
  x show winning option? (only when closed?)
*/

bool SignalBackup::HTMLwritePollDiv(std::ofstream &htmloutput, int indent,
                                    SqliteDB::QueryResults const &poll,
                                    SqliteDB::QueryResults const &poll_options,
                                    SqliteDB::QueryResults const &poll_votes) const
{
  if (d_verbose) [[unlikely]]
    Logger::message("Got poll: '", poll("question"), "'");

  // poll: _id, author_id, message_id, question, allow_multiple_votes, end_message_id
  // poll_option: _id, poll_id, option_text, option_order
  // poll_vote: _id, poll_id, poll_option_id, voter_id, vote_count, date_received, vote_state

  /*
    enum VoteState
    {
    NONE = 0,
    PENDING_REMOVE = 1,
    PENDING_ADD = 2,
    REMOVED = 3,
    ADDED = 4,
    };
  */

  std::string poll_title(poll("question"));
  HTMLprepMsgBody(&poll_title);

  htmloutput
    << std::string(indent, ' ') << "<div class=\"poll\">\n"
    << std::string(indent, ' ') << "  <div class=\"poll-title\">" << poll_title << "</div>\n"
    << std::string(indent, ' ') << "  <div class=\"poll-subtitle\">Poll &middot; ";
  if (poll.valueAsInt(0, "end_message_id", 0) != 0)
    htmloutput << "Final results";
  else if (poll.valueAsInt(0, "allow_multiple_votes", 0) == 1)
    htmloutput << "Select one or more";
  else
    htmloutput << "Select one";
  htmloutput << "</div>\n"
    << std::string(indent, ' ') << "  <div class=\"poll-options\">\n";

  // gather data on number of votes per option, totals and max
  int totalvotes = 0;
  int maxvotes = 0;
  std::map<long long int, int> votes_per_option; // maps option_id -> num-votes
  for (unsigned int j = 0; j < poll_votes.rows(); ++j)
  {
    long long int poll_option_id = poll_votes.valueAsInt(j, "poll_option_id", -1);
    if (poll_option_id == -1) [[unlikely]]
      continue;

    if (votes_per_option.contains(poll_option_id))
      ++votes_per_option[poll_option_id];
    else
      votes_per_option.emplace(poll_option_id, 1);

    if (votes_per_option[poll_option_id] > maxvotes)
      maxvotes = votes_per_option[poll_option_id];

    ++totalvotes;
  }

  for (unsigned int i = 0; i < poll_options.rows(); ++i)
  {
    std::string option(poll_options(i, "option_text"));
    HTMLprepMsgBody(&option);
    long long int poll_option_id = poll_options.valueAsInt(i, "_id", -1);
    if (poll_option_id == -1) [[unlikely]]
      continue;

    htmloutput
      << std::string(indent, ' ') << "    <div class=\"poll-option\">\n"
      << std::string(indent, ' ') << "      <div class=\"poll-option-title-votes\">\n"
      << std::string(indent, ' ') << "        <div class=\"poll-option-title\">" << option << "</div>"
      << "<div class=\"poll-option-votes\">" << votes_per_option[poll_option_id] << "</div>\n"
      << std::string(indent, ' ') << "      </div>\n"
      << std::string(indent, ' ') << "      <div class=\"poll-option-meter-bar\">";
    if (votes_per_option[poll_option_id]) // if votes were cast, fill meter bar
      htmloutput << "<div class=\"poll-option-meter-filled\" style=\"width:" << ((100 * votes_per_option[poll_option_id]) / maxvotes) << "%\"></div>";
    htmloutput << "</div>\n";
    if (votes_per_option[poll_option_id])
    {
      htmloutput
        << std::string(indent, ' ') << "      <div class=\"poll-option-details\">";
      for (unsigned int j = 0; j < poll_votes.rows(); ++j)
        if (poll_votes.valueAsInt(j, "poll_option_id", -1) == poll_option_id)
          htmloutput << "(vote-details for option " << poll_option_id << " (TO-DO))";
      htmloutput << "</div>\n";
    }
    htmloutput
      << std::string(indent, ' ') << "    </div>\n";
  }
  htmloutput
    << std::string(indent, ' ') << "  </div>\n"
    << std::string(indent, ' ') << "</div>\n";

  return true;
}
