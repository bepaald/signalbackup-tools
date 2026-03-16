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

#include "signalbackup.ih"

std::string SignalBackup::getHostname(std::string_view host) const
{
  // This does a simple parse to extract the hostname from a string
  // that is a valid URL. It currently does not support URL using
  // raw IPv6 addresses as host, but signal itself does not recognize
  // these as links either (no preview, not even linkified)
  // example: http://[1fff:0:a88:85a3::ac1f]:8001/index.html


  //std::cout << "     URL: '" << host << "'" << std::endl;

  // get protocol...
  auto protocol_endpos = host.find("://");
  if (protocol_endpos == std::string_view::npos) [[unlikely]]
    protocol_endpos = 0;
  else
    protocol_endpos += 3;
  host = host.substr(protocol_endpos);

  // get hostname (+port +userdata)
  auto hostname_endpos = host.find("/");
  host = host.substr(0, hostname_endpos);

  // chop off userinfo if present
  std::string_view::size_type userinfo_pos;
  if ((userinfo_pos = host.find("@")) != std::string_view::npos) [[unlikely]]
    host = host.substr(userinfo_pos + 1);

  // chop off port...
  auto port_pos = host.find(":");
  host = host.substr(0, port_pos);

  //std::cout << "HOSTNAME: '" << (host.find('.') != std::string_view::npos ? host : std::string_view()) << "'" << std::endl;
  return host.find('.') != std::string_view::npos ? std::string(host) : std::string();
}
