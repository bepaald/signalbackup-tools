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

#ifndef COMMON_REGEX_H_
#define COMMON_REGEX_H_

#if defined USE_BOOST_REGEX && __has_include("boost/regex.hpp")
#pragma message("USING BOOST REGEX!")

#define BOOST_REGEX_NO_LIB
#include <boost/regex.hpp>

#define REGEX boost::regex
#define REGEX_SMATCH_RESULTS boost::smatch
#define REGEX_MATCH boost::regex_match
#define REGEX_SEARCH boost::regex_search
#define REGEX_FLAGS boost::regex::icase | boost::regex::ECMAScript | boost::regex::no_mod_m

#else

#include <regex>

#define REGEX std::regex
#define REGEX_SMATCH_RESULTS std::smatch
#define REGEX_MATCH std::regex_match
#define REGEX_SEARCH std::regex_search
#define REGEX_FLAGS std::regex_constants::icase | std::regex_constants::ECMAScript
#endif

#endif
