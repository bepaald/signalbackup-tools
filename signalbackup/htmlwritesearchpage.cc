/*
  Copyright (C) 2023-2024  Selwin van Dijk

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

void SignalBackup::HTMLwriteSearchpage(std::string const &dir, bool light, bool themeswitching) const
{

  Logger::message("Writing searchpage.html...");

  std::ofstream outputfile(dir + "/" + "searchpage.html", std::ios_base::binary);
  if (!outputfile.is_open())
  {
    Logger::error("Failed to open '", dir, "/searchpage.html' for writing");
    return;
  }

  outputfile <<
    R"(<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <title>Signal conversation search</title>
    <style>

.advancedsearch {
  padding: 5px;
}

#summary-box {
  display: flex;
  justify-content: center;
  padding: 10px 0px 10px 0px;
  align-items: center;
}

#searchfirst,
#searchlast,
#searchprev,
#searchnext,
#summary {
  display: inline-block;
  vertical-align: top;
}

#summary {
  text-align: center;
  min-width: 300px;
  padding: 0px 30px 0px 30px;
}

#searchfirst.disabled,
#searchlast.disabled,
#searchprev.disabled,
#searchnext.disabled {
  color: #AAAAAA;
  visibility: hidden;
}

#searchfirst.enabled,
#searchlast.enabled,
#searchprev.enabled,
#searchnext.enabled {
  color: inherit;
}

#searchfirst:hover,
#searchlast:hover,
#searchprev:hover,
#searchnext:hover {
  cursor: default;
}

#searchfirst.enabled:hover,
#searchlast.enabled:hover,
#searchprev.enabled:hover,
#searchnext.enabled:hover {
  cursor: pointer;
}

)";

  outputfile << ":root" << (themeswitching ? "[data-theme=\"" + (light ? "light"s : "dark") + "\"]" : "") << " {" << '\n';
  outputfile << "  /* " << (light ? "light" : "dark") << " */" << '\n';
  outputfile << "  --body-bgc: " << (light ? "#EDF0F6;" : "#000000;") << '\n';
  outputfile << "  --messageheader-c: " << (light ? "#000000;" : "#FFFFFF;") << '\n';
  outputfile << "  --conversationbox-bc: " << (light ? "#FBFCFF;" : "#1B1C1F;") << '\n';
  outputfile << "  --conversationbox-c: " << (light ? "#000000;" : "#FFFFFF;") << '\n';
  outputfile << "  --msgincoming-b: " << (light ? "#E7EBF3;" : "#303133;") << '\n';
  outputfile << "  --msgoutgoing-c: " << (light ? "#FFFFFF;" : "#FFFFFF;") << '\n';
  outputfile << "  --icon-f: " << (light ? "brightness(0);" : "none;") << '\n';
  outputfile << "  --menuitem-c: " << (light ? "#000000;" : "#FFFFFF;") << '\n';
  outputfile << "  --msg-incoming-bc-hover: " << (light ? "#F9FEFF;" : "#46474A;") << '\n';
  outputfile << "}" << '\n';
  outputfile << '\n';
  if (themeswitching)
  {
    outputfile << ":root" << (themeswitching ? "[data-theme=\"" + (!light ? "light"s : "dark") + "\"]" : "") << " {" << '\n';
    outputfile << "  /* " << (!light ? "light" : "dark") << " */" << '\n';
    outputfile << "  --body-bgc: " << (!light ? "#EDF0F6;" : "#000000;") << '\n';
    outputfile << "  --messageheader-c: " << (!light ? "#000000;" : "#FFFFFF;") << '\n';
    outputfile << "  --conversationbox-bc: " << (!light ? "#FBFCFF;" : "#1B1C1F;") << '\n';
    outputfile << "  --conversationbox-c: " << (!light ? "#000000;" : "#FFFFFF;") << '\n';
    outputfile << "  --msgincoming-b: " << (!light ? "#E7EBF3;" : "#303133;") << '\n';
    outputfile << "  --msgoutgoing-c: " << (!light ? "#FFFFFF;" : "#FFFFFF;") << '\n';
    outputfile << "  --icon-f: " << (!light ? "brightness(0);" : "none;") << '\n';
    outputfile << "  --menuitem-c: " << (!light ? "#000000;" : "#FFFFFF;") << '\n';
    outputfile << "  --msg-incoming-bc-hover: " << (!light ? "#F9FEFF;" : "#46474A;") << '\n';
    outputfile << "}" << '\n';
    outputfile << '\n';
  }
  outputfile << R"(
