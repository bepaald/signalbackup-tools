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

void SignalBackup::setRecipientInfo(std::set<long long int> const &recipients,
                                    std::map<long long int, RecipientInfo> *recipientinfo) const
{
  // get info from all recipients:
  for (long long int rid : recipients)
  {
    if (bepaald::contains(recipientinfo, rid)) // already present
      continue;

    // get info
    SqliteDB::QueryResults results;
    d_database.exec("SELECT COALESCE(NULLIF(recipient.system_display_name, ''), " +
                    (d_database.tableContainsColumn("recipient", "profile_joined_name") ? "NULLIF(recipient.profile_joined_name, ''),"s : ""s) +
                    "NULLIF(recipient.signal_profile_name, ''), NULLIF(groups.title, ''), NULLIF(recipient.phone, ''), NULLIF(recipient.uuid, ''), "
                    " recipient._id) AS 'display_name', recipient.phone, recipient.username, recipient.uuid, recipient.group_id, recipient.color, recipient.wallpaper, "
                    " recipient.chat_colors " //wallpaper_file, custom_chat_colors_id
                    "FROM recipient LEFT JOIN groups ON recipient.group_id = groups.group_id WHERE recipient._id = ?", rid, &results);

    std::string display_name = results.valueAsString(0, "display_name");
    if (display_name.empty())
      display_name = "?";

    std::string initial;
    bool initial_is_emoji = false;
    if (bepaald::contains(s_emoji_first_bytes, display_name[0]))
    {
      for (char const *const emoji_string : s_emoji_unicode_list)
      {
        unsigned int emoji_size = std::strlen(emoji_string);
        if ((display_name.size() >= emoji_size) &&
            std::strncmp(display_name.data(), emoji_string, emoji_size) == 0)
        {
          initial = emoji_string;
          initial_is_emoji = true;
          break;
        }
      }
    }

    if (initial.empty())
    {
      int charsize = bytesToUtf8CharSize(display_name, 0);
      if (charsize == 1)
        initial = std::toupper(display_name[0]);
      else
        initial = display_name.substr(0, charsize);
    }
    if (display_name[0] != '?' && (std::ispunct(display_name[0]) || std::isdigit(display_name[0])))
      initial = "#";

    std::string color = results.isNull(0, "group_id") ? "555555" : s_html_colormap.at("group_color");
    if (bepaald::contains(s_html_colormap, results.valueAsString(0, "color")))
      color = s_html_colormap.at(results.valueAsString(0, "color"));

    // custom color?
    if (!results.isNull(0, "chat_colors"))
    {
      //std::cout << "CHAT COLOR" << std::endl;
      auto [lightcolor, darkcolor] = getCustomColor(results.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(0, "chat_colors"));
      if (!lightcolor.empty())
        color = lightcolor;
    }

    // custom wallpaper?
    std::string wall_light;
    std::string wall_dark;
    if (!results.isNull(0, "wallpaper"))
    {
      auto [lightcolor, darkcolor] = getCustomColor(results.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(0, "wallpaper"));
      if (!lightcolor.empty())
      {
        wall_light = lightcolor;
        wall_dark = (darkcolor.empty() ? lightcolor : darkcolor);
      }
    }

    bool hasavatar = (std::find_if(d_avatars.begin(), d_avatars.end(),
                                   [rid](auto const &p) { return p.first == bepaald::toString(rid); }) != d_avatars.end());

    (*recipientinfo)[rid] = {display_name,
                             initial,
                             initial_is_emoji,
                             results.valueAsString(0, "uuid"),
                             results.valueAsString(0, "phone"),
                             results.valueAsString(0, "username"),
                             color,
                             wall_light,
                             wall_dark,
                             hasavatar};
  }
}

