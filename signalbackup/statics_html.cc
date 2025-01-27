/*
  Copyright (C) 2023-2025  Selwin van Dijk

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
  app/src/main/java/org/thoughtcrime/securesms/conversation/colors/AvatarColor.java

  // this map is actually the other way around, so can;t really be used for this purpose,
  // but im doing it anyway
    NAME_MAP.put("crimson", A170);
    NAME_MAP.put("vermillion", A170);
    NAME_MAP.put("burlap", A190);
    NAME_MAP.put("forest", A130);
    NAME_MAP.put("wintergreen", A130);
    NAME_MAP.put("teal", A120);
    NAME_MAP.put("blue", A110);
    NAME_MAP.put("indigo", A100);
    NAME_MAP.put("violet", A140);
    NAME_MAP.put("plum", A150);
    NAME_MAP.put("taupe", A190);
    NAME_MAP.put("steel", A210);
    NAME_MAP.put("ultramarine", A100);
    NAME_MAP.put("unknown", A210);
    NAME_MAP.put("red", A170);
    NAME_MAP.put("orange", A170);
    NAME_MAP.put("deep_orange", A170);
    NAME_MAP.put("brown", A190);
    NAME_MAP.put("green", A130);
    NAME_MAP.put("light_green", A130);
    NAME_MAP.put("purple", A140);
    NAME_MAP.put("deep_purple", A140);
    NAME_MAP.put("pink", A150);
    NAME_MAP.put("blue_grey", A190);
    NAME_MAP.put("grey", A210);

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
    val ULTRAMARINE = ChatColors.forColor(ChatColors.Id.BuiltIn,0xFF315FF4.toInt())
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
*/
std::map<std::string, std::string> const SignalBackup::s_html_colormap = {{"red", "CF163E"},
                                                                          {"deep_orange", "CF163E"},
                                                                          {"CRIMSON", "CF163E"},

                                                                          {"orange", "C73F0A"},
                                                                          {"amber", "C73F0A"},
                                                                          {"A170", "C73F0A"},
                                                                          {"C010", "C73F0A"},
                                                                          {"C020", "C73F0A"},
                                                                          {"C030", "C73F0A"},
                                                                          {"VERMILION", "C73F0A"},

                                                                          {"brown", "6F6A58"},
                                                                          {"yellow", "6F6A58"},
                                                                          {"C000", "6F6A58"},
                                                                          {"C060", "6F6A58"},
                                                                          {"C070", "6F6A58"},
                                                                          {"A190", "6F6A58"}, // OR taupe
                                                                          {"BURLAP", "6F6A58"},

                                                                          {"pink", "AA377A"},
                                                                          {"C300", "AA377A"},
                                                                          {"C310", "AA377A"},
                                                                          {"C320", "AA377A"},
                                                                          {"A150", "AA377A"},
                                                                          {"PLUM", "AA377A"},

                                                                          {"purple", "9932CB"},
                                                                          {"deep_purple", "9932CB"},
                                                                          {"C270", "9932CB"},
                                                                          {"C280", "9932CB"},
                                                                          {"C290", "9932CB"},
                                                                          {"A140", "9932CB"},
                                                                          {"VIOLET", "9932CB"},

                                                                          {"indigo", "6058CA"},
                                                                          {"C230", "6058CA"},
                                                                          {"C240", "6058CA"},
                                                                          {"C250", "6058CA"},
                                                                          {"C260", "6058CA"},
                                                                          {"A100", "6058CA"}, // OR ultramarine
                                                                          {"INDIGO", "6058CA"},

                                                                          {"blue", "336BA3"},
                                                                          {"light_blue", "336BA3"},
                                                                          {"C200", "336BA3"},
                                                                          {"C210", "336BA3"},
                                                                          {"C220", "336BA3"},
                                                                          {"A110", "336BA3"},
                                                                          {"BLUE", "336BA3"},

                                                                          {"cyan", "077D92"},
                                                                          {"teal", "077D92"},
                                                                          {"C170", "077D92"},
                                                                          {"C180", "077D92"},
                                                                          {"C190", "077D92"},
                                                                          {"A120", "077D92"},
                                                                          {"TEAL", "077D92"},

                                                                          {"green", "3B7845"},
                                                                          {"C080", "3B7845"},
                                                                          {"C090", "3B7845"},
                                                                          {"C100", "3B7845"},
                                                                          {"C110", "3B7845"},
                                                                          {"C120", "3B7845"},
                                                                          {"C130", "3B7845"},
                                                                          {"C140", "3B7845"},
                                                                          {"C150", "3B7845"},
                                                                          {"C160", "3B7845"},
                                                                          {"A130", "3B7845"}, // OR wintergreen
                                                                          {"FOREST", "3B7845"},

                                                                          {"light_green", "1D8663"},
                                                                          {"lime", "1D8663"},
                                                                          {"WINTERGREEN", "1D8663"},

                                                                          {"blue_grey", "8F616A"},
                                                                          {"TAUPE", "8F616A"},

                                                                          {"grey", "71717F"},
                                                                          {"A210", "71717F"},
                                                                          {"STEEL", "71717F"},

                                                                          {"ultramarine", "315FF4"},
                                                                          {"ULTRAMARINE", "315FF4"},
                                                                          {"group_color", "315FF4"}//,
                                                                          /*
                                                                          {"GROUP", "315FF4"},
                                                                          */

                                                                          /*
                                                                          {"A180", "FEF5D0"},
                                                                          {"C040", "FEF5D0"},
                                                                          {"C050", "FEF5D0"},

                                                                          {"A160", "F6D8EC"},
                                                                          {"C330", "F6D8EC"},
                                                                          {"C340", "F6D8EC"},
                                                                          {"C350", "F6D8EC"},

                                                                          {"A200", "D2D2DC"}
                                                                          */
                                                                          };
