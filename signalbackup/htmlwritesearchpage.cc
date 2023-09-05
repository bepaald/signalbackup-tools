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

void SignalBackup::HTMLwriteSearchpage(std::string const &dir, bool light, bool themeswitching) const
{
  std::ofstream outputfile(dir + "/" + "searchpage.html", std::ios_base::binary);
  if (!outputfile.is_open())
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
              << ": Failed to open '" << dir << "/searchpage.html' for writing" << std::endl;
    return;
  }

  outputfile <<
    R"(<!DOCTYPE html>
<html>
  <head>
    <title>Signal conversation search</title>
    <style>
.searchresults {
  border: 1px solid black;
  margin: 5px;
  padding: 5px;
  max-width: 50%;
  color: inherit;
  text-decoration: inherit;
}

.searchresults:hover {
  background: red;
}

)";

  outputfile << ":root" << (themeswitching ? "[data-theme=\"" + (light ? "light"s : "dark") + "\"]" : "") << " {" << std::endl;
  outputfile << "  /* " << (light ? "light" : "dark") << " */" << std::endl;
  outputfile << "  --body-bgc: " << (light ? "#EDF0F6;" : "#000000;") << std::endl;
  outputfile << "  --messageheader-c: " << (light ? "#000000;" : "#FFFFFF;") << std::endl;
  outputfile << "  --conversationbox-bc: " << (light ? "#FBFCFF;" : "#1B1C1F;") << std::endl;
  outputfile << "  --conversationbox-c: " << (light ? "#000000;" : "#FFFFFF;") << std::endl;
  outputfile << "  --msgincoming-b: " << (light ? "#E7EBF3;" : "#303133;") << std::endl;
  outputfile << "  --msgoutgoing-c: " << (light ? "#FFFFFF;" : "#FFFFFF;") << std::endl;
  outputfile << "  --icon-f: " << (light ? "brightness(0);" : "none;") << std::endl;
  outputfile << "  --menuitem-c: " << (light ? "#000000;" : "#FFFFFF;") << std::endl;
  outputfile << "}" << std::endl;
  outputfile << std::endl;
  if (themeswitching)
  {
    outputfile << ":root" << (themeswitching ? "[data-theme=\"" + (!light ? "light"s : "dark") + "\"]" : "") << " {" << std::endl;
    outputfile << "  /* " << (!light ? "light" : "dark") << " */" << std::endl;
    outputfile << "  --body-bgc: " << (!light ? "#EDF0F6;" : "#000000;") << std::endl;
    outputfile << "  --messageheader-c: " << (!light ? "#000000;" : "#FFFFFF;") << std::endl;
    outputfile << "  --conversationbox-bc: " << (!light ? "#FBFCFF;" : "#1B1C1F;") << std::endl;
    outputfile << "  --conversationbox-c: " << (!light ? "#000000;" : "#FFFFFF;") << std::endl;
    outputfile << "  --msgincoming-b: " << (!light ? "#E7EBF3;" : "#303133;") << std::endl;
    outputfile << "  --msgoutgoing-c: " << (!light ? "#FFFFFF;" : "#FFFFFF;") << std::endl;
    outputfile << "  --icon-f: " << (!light ? "brightness(0);" : "none;") << std::endl;
    outputfile << "  --menuitem-c: " << (!light ? "#000000;" : "#FFFFFF;") << std::endl;
    outputfile << "}" << std::endl;
    outputfile << std::endl;
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

.incoming-group-msg {
  display: flex;
  flex-direction: row;
}

.msg-incoming {
  align-self: flex-start;
  background: var(--msgincoming-b);
}

.msg-sender-2 { background: #AA377A;}
.msg-sender-5 { background: #336BA3;}

.msg-name-2 { color: #AA377A;}
.msg-name-5 { color: #336BA3;}

.msg-outgoing {
  align-self: flex-end;
  background: #336BA3;
  color: var(--msgoutgoing-c);
}

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
  color: #FFFFFF;
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
    </style>
  </head>

  <body>)" << std::endl;

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
          document.cookie = name + "=" + (value || "")  + expires + "; path=/";
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
      const theme = getCookie('theme') || 'light';
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

              <div class="advancedsearch" style="padding: 5px;">

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
            </div>
          </div>
        </div>
      </div>

      <div id="menu">
        <a href="../index.html">
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

      /* set initial dates */
      today = new Date();
      mindates = document.getElementById('mindate');
      mindate.value = today.toISOString().substring(0, 10);
      maxdates = document.getElementById('maxdate');
      maxdate.value = today.toISOString().substring(0, 10);

      function mindateset(e)
      {
          // also change maxdate unless it's already set...
          if (document.getElementById("maxdate").value === "")
          {
              console.log('setting');
              document.getElementById("maxdate").value = document.getElementById("mindate").value;
          }
      }
)";

  if (themeswitching)
  {
    outputfile <<
      R"(const themeSwitch = document.querySelector('#theme-switch');
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
        str = document.getElementById('search_field').value;
        if (!str || str.length === 0 )
            return;

        start = performance.now();

        var r = search(message_idx, str, document.getElementById('enable_regex').checked);

        end = performance.now();
        console.log(`Execution time: ${end - start} ms`);

        showResults(r);
    }

    function search(obj, term, regex)
    {
      var mindate = 0;
      var maxdate = 0;
      if (document.getElementById('enable_date').checked)
      {
        var d = new Date(document.getElementById("mindate").value);
        mindate = Date.UTC(d.getFullYear(), d.getMonth(), d.getDate());
        var d = new Date(document.getElementById("maxdate").value);
        maxdate = Date.UTC(d.getFullYear(), d.getMonth(), d.getDate());
      }

      var recipient = -1;
      if (document.getElementById('enable_recipient').checked)
      {
      }

      if (regex == true)
      {
        const regex = RegExp(term, "i");
        return obj.filter(element => regex.test(element.body) &&
                                     (document.getElementById('enable_date').checked === false || (element.date >= mindate && element.date <= maxdate)) &&
                                     (document.getElementById('enable_recipient').checked === false || (element.from == recipient || element.thread_id == recipient))).sort((r1, r2) => r1.date - r2.date);
      }
      return obj.filter(element => element.body.toUpperCase().includes(term.toUpperCase()) &&
                                   (document.getElementById('enable_date').checked === false || (element.date >= mindate && element.date <= maxdate)) &&
                                   (document.getElementById('enable_recipient').checked === false || (element.from == recipient || element.thread_id == recipient))).sort((r1, r2) => r1.date - r2.date);
    }

    function showResults(results)
    {
    // remove old search results
        const elements = document.getElementsByClassName("searchresults");
        while (elements.length > 0)
            elements[0].parentNode.removeChild(elements[0]);

        if (results.length == 0)
        {
            // add element saying 'Too many results, showing 100'
            console.log("No results!");
        }

        if (results.length > 100)
        {
            // add element saying 'Too many results, showing 100'
            console.log("too many results!");
        }

        for (i = 0; i < Math.min(100, results.length); ++i)
        {
            // get displayname of 'from' id
            var index = recipient_idx.findIndex(function(item){
                return item._id === results[i].from;
            });
            var displayname = recipient_idx[index].display_name;

            // get name of 'thread' id
            var index = recipient_idx.findIndex(function(item){
                return item._id === results[i].thread_recipient_id;
            });
            var threadname = recipient_idx[index].display_name;

            // add searchresults
            var elem = document.createElement('a');
            elem.classList.add("searchresults");
            elem.classList.add("msg");
            if (results[i].outgoing == 1)
              elem.classList.add("msg-outgoing");
            else
              elem.classList.add("msg-incoming");
            elem.setAttribute('href', encodeURI(results[i].page + '#' + results[i]._id));

            var linkdiv = document.createElement('div');

            var fromspan = document.createElement('span');
            fromspan.classList.add("msg-name");
            if (results[i].outgoing == 0)
              fromspan.classList.add("msg-name-" + results[i].from);

            if (results[i].outgoing == 1)
                fromspan.innerHTML = displayname + " (to <i>" + threadname + "</i>)";
            else
            {
              if (results[i].from === results[i].thread_recipient_id)
                fromspan.innerHTML = displayname;
              else
                fromspan.innerHTML = displayname + " (in <i>" + threadname + "</i>)";
            }
            linkdiv.append(fromspan);

            var msgbody = document.createElement('div');
            var prebody = document.createElement('pre');
            var body = document.createTextNode(results[i].body);
            prebody.append(body);
            msgbody.append(prebody);
            linkdiv.append(msgbody);

            var d = new Date(results[i].date);

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
    }
  </script>

  </body>
</html>)code";

}