body {
  margin: 0px;
  padding: 0px;
  width: 100%;
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
  min-height: 100vh;
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
  color: var(--messageheader-c);
  font-family: Roboto, "Noto Sans", "Liberation Sans", OpenSans, sans-serif;
  padding-top: 30px;
  padding-bottom: 30px;
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

.msg {
  color: inherit;
  text-decoration: inherit;
  border: 1px solid var(--conversationbox-c);
  max-width: 50%;
  border-radius: .6em;
  margin: 7px 0;
  padding: 10px;
  position: relative;
  transition: background-color .1s;
}

.msg:hover {
  border: 2px solid var(--conversationbox-c);
  padding: 9px;
}

.msg-incoming:hover {
  background-color: var(--msg-incoming-bc-hover);
  transition: background-color .1s;
}

.msg-outgoing:hover {
  background-color: #4999E9;
  transition: background-color .1s;
}

.msg pre {
  font-family: Roboto, "Noto Sans", "Liberation Sans", OpenSans, sans-serif;
  white-space: pre-wrap;
  margin-top: 0px;
  margin-bottom: 5px;
  overflow-wrap: anywhere;
}

.msg pre a {
  color: #FFFFFF;
  text-decoration: underline;
}

.msg-incoming {
  align-self: flex-start;
  background: var(--msgincoming-b);
}

.msg-outgoing {
  align-self: flex-end;
  background: #336BA3;
  color: var(--msgoutgoing-c);
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

.msg-name {
  font-weight: bold;
  font-size: smaller;
  margin-bottom: 5px;
  display: block;
}

.msg-outgoing .msg-data {
  text-align: right;
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

.menu-item {
  display: flex;
  flex-direction: row;
  color: var(--menuitem-c);
  align-items: center;
  font-family: Roboto, "Noto Sans", "Liberation Sans", OpenSans, sans-serif;
  padding: 5px;
}

.threadtitle {
  font-family: Roboto, "Noto Sans", "Liberation Sans", OpenSans, sans-serif;
  padding: 0px;
  margin: 0px;
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

#theme {
  display: flex;
  flex-direction: column;
  position: fixed;
  top: 20px;
  right: 20px;
}

.themebutton {
  display: block;
  background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="white" stroke="white"><g id="g_0"><path d="M11.5 7.75c0-0.4 0.34-0.77 0.78-0.74C14.9 7.15 17 9.33 17 12c0 2.67-2.09 4.85-4.72 5-0.44 0.02-0.78-0.34-0.78-0.75v-8.5Z"/><path d="M12.97 0.73c-0.53-0.53-1.4-0.53-1.94 0L8.39 3.38H4.75c-0.76 0-1.37 0.61-1.37 1.37v3.64l-2.65 2.64c-0.53 0.53-0.53 1.4 0 1.94l2.65 2.64v3.64c0 0.76 0.61 1.38 1.37 1.38h3.64l2.64 2.64c0.53 0.53 1.4 0.53 1.94 0l2.64-2.63 3.64-0.01c0.76 0 1.38-0.62 1.38-1.38v-3.64l2.63-2.64c0.54-0.53 0.54-1.4 0-1.94l-2.62-2.61-0.01-3.67c0-0.76-0.62-1.38-1.38-1.38h-3.64l-2.64-2.64Zm-3.45 4L12 2.22l2.48 2.5c0.26 0.25 0.61 0.4 0.98 0.4h3.42v3.45c0.01 0.36 0.16 0.71 0.41 0.97L21.76 12l-2.48 2.48c-0.26 0.26-0.4 0.61-0.4 0.98v3.42h-3.43c-0.36 0.01-0.7 0.15-0.96 0.4L12 21.77l-2.48-2.48c-0.26-0.26-0.61-0.4-0.98-0.4H5.13v-3.42c0-0.37-0.15-0.72-0.4-0.98L2.22 12l2.5-2.48c0.25-0.26 0.4-0.61 0.4-0.98V5.13h3.41c0.37 0 0.72-0.15 0.98-0.4Z"></path></g></svg>');
  filter: var(--icon-f);
}

.nav-up {
  background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="white" stroke="white"><path d="M9.5,17.5l1.1,-1.1l-4.9,-4.9l-1.1,-0.8H17V9.2H4.6l1.1,-0.8l4.9,-5L9.5,2.5L2,10L9.5,17.5z"></path></svg>');
  filter: var(--icon-f);
}

.nav-one {
  background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none" stroke="white"><path style="stroke-width: 1.5;" d="M 13.796428,2.9378689 6.7339026,10.000394 13.795641,17.062131"></path></svg>');
  filter: var(--icon-f);
}

.nav-max {
  background-image: url('data:image/svg+xml;utf-8,<svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none" stroke="white"><path style="stroke-width: 1.5;" d="M 10.746186,2.9378689 3.6836603,10.000394 10.745399,17.062131"></path><path style="stroke-width: 1.5;" d="M 16.846186,2.9378689 9.7836603,10.000394 16.845399,17.062131"></path></svg>');
  filter: var(--icon-f);
}

.nav-fwd {
  transform: scaleX(-1);
}

.searchnav {
  margin-right: 0px;
  width: 15px;
  height: 15px;
  background-position: center;
  background-repeat: no-repeat;
  background-size: cover;
}

#searchfirst, #searchlast {
  margin: 7px;
}

@media print {
  #menu, #theme, #searchfirst, #searchlast, #searchprev, #searchnext {
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

  .msg.msg-incoming {
   margin-right: auto;
  }

  .msg.msg-outgoing {
    margin-left: auto;
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

  #message-header {
    padding-top: 0;
    padding-bottom: 10px;
    margin: auto;
    width: fit-content;
    /*leave it up to print settings */
    /* color: black;*/
  }

  /* todo: print style for audio, video and attachment previews */
} /* end @media print */

    </style>
  </head>

  <body>)" << '\n';

  if (themeswitching)
  {
    outputfile <<
      R"(
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
    </script>
)";
  }

  outputfile <<
    R"code(
    <input type="checkbox" id="theme-switch">
    <div id="page">
      <div class="controls-wrapper">
        <div class="conversation-wrapper">
          <div id="message-header">

              <input type="search" id="search_field" name="search_field">
              <button id="search_button" onclick="getSearchFieldAndSearch()">Search</button>
              <input type="checkbox" id="enable_regex" name="enable_regex">
              <label for="enable_regex">Regex</label>
              <input type="checkbox" id="enable_case_sensitive" name="enable_case_sensitive">
              <label for="enable_case_sensitive">Case sensitive</label>

              <div class="advancedsearch">

                <div style="text-align: left;">
                  <input type="checkbox" id="enable_date" name="enable_date" onclick="toggleAdvancedSearch('enable_date', 'limitdate')">
                  <label for="enable_date">Limit to dates: </label>
                  <label class="limitdate" for="mindate">From</label>
                  <input class="limitdate" type="date" id="mindate" name="mindate" disabled required>
                  <label class="limitdate" for="maxdate">To</label>
                  <input class="limitdate" type="date" id="maxdate" name="maxdate" disabled required>
                </div>

                <div style="text-align: left;">
                  <input type="checkbox" id="enable_recipient" name="enable_recipient" onclick="toggleAdvancedSearch('enable_recipient', 'limitrecipient')">
                  <label for="enable_recipient">Limit to contact: </label>

                  <select class="limitrecipient" name="recipientselector" id="recipientselector" disabled>
                  </select>
                </div>
              </div>
            </div>
            <div class="conversation-box" id="search_results">
              <div id="summary-box">
                <div id="searchfirst" class="searchnav nav-max disabled"></div>
                <div id="searchprev" class="searchnav nav-one disabled"></div>
                <div id="summary"></div>
                <div id="searchnext" class="searchnav nav-one nav-fwd disabled"></div>
                <div id="searchlast" class="searchnav nav-max nav-fwd disabled"></div>
              </div>
            </div>
          </div>
        </div>
      </div>

      <div id="menu">
        <a href="index.html">
          <div class="menu-item">
            <div class="menu-icon nav-up">
            </div>
            <div>
              index
            </div>
          </div>
        </a>
      </div>
    )code";
  if (themeswitching)
  {
    outputfile <<
      R"(
      <div id="theme">
        <div class="menu-item">
          <label for="theme-switch">
            <span class="menu-icon themebutton">
            </span>
          </label>
        </div>
      </div>
    )";
  }
  outputfile <<
    R"(
    <script src="searchidx.js"></script>
    <script>

      var max_per_page = 200;
      var global_results;
      var global_searchstring;
      var global_page = 0;
      var prevbutton = document.getElementById("searchprev");
      var nextbutton = document.getElementById("searchnext");
      var firstbutton = document.getElementById("searchfirst");
      var lastbutton = document.getElementById("searchlast");

      /* fill recipient selection list */
      recipient_idx.sort((a, b) => (a.display_name > b.display_name));
      for (i = 0; i < recipient_idx.length; ++i)
      {
          var list = document.getElementById("recipientselector");
          var listoption = document.createElement('option');
          listoption.setAttribute('value', recipient_idx[i]._id);
          listoption.append(recipient_idx[i].display_name);
          list.append(listoption);
      }

      function toggleAdvancedSearch(checkbox, id)
      {
          var elemstotoggle = document.getElementsByClassName(id);
          for (let e = 0; e < elemstotoggle.length; ++e)
          {
              if (document.getElementById(checkbox).checked)
                  elemstotoggle[e].removeAttribute('disabled');
              else
                  elemstotoggle[e].setAttribute('disabled', true);
          }
      }

      /* get url parameters */
      const url_params = new URLSearchParams(window.location.search);

      /* set initial dates */
      today = new Date();
      const mindateparam = url_params.get('mindate');
      if (mindateparam)
        document.getElementById('mindate').value = mindateparam;
      else
        document.getElementById('mindate').value = today.toISOString().substring(0, 10);

      const maxdateparam = url_params.get('maxdate');
      if (maxdateparam)
        document.getElementById('maxdate').value = maxdateparam;
      else
        document.getElementById('maxdate').value = today.toISOString().substring(0, 10);

      /* set initial recipient from url param */
      const recipientparam = url_params.get('recipient');
      var onlythread = false;
      if (recipientparam)
      {
        var rlimit = document.getElementById("recipientselector");
        for (var i, j = 0; i = rlimit.options[j]; ++j)
        {
          if (i.value == recipientparam)
          {
            rlimit.selectedIndex = j;
            var rlimit_enable = document.getElementById("enable_recipient");
            onlythread = true;
            rlimit_enable.click();
            break;
          }
        }
      }

      /* set initial case from url param */
      const caseparam = url_params.get('case');
      if (caseparam === "true")
        document.getElementById('enable_case_sensitive').checked = true;

      /* set regex from url param */
      const regexparam = url_params.get('regex');
      if (regexparam === "true")
        document.getElementById('enable_regex').checked = true;

      const directquery = url_params.get('query');
      if (directquery)
      {
        document.getElementById('search_field').value = directquery;
        getSearchFieldAndSearch();
      }
)";

  if (themeswitching)
  {
    outputfile << R"(
    const themeSwitch = document.querySelector('#theme-switch');

    themeSwitch.addEventListener('change', function(e)
    {
      if (e.currentTarget.checked === true)
      {
        //alert('Setting theme light');
        setCookie('theme', 'light');
        document.documentElement.dataset.theme = 'light';
      }
      else
      {
        //alert('Setting theme dark');
        setCookie('theme', 'dark');
        document.documentElement.dataset.theme = 'dark';
      }
    });)";
  }

  outputfile <<
    R"code(
    var input = document.getElementById("search_field");
    input.addEventListener("keypress", function(event)
                           {
                               if (event.key == "Enter")
                               {
                                   event.preventDefault();
                                   document.getElementById("search_button").click();
                               }
                           });

    function getSearchFieldAndSearch()
    {
      searchstr = document.getElementById('search_field').value;
      if (!searchstr || searchstr.length === 0)
        return;
      /*start = performance.now();*/
      global_results = search(message_idx, searchstr, document.getElementById('enable_regex').checked, document.getElementById('enable_case_sensitive').checked);
      /*end = performance.now();*/
      /*console.log(`Execution time: ${end - start} ms`);*/
      global_searchstring = searchstr;
      global_page = 0;
      showResults(document.getElementById('enable_case_sensitive').checked);
    }

    function search(obj, term, regex, case_sensitive)
    {
      var mindate = 0;
      var maxdate = 0;
      if (document.getElementById('enable_date').checked)
      {
        var d = new Date(document.getElementById("mindate").value);
        mindate = Date.UTC(d.getFullYear(), d.getMonth(), d.getDate()) / 1000 - 1404165600;
        var d = new Date(document.getElementById("maxdate").value);
        maxdate = Date.UTC(d.getFullYear(), d.getMonth(), d.getDate()) / 1000 - 1404165600;;
      }

      var recipient = -1;
      if (document.getElementById('enable_recipient').checked)
      {
        recipient = document.getElementById("recipientselector").value;
      }

      if (regex == true)
      {
        var flags = '';
        if (!case_sensitive)
          flags += 'i';
        const regex = RegExp(term, flags);
        //console.log(regex.flags);

        return obj.filter(element => regex.test(element.b) &&
                                     (document.getElementById('enable_date').checked === false || (element.d >= mindate && element.d <= maxdate)) &&
                                     (document.getElementById('enable_recipient').checked === false || (element.f == recipient || element.tr == recipient))).sort((r1, r2) => r1.d - r2.d);
      }
      if (case_sensitive == false)
        return obj.filter(element => element.b.toUpperCase().includes(term.toUpperCase()) &&
                                     (document.getElementById('enable_date').checked === false || (element.d >= mindate && element.d <= maxdate)) &&
                                     (document.getElementById('enable_recipient').checked === false || ((onlythread === false && element.f == recipient) || element.tr == recipient))).sort((r1, r2) => r1.d - r2.d);
      else
        return obj.filter(element => element.b.includes(term) &&
                                     (document.getElementById('enable_date').checked === false || (element.d >= mindate && element.d <= maxdate)) &&
                                     (document.getElementById('enable_recipient').checked === false || ((onlythread === false && element.f == recipient) || element.tr == recipient))).sort((r1, r2) => r1.d - r2.d);
    }

    function stringinsert(str, index, value)
    {
      return str.substr(0, index) + value + str.substr(index);
    }

    function surroundregex(rgxstr, str, pre, post, case_sensitive)
    {
      //console.log(rgxstr);
      var flags = 'gd';
      //console.log(case_sensitive)
      if (!case_sensitive)
        flags += 'i';
      const regex1 = RegExp(rgxstr, flags);
      let array1;
      var marked = 0;
      while ((array1 = regex1.exec(str)) !== null)
      {
        //console.log(`Found ${array1[0]}. Next starts at ${regex1.lastIndex}.`);
        //console.log(array1.indices[0]);
        str = stringinsert(str, array1.indices[0][0], pre);
        str = stringinsert(str, array1.indices[0][1] + pre.length, post);
        //console.log(str);
        regex1.lastIndex += pre.length + post.length;
        ++marked;
        if (marked > 10) // for safety, searching for '$' for example, would lead to infinite loop if regex was checked
          break;
      }
      return str;
    }

    function showResults(case_sensitive)
    {
      // remove old search results
      const elements = document.getElementsByClassName("searchresults");
      while (elements.length > 0)
        elements[0].parentNode.removeChild(elements[0]);

      summary = document.getElementById("summary");
      if (global_results.length == 0)
        summary.innerHTML = "(no results)";
      else
        summary.innerHTML = "Results " + (global_page * max_per_page + 1) + " - " + Math.min(global_page * max_per_page + max_per_page, global_results.length) + " out of " + global_results.length;

      for (i = global_page * max_per_page; i < Math.min(global_page * max_per_page + max_per_page, global_results.length); ++i)
      {
        // get displayname of 'from' id
        var index = recipient_idx.findIndex(function(item) {
          return item._id === global_results[i].f;
        });
        var displayname = recipient_idx[index].display_name;

        // get name of 'thread' id
        var index = recipient_idx.findIndex(function(item){
          return item._id === global_results[i].tr;
        });
        var threadname = recipient_idx[index].display_name;

        // add searchresults
        var elem = document.createElement('a');
        elem.classList.add("searchresults");
        elem.classList.add("msg");
        if (global_results[i].o == 1)
          elem.classList.add("msg-outgoing");
        else
          elem.classList.add("msg-incoming");
        elem.setAttribute('href', encodeURI(global_results[i].p + '.html#' + global_results[i].id));

        var linkdiv = document.createElement('div');

        var fromspan = document.createElement('span');
        fromspan.classList.add("msg-name");
        if (global_results[i].o == 0)
          fromspan.classList.add("msg-name-" + global_results[i].f);

        if (global_results[i].o == 1)
          fromspan.innerHTML = displayname + " (to <i>" + threadname + "</i>)";
        else
        {
          if (global_results[i].f === global_results[i].tr)
            fromspan.innerHTML = displayname;
          else
            fromspan.innerHTML = displayname + " (in <i>" + threadname + "</i>)";
        }
        linkdiv.append(fromspan);

        var msgbody = document.createElement('div');
        var prebody = document.createElement('pre');
        var body = document.createTextNode(global_results[i].b);
        prebody.append(body);

        // mark the found searchstring in html
        var markedbody = prebody.innerHTML;
        //console.log(markedbody);
        if (!document.getElementById('enable_regex').checked)
        {
          // escape any characters that happen to be special in regex
          var rgx = global_searchstring.replace(/[.*+\-?^${}()|[\]\\]/g, '\\$&'); // $& means the whole matched string
        }
        else
        {
          var rgx = global_searchstring;
        }
        markedbody = surroundregex(rgx, markedbody, '<mark>', '</mark>', case_sensitive);
        //console.log(markedbody);

        prebody.innerHTML = markedbody;
        msgbody.append(prebody);
        linkdiv.append(msgbody);

        var d = new Date(global_results[i].d * 1000 + 1404165600;);

        var msgdate = document.createElement('div');
        msgdate.classList.add("footer");
        var msgdatespan = document.createElement('span');
        msgdatespan.classList.add("msg-data");
        var date = document.createTextNode(d.toLocaleTimeString('en-us', {hour12: false, year:"numeric", month:"short", day:"numeric"}));
        msgdatespan.append(date);
        msgdate.append(msgdatespan);
        linkdiv.append(msgdate);
        elem.append(linkdiv);

        searchresultsdiv = document.getElementById("search_results");
        searchresultsdiv.append(elem);
      }

      if (global_page <= 0 || global_results.length == 0)
      {
        prevbutton.classList.remove("enabled");
        prevbutton.classList.add("disabled");
        prevbutton.removeEventListener("click", showResultsPrev);
        firstbutton.classList.remove("enabled");
        firstbutton.classList.add("disabled");
        firstbutton.removeEventListener("click", showResultsFirst);
      }
      else
      {
        prevbutton.classList.remove("disabled");
        prevbutton.classList.add("enabled");
        prevbutton.removeEventListener("click", showResultsPrev);
        prevbutton.addEventListener("click", showResultsPrev);
        firstbutton.classList.remove("disabled");
        firstbutton.classList.add("enabled");
        firstbutton.removeEventListener("click", showResultsFirst);
        firstbutton.addEventListener("click", showResultsFirst);
      }

      if ((global_page * max_per_page + max_per_page) >= global_results.length || global_results.length == 0)
      {
        nextbutton.classList.add("disabled");
        nextbutton.classList.remove("enabled");
        nextbutton.removeEventListener("click", showResultsNext);
        lastbutton.classList.add("disabled");
        lastbutton.classList.remove("enabled");
        lastbutton.removeEventListener("click", showResultsLast);
      }
      else
      {
        nextbutton.classList.remove("disabled");
        nextbutton.classList.add("enabled");
        nextbutton.removeEventListener("click", showResultsNext);
        nextbutton.addEventListener("click", showResultsNext);
        lastbutton.classList.remove("disabled");
        lastbutton.classList.add("enabled");
        lastbutton.removeEventListener("click", showResultsLast);
        lastbutton.addEventListener("click", showResultsLast);
      }
    }

    function showResultsFirst()
    {
      global_page = 0;
      showResults();
    }

    function showResultsPrev()
    {
      --global_page;
      if (global_page < 0)
        global_page = 0;
      showResults();
    }

    function showResultsNext()
    {
      ++global_page;
      showResults();
    }

    function showResultsLast()
    {
      global_page = Math.floor(global_results.length / max_per_page) - ((global_results.length % max_per_page) == 0);
      showResults();
    }

  </script>

  </body>
</html>
)code";

}