/*
  app/src/main/java/org/thoughtcrime/securesms/conversation/colors/AvatarColor.java
  A100("A100", 0xFFE3E3FE),
  A110("A110", 0xFFDDE7FC),
  A120("A120", 0xFFD8E8F0),
  A130("A130", 0xFFCDE4CD),
  A140("A140", 0xFFEAE0F8),
  A150("A150", 0xFFF5E3FE),
  A160("A160", 0xFFF6D8EC),
  A170("A170", 0xFFF5D7D7),
  A180("A180", 0xFFFEF5D0),
  A190("A190", 0xFFEAE6D5),
  A200("A200", 0xFFD2D2DC),
  A210("A210", 0xFFD7D7D9),
  UNKNOWN("UNKNOWN", 0x00000000),
  ON_SURFACE_VARIANT("ON_SURFACE_VARIANT", 0x00000000);
*/
std::array<std::pair<std::string, std::string>, 12> const SignalBackup::s_html_random_colors{std::pair<std::string, std::string>{"A100", "E3E3FE"},
                                                                                             std::pair<std::string, std::string>{"A110", "DDE7FC"},
                                                                                             std::pair<std::string, std::string>{"A120", "D8E8F0"},
                                                                                             std::pair<std::string, std::string>{"A130", "CDE4CD"},
                                                                                             std::pair<std::string, std::string>{"A140", "EAE0F8"},
                                                                                             std::pair<std::string, std::string>{"A150", "F5E3FE"},
                                                                                             std::pair<std::string, std::string>{"A160", "F6D8EC"},
                                                                                             std::pair<std::string, std::string>{"A170", "F5D7D7"},
                                                                                             std::pair<std::string, std::string>{"A180", "FEF5D0"},
                                                                                             std::pair<std::string, std::string>{"A190", "EAE6D5"},
                                                                                             std::pair<std::string, std::string>{"A200", "D2D2DC"},
                                                                                             std::pair<std::string, std::string>{"A210", "D7D7D9"}};
