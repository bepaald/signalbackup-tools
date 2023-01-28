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
                                                                          {"VERMILION", "C73F0A"},
                                                                          {"brown", "6F6A58"},
                                                                          {"yellow", "6F6A58"},
                                                                          {"A190", "6F6A58"}, // OR taupe
                                                                          {"BURLAP", "6F6A58"},
                                                                          {"pink", "AA377A"},
                                                                          {"A150", "AA377A"},
                                                                          {"PLUM", "AA377A"},
                                                                          {"purple", "9932CB"},
                                                                          {"deep_purple", "9932CB"},
                                                                          {"A140", "9932CB"},
                                                                          {"VIOLET", "9932CB"},
                                                                          {"indigo", "6058CA"},
                                                                          {"A100", "6058CA"}, // OR ultramarine
                                                                          {"INDIGO", "6058CA"},
                                                                          {"blue", "336BA3"},
                                                                          {"light_blue", "336BA3"},
                                                                          {"A110", "336BA3"},
                                                                          {"BLUE", "336BA3"},
                                                                          {"cyan", "077D92"},
                                                                          {"teal", "077D92"},
                                                                          {"A120", "077D92"},
                                                                          {"TEAL", "077D92"},
                                                                          {"green", "3B7845"},
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
                                                                          {"ULTRAMARINE", "315FF4"}/*,
                                                                          {"group_color", "315FF4"},
                                                                          {"GROUP", "315FF4"}*/};