/*
  app/src/main/java/org/thoughtcrime/securesms/conversation/colors/AvatarColor.java
  color: 'pink' -> A150
         'C300' -> A150
         'A150' -> 0xFFF5D7D7

  app/src/main/java/org/thoughtcrime/securesms/color/MaterialColor.java
private static final Map<String, MaterialColor> COLOR_MATCHES = new HashMap<String, MaterialColor>() {{
    put("red",         CRIMSON);
    put("deep_orange", CRIMSON);
    put("orange",      VERMILLION);
    put("amber",       VERMILLION);
    put("brown",       BURLAP);
    put("yellow",      BURLAP);
    put("pink",        PLUM);
    put("purple",      VIOLET);
    put("deep_purple", VIOLET);
    put("indigo",      INDIGO);
    put("blue",        BLUE);
    put("light_blue",  BLUE);
    put("cyan",        TEAL);
    put("teal",        TEAL);
    put("green",       FOREST);
    put("light_green", WINTERGREEN);
    put("lime",        WINTERGREEN);
    put("blue_grey",   TAUPE);
    put("grey",        STEEL);
    put("ultramarine", ULTRAMARINE);
    put("group_color", GROUP);

  app/src/main/java/org/thoughtcrime/securesms/conversation/colors/ChatColorsPalette.kt
    val CRIMSON = ChatColors.forColor(ChatColors.Id.BuiltIn, 0xFFCF163E.toInt())
    val VERMILION = ChatColors.forColor(ChatColors.Id.BuiltIn, 0xFFC73F0A.toInt())
    val BURLAP = ChatColors.forColor(ChatColors.Id.BuiltIn, 0xFF6F6A58.toInt())
    val FOREST = ChatColors.forColor(ChatColors.Id.BuiltIn, 0xFF3B7845.toInt())
    val WINTERGREEN = ChatColors.forColor(ChatColors.Id.BuiltIn, 0xFF1D8663.toInt())
    val TEAL = ChatColors.forColor(ChatColors.Id.BuiltIn, 0xFF077D92.toInt())
    val BLUE = ChatColors.forColor(ChatColors.Id.BuiltIn, 0xFF336BA3.toInt())
    val INDIGO = ChatColors.forColor(ChatColors.Id.BuiltIn, 0xFF6058CA.toInt())
    val VIOLET = ChatColors.forColor(ChatColors.Id.BuiltIn, 0xFF9932CB.toInt())
    val PLUM = ChatColors.forColor(ChatColors.Id.BuiltIn, 0xFFAA377A.toInt())
    val TAUPE = ChatColors.forColor(ChatColors.Id.BuiltIn, 0xFF8F616A.toInt())
    val STEEL = ChatColors.forColor(ChatColors.Id.BuiltIn, 0xFF71717F.toInt())

  // from app/src/main/res/values/material3_colors_dark.xml:
  screen background:     <color name="signal_dark_colorBackground">#1B1C1F</color> / <color name="signal_dark_colorSurface">#1B1C1F</color>
  chat bubble (other party):     <color name="signal_dark_colorSurfaceVariant">#303133</color>

  // from app/src/main/java/org/thoughtcrime/securesms/color/MaterialColor.java
  quote background: outgoing ?
                               int alpha = isDarkTheme(context) ? (int) (0.2 * 255) : (int) (0.4 * 255)
                             :
                               isDarkTheme(context) ? R.color.transparent_black_40 : R.color.transparent_white_60

  quote footer: outgoing ?
                           int alpha = isDarkTheme(context) ? (int) (0.4 * 255) : (int) (0.6 * 255)
                         :
                           isDarkTheme(context) ? R.color.transparent_black_60 : R.color.transparent_white_80

  // from app/src/main/res/values/colors.xml

    <color name="transparent_black_40">#66000000</color>
    <color name="transparent_black_50">#80000000</color>
    <color name="transparent_black_60">#99000000</color>

    <color name="transparent_white_60">#99ffffff</color>
    <color name="transparent_white_70">#b2ffffff</color>
    <color name="transparent_white_80">#ccffffff</color>

 */
