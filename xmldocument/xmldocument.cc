/*
  Copyright (C) 2024-2025  Selwin van Dijk

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

#include "xmldocument.h"

#include <cstring>
#include <fstream>
#include <memory>

#define CASE_NUMBER case'0':case'1':case'2':case'3':case'4':case'5':case'6':case'7':case'8':case'9'
#define CASE_ALPHA_LOWER case'a':case'b':case'c':case'd':case'e':case'f':case'g':case'h':case'i':case'j':case'k':case'l':case'm':case'n':case'o':case'p':case'q':case'r':case's':case't':case'u':case'v':case'w':case'x':case'y':case'z'
#define CASE_ALPHA_UPPER case'A':case'B':case'C':case'D':case'E':case'F':case'G':case'H':case'I':case'J':case'K':case'L':case'M':case'N':case'O':case'P':case'Q':case'R':case'S':case'T':case'U':case'V':case'W':case'X':case'Y':case'Z'
#define CASE_ALPHA CASE_ALPHA_UPPER:CASE_ALPHA_LOWER
#define CASE_ALPHANUM CASE_ALPHA:CASE_NUMBER

XmlDocument::XmlDocument(std::string const &filename)
  :
  d_currentnode(&d_rootnode),
  d_ok(false)
{
  std::ifstream file(filename, std::ios_base::in | std::ios_base::binary);
  if (!file.is_open()) [[unlikely]]
  {
    Logger::error("Failed to open XML file for reading: '", filename, "'");
    return;
  }

  unsigned int constexpr buffer_size = 16 * 1024;
  //std::unique_ptr<char[]> buffer(new char[buffer_size]);
  char buffer[buffer_size];

  State state = INITIAL;
  State state_before_comment = INITIAL;
  std::string attribute_name_tmp;
  std::string attribute_value_tmp;
  long long int attribute_pos = -1;
  long long int attribute_size = 0;
  std::string closing_tag_tmp;
  bool has_root_element = false;
  while (true)
  {
    unsigned long long int filepos = file.tellg();
    file.read(buffer, buffer_size);
    unsigned int available = file.gcount();

    for (unsigned int i = 0; i < available; ++i)
    {
      switch (state) // order determined for SignalPlaintextBackup.xml
      {

        case ATTRIBUTE_VALUE_DOUBLE:       //                  v
        {                                  //  <tag attribute="...
          if (attribute_pos == -1) [[unlikely]]
          {
            attribute_pos = filepos + i;
            attribute_size = 0;
            // if (attribute_name_tmp == "data")
            //   std::cout << "attribute value starts at pos: " << attribute_pos << "(" << attribute_size << ")" << std::endl;
          }

          /**** NEW METHOD ****/
          // just quickly scan for closing quote,
          // this is not very strict, as we dont
          // care about invalid characters in the
          // attribute value ('<' '>' '&'(except in entity))
          char *closing_quote = static_cast<char *>(std::memchr(buffer + i, '"', available - i));
          if (closing_quote != nullptr)
          {
            attribute_size = attribute_size + (closing_quote - (buffer + i));
            attribute_value_tmp.append(buffer + i, attribute_size < Node::s_maxsize ? attribute_size - attribute_value_tmp.size() : 0);

            // if (attribute_name_tmp == "data")
            // {
            // std::cout << " - attribute: '" << attribute_name_tmp << "'='" << attribute_value_tmp << "'" << std::endl;
            // std::cout << "filepos : " << filepos << std::endl;
            // //std::cout << "i       : " << newbufferpos << std::endl;
            // std::cout << "att_pos : " << attribute_pos << std::endl;
            // std::cout << "att_size: " << attribute_size << std::endl;
            // std::cout << "attribute value ends at pos: " << (filepos + i + (closing_quote - (buffer.get() + i))) << " SIZE: " << attribute_size << std::endl;
            // }

            Node::StringOrRef attributevalue =
              {
                (attribute_size < Node::s_maxsize) ? std::move(attribute_value_tmp) : std::string(),
                (attribute_size < Node::s_maxsize) ? std::string() : filename,
                (attribute_size < Node::s_maxsize) ? -1 : attribute_pos,
                attribute_size
              };
            // if (attribute_name_tmp == "data")
            // {
            // std::cout << attributevalue.file << std::endl;
            // std::cout << attributevalue.value << std::endl;
            // std::cout << attributevalue.size << std::endl;
            // std::cout << attributevalue.pos << std::endl;
            // }
            d_currentnode->d_attributes[attribute_name_tmp] = std::move(attributevalue);

            attribute_value_tmp.clear();
            attribute_name_tmp.clear();
            attribute_pos = -1;

            i += (closing_quote - (buffer + i));

            //std::cout << "State: ATTRIBUTE_VALUE_DOUBLE -> ELEMENT_AFTER_TAGNAME" << std::endl;
            state = ELEMENT_AFTER_TAGNAME;
          }
          else
          {
            attribute_size += available - i;
            attribute_value_tmp.append(buffer + i, attribute_value_tmp.size() < Node::s_maxsize ? available - i : 0);
            i = available;
          }
          /**** END NEW METHOD ****/

          /**** OLD METHOD
          switch (buffer[i])
          {
            [[unlikely]] case '"':
            {
              attribute_size = (filepos + i) - attribute_pos;
              // std::cout << " - attribute: '" << attribute_name_tmp << "'='" << attribute_value_tmp << "'" << std::endl;
              // std::cout << "filepos : " << filepos << std::endl;
              // std::cout << "i       : " << i << std::endl;
              // std::cout << "att_pos : " << attribute_pos << std::endl;
              // std::cout << "att_size: " << attribute_size << std::endl;
              // std::cout << "attribute value ends at pos: " << (filepos + i) << " SIZE: " << attribute_size << std::endl;

              // {
              //   if (attribute_size > 0)
              //   {
              //     std::ifstream tmp(filename, std::ios_base::in | std::ios_base::binary);
              //     tmp.seekg(attribute_pos);
              //     std::unique_ptr<char[]> v(new char[attribute_size]);
              //     tmp.read(v.get(), attribute_size);
              //     std::cout << " *** " << std::string(v.get(), attribute_size) << std::endl;
              //   }
              // }

              Node::StringOrRef attributevalue =
                {
                  (attribute_size < Node::s_maxsize) ? std::move(attribute_value_tmp) : std::string(),
                  (attribute_size < Node::s_maxsize) ? std::string() : filename,
                  (attribute_size < Node::s_maxsize) ? -1 : attribute_pos,
                  attribute_size
                };

              d_currentnode->d_attributes[attribute_name_tmp] = std::move(attributevalue);
              attribute_value_tmp.clear();
              attribute_name_tmp.clear();
              attribute_pos = -1;

              //std::cout << "State: ATTRIBUTE_VALUE_DOUBLE -> ELEMENT_AFTER_TAGNAME" << std::endl;
              state = ELEMENT_AFTER_TAGNAME;
              break;
            }
            [[unlikely]] case '<': // ampersand is also not allowed as representing itself, but is used for entities (like &amp;)
            [[unlikely]] case '>':
            {
              Logger::error("Illegal character in attribute value: '", buffer[i], "'");
              return;
            }
            default:
            {
              attribute_value_tmp += buffer[i];
              break;
            }
          }
          END OLD METHOD ****/

          break;
        }

        case ATTRIBUTE_NAME:               //              v
        {                                  //  <tag attribu...
          switch (buffer[i])
          {
            case '=':
            {
              //std::cout << "State: ATTRIBUTE_NAME -> ATTRIBUTE_WAIT_QUOTE" << std::endl;
              state = ATTRIBUTE_WAIT_QUOTE;
              break;
            }
            CASE_ALPHANUM:
            case '-':
            case '_':
            case '.':
            {
              attribute_name_tmp += buffer[i];
              break;
            }
            case ' ':  // ignoring whitespace around '=' in attributename="attributevalue"
            case '\n': // this needs special case, as only a '=' is expected after this...
            case '\t': // that is, if we stay in ATTRIBUTE_NAME, the previous case should be denied.
            case '\r':
            {
              break;
            }
            [[unlikely]] default:
            {
              Logger::error("Illegal character in attribute name: '", buffer[i], "'");
              return;
            }
          }
          break;
        }

        case ELEMENT_AFTER_TAGNAME:    //      v
        {                              // <tag ...
          switch (buffer[i])
          {
            case '/':
            {
              //std::cout << "State: ELEMENT_AFTER_TAGNAME -> ELEMENT_SELFCLOSING_END " << std::endl;
              state = ELEMENT_SELFCLOSING_END;
              break;
            }
            case '>': // tag closed, but node open...
            {
              //std::cout << "State: ELEMENT_AFTER_TAGNAME -> ELEMENT_VALUE" << std::endl;
              state = ELEMENT_VALUE;
              break;
            }
            CASE_ALPHA:
            case '_':
            {
              //std::cout << "State: ELEMENT_AFTER_TAG_NAME -> ATTRIBUTE_NAME" << std::endl;
              state = ATTRIBUTE_NAME;
              attribute_name_tmp += buffer[i];
              break;
            }
            case ' ':  //ignore more spaces..
            case '\n':
            case '\t':
            case '\r':
            {
              break;
            }
            [[unlikely]] default:
            {
              Logger::error("Illegal character in ELEMENT_AFTER_TAGNAME? ('", buffer[i], "')");
              return;
            }
          }
          break;
        }

        case ATTRIBUTE_WAIT_QUOTE: //                v
        {                          // <tag attribute=...
          switch (buffer[i])
          {
            case '"':
            {
              //std::cout << "State: ATTRIBUTE_WAIT_QUOTE -> ATTRIBUTE_VALUE_DOUBLE " << std::endl;
              state = ATTRIBUTE_VALUE_DOUBLE;
              break;
            }
            case '\'':
            {
              //std::cout << "State: ATTRIBUTE_WAIT_QUOTE -> ATTRIBUTE_VALUE_SINGLE" << std::endl;
              state = ATTRIBUTE_VALUE_SINGLE;
              break;
            }
            case ' ':  // ignoring whitespace around '=' in attributename="attributevalue"
            case '\n':
            case '\t':
            case '\r':
            {
              break;
            }
            [[unlikely]] default:
            {
              Logger::error("Illegal character waiting for attribute value (must be quotation mark): '", buffer[i], "'");
              return;
            }
          }
          break;
        }

        case ELEMENT_TAG:   //   v
        {                   // <t...
          switch (buffer[i])
          {
            CASE_ALPHANUM: // Element names can contain letters, digits,
            case '-':      // hyphens, underscores, and periods
            case '_':
            case '.':
            {
              d_currentnode->d_name += buffer[i];
              break;
            }
            case ' ':
            case '\n':
            case '\t':
            case '\r':
            {
              //std::cout << "State: ELEMENT_TAG -> ELEMENT_AFTER_TAGNAME (" << d_currentnode->d_name << ")" << std::endl;
              state = ELEMENT_AFTER_TAGNAME;
              break;
            }
            case '>': // tag closed, but node open...
            {
              state = ELEMENT_VALUE;
              break;
            }
            [[unlikely]] default:
            {
              Logger::error("Illegal character in ELEMENT_TAG ('", buffer[i], "')");
              return;
            }
          }
          break;
        }

        case ELEMENT_VALUE:   //            v
        {                     // <tag [...]>...
          switch (buffer[i])
          {
            case '<':
            {
              //std::cout << "State: ELEMENT_VALUE -> PROLOG_ELEMENT_DTD_COMMENT" << std::endl;
              state = PROLOG_ELEMENT_DTD_COMMENT; // technically, prolog and dtd can not happen at this point...
              state_before_comment = ELEMENT_VALUE;
              if (d_currentnode->is_text_node && !d_currentnode->is_closed)
              {
                d_currentnode->is_closed = true;
                if (d_currentnode->d_value.find_first_not_of(" \n\t\r") == std::string::npos)
                {
                  d_currentnode = d_currentnode->d_parent;
                  d_currentnode->d_children.pop_back(); // delete the empty text node
                }
                else
                  d_currentnode = d_currentnode->d_parent;              }
              break;
            }
            //case 'something'/// illegal characters...
            default:
            {
              //std::cout << "Got char in element value: '" << buffer[i] << "'" << std::endl;
              //std::cout << "Current node exists: " << (d_currentnode ? "yes" : "no") << std::endl;
              if (!d_currentnode->is_text_node || d_currentnode->is_closed)
              {
                d_currentnode->d_children.emplace_back(d_currentnode);
                d_currentnode = &d_currentnode->d_children.back();
                d_currentnode->is_text_node = true;
              }
              d_currentnode->d_value += buffer[i];
            }
            break;
          }
          break;
        }

        case ELEMENT_SELFCLOSING_END:       //      v
        {                                   // <tag /...
          switch (buffer[i])
          {
            case '>':
            {
              // NODE CLOSED!
              //std::cout << " - Got node (closed!): '" << d_currentnode->d_name << "'" << std::endl;
              d_currentnode->is_closed = true;
              // I dont think the below is possible on a self closing tag, it cant have ever entered ELEMENT_VALUE
              //if (d_currentnode->d_value.find_first_not_of(" \n\t\r") == std::string::npos)
              //  d_currentnode->d_value.clear();
              d_currentnode = d_currentnode->d_parent;

              //std::cout << "State: ELEMENT_SELFCLOSING_END -> INITIAL" << std::endl;
              state = ELEMENT_VALUE; // not sure about this...
              break;
            }
            [[unlikely]] default:
            {
              Logger::error("Illegal character sequence in ELEMENT_SELFCLOSING_END? ('/", buffer[i], "')");
              return;
            }
          }
          break;
        }

        case PROLOG_ELEMENT_DTD_COMMENT:  //  v
        {                                 // <....
          switch (buffer[i])
          {
            CASE_ALPHA:
            case '_':
            {
              //std::cout << "State: PROLOG_ELEMENT_DTD_COMMENT -> ELEMENT_TAG" << std::endl;
              state = ELEMENT_TAG;

              if (has_root_element)
              {
                if (d_currentnode->is_closed)
                {
                  d_currentnode->d_parent->d_children.emplace_back(d_currentnode);
                  d_currentnode = &d_currentnode->d_parent->d_children.back();
                }
                else
                {
                  d_currentnode->d_children.emplace_back(d_currentnode);
                  d_currentnode = &d_currentnode->d_children.back();
                }
              }
              else
              {
                has_root_element = true;
              }

              d_currentnode->d_name += buffer[i];
              break;
            }
            case '?':
            {
              //std::cout << "State: PROLOG_ELEMENT_DTD_COMMENT -> PROLOG" << std::endl;
              state = PROLOG;
              break;
            }
            case '!':
            {
              //std::cout << "State: PROLOG_ELEMENT_DTD_COMMENT -> DTD_COMMENT" << std::endl;
              state = DTD_COMMENT;
              break;
            }
            case '/':
            {
              //std::cout << "State: PROLOG_ELEMENT_DTD_COMMENT -> CLOSING TAG" << std::endl;
              state = ELEMENT_CLOSING_TAG_START;
              break;
            }
            [[unlikely]] default:
            {
              Logger::error("Illegal char in PROLOG_ELEMENT_DTD_COMMENT state: '", buffer[i], "'");
              return;
            }
          }
          break;
        }

        // in the initial case, we expect only PROLOG, DTD, or ELEMENT (the root element)
        case INITIAL:
        {
          switch (buffer[i])
          {
            case '<':
            {
              state = PROLOG_ELEMENT_DTD_COMMENT;
              state_before_comment = INITIAL;
              //std::cout << "State: INITIAL -> PROLOG_ELEMENT_DTD_COMMENT" << std::endl;
              break;
            }
            case ' ':
            case '\n':
            case '\t':
            case '\r':
            {
              break;
            }
            [[unlikely]] default:
            {
              Logger::error("Illegal char in INITIAL state: '", buffer[i], "'");
              return;
            }
          }
          break;
        }

        case ELEMENT_CLOSING_TAG_START:    //         v
        {                                  // <open></...
          switch (buffer[i])
          {
            CASE_ALPHA:
            case '_':
            {
              closing_tag_tmp = buffer[i];
              state = ELEMENT_CLOSING_TAG;
              break;
            }
            [[unlikely]] default:
            {
              Logger::error("Illegal character in ELEMENT_CLOSING_TAG_START: '", buffer[i], "'");
              return;
            }
          }

          break;
        }

        case ELEMENT_CLOSING_TAG:       //           v
        {                               // <open></op...
          switch (buffer[i])
          {
            CASE_ALPHANUM:
            case '-':
            case '_':
            case '.':
            {
              closing_tag_tmp += buffer[i];
              break;
            }
            case '>':
            {
              // check name, close node!
              if (closing_tag_tmp != d_currentnode->d_name)
              {
                Logger::error("Non matching closing tag (open: '", d_currentnode->d_name, "' close: '", closing_tag_tmp, "')");
                return;
              }
              //std::cout << " - Got node (closed!): '" << d_currentnode->d_name << "'" << std::endl;
              d_currentnode->is_closed = true;

              if (d_currentnode->is_text_node && d_currentnode->d_value.find_first_not_of(" \n\t\r") == std::string::npos)
              {
                d_currentnode = d_currentnode->d_parent;
                d_currentnode->d_children.pop_back(); // delete the empty text node
              }
              else
                d_currentnode = d_currentnode->d_parent;

              if (d_currentnode == nullptr) [[unlikely]] // we just closed the root node, we should be done
                state = FINISHED;                        // no data can follow, but maybe a newline (or similar)
              else
                state = ELEMENT_VALUE; // prolog and dtd are impossible i think
              break;
            }
            [[unlikely]] default:
            {
              Logger::error("Illegal character in ELEMENT_CLOSING_TAG: '", buffer[i], "'");
              return;
            }
          }
          break;
        }

        case ATTRIBUTE_VALUE_SINGLE:       //                 v
        {                                  //  <tag attribute='...
          if (attribute_pos == -1) [[unlikely]]
          {
            attribute_pos = filepos + i;
            attribute_size = 0;
          }

          /**** NEW METHOD ****/
          // See ATTRIBUTE_VALUE_DOUBLE for notes and OLD METHOD
          char *closing_quote = static_cast<char *>(std::memchr(buffer + i, '\'', available - i));
          if (closing_quote != nullptr)
          {
            attribute_size = attribute_size + (closing_quote - (buffer + i));
            attribute_value_tmp.append(buffer + i, attribute_size < Node::s_maxsize ? attribute_size - attribute_value_tmp.size() : 0);

            Node::StringOrRef attributevalue = {
              (attribute_size < Node::s_maxsize) ? std::move(attribute_value_tmp) : std::string(),
              (attribute_size < Node::s_maxsize) ? std::string() : filename,
              (attribute_size < Node::s_maxsize) ? -1 : attribute_pos,
              attribute_size
            };
            d_currentnode->d_attributes[attribute_name_tmp] = std::move(attributevalue);

            attribute_value_tmp.clear();
            attribute_name_tmp.clear();
            attribute_pos = -1;

            i += (closing_quote - (buffer + i));

            state = ELEMENT_AFTER_TAGNAME;
          }
          else
          {
            attribute_size += available - i;
            attribute_value_tmp.append(buffer + i, attribute_value_tmp.size() < Node::s_maxsize ? available - i : 0);
            i = available;
          }
          /**** END NEW METHOD ****/
          break;
        }

        case PROLOG: // in the prolog, we only wait for '?>' to close it
        {
          switch (buffer[i])
          {
            case '?':
            {
              //std::cout << "State: PROLOG -> PROLOG_READ_QUESTIONMARK" << std::endl;
              state = PROLOG_READ_QUESTIONMARK;
              break;
            }
            default:
            {
              d_prolog += buffer[i];
              break;
            }
          }
          break;
        }

        case PROLOG_READ_QUESTIONMARK:
        {
          switch (buffer[i])
          {
            case '>': // the prolog has finished
            {
              //std::cout << " - prolog: '" << d_prolog << "'" << std::endl;
              //std::cout << "State: PROLOG_READ_QUESTIONMARK -> INITIAL" << std::endl;
              state = INITIAL;
              break;
            }
            default: // the '?' did not signal the end of the prolog
            {
              d_prolog += '?';
              d_prolog += buffer[i];
              //std::cout << "State: PROLOG_READ_QUESTIONMARK -> PROLOG" << std::endl;
              state = PROLOG;
            }
          }
          break;
        }

        case DTD_COMMENT:   //   v
        {                   // <!...
          switch (buffer[i])
          {
            case '-': // start of comment?
            {
              //std::cout << "State: DTD_COMMENT -> COMMENT_FIRST_OPEN_HYPHEN" << std::endl;
              state = COMMENT_FIRST_OPEN_HYPHEN;
              break;
            }
            case 'D': // Lets assume 'DOCTYPE'
            {
              //std::cout << "State: DTD_COMMENT -> DTD" << std::endl;
              state = DTD;
              break;
            }
          }
          break;
        }

        case COMMENT_FIRST_OPEN_HYPHEN:   //    v
        {                                 // <!-..
          if (buffer[i] == '-') [[likely]]
          {
            //std::cout << "State: COMMENT_FIRST_OPEN_HYPHEN -> COMMENT" << std::endl;
            state = COMMENT;
            break;
          }
          else
          {
            Logger::error("Illegal character sequence: '<!-", buffer[i], "'");
            return;
          }
          break;
        }

        case COMMENT:        //             v
        {                    // <!-- comment...
          switch (buffer[i])
          {
            case '-':
            {
              //std::cout << "State: COMMENT -> COMMENT_FIRST_CLOSE_HYPHEN" << std::endl;
              state = COMMENT_FIRST_CLOSE_HYPHEN;
              break;
            }
            default:
              break;
          }
          break;
        }

        case COMMENT_FIRST_CLOSE_HYPHEN:         //               v
        {                                        // <!-- comment -...
          switch (buffer[i])
          {
            case '-':
            {
              //std::cout << "State: COMMENT_FIRST_CLOSE_HYPHEN -> COMMENT_SECOND_CLOSE_HYPHEN" << std::endl;
              state = COMMENT_SECOND_CLOSE_HYPHEN;
              break;
            }
            default:
            {
              state = COMMENT;
              break;
            }
          }
          break;
        }

        case COMMENT_SECOND_CLOSE_HYPHEN:         //                v
        {                                         // <!-- comment --...
          switch (buffer[i])
          {
            case '>':
            {
              //std::cout << "State: COMMENT_SECOND_CLOSE_HYPHEN -> INITIAL" << std::endl;
              //std::cout << "comment closed, returning to state " << ((state_before_comment == INITIAL) ? "INITIAL" : (state_before_comment == ELEMENT_VALUE) ? "ELEMENT_VALUE" : "UNKNOWN") << std::endl;
              state = state_before_comment; // not sure about this...
              break;
            }
            [[unlikely]] default:
            {
              Logger::error("Illegal character sequence in COMMENT. ('--", buffer[i], "'");
              return;
            }
          }
          break;
        }

        case DTD:
        {
          switch (buffer[i])
          {
            case '[': // start of list
            {
              //std::cout << "State DTD -> DTD_LIST" << std::endl;
              state = DTD_LIST;
              break;
            }
            case '>': // end of DTD
            {
              //std::cout << "State DTD -> INITIAL" << std::endl;
              state = INITIAL;
              break;
            }
            default:
              break;
          }
          break;
        }

        case DTD_LIST:
        {
          switch (buffer[i])
          {
            case ']': // end of list
            {
              //std::cout << "State DTD_LIST -> DTD" << std::endl;
              state = DTD;
              break;
            }
            // case '<': -> DTD_ELEMENT_DTD_ENTITY_START ( I think need '!' in next state, then 'E', then 'L'/'N'
            default:
              break;
          }
          break;
        }

        case FINISHED:
        {
          switch (buffer[i])
          {
            case ' ':
            case '\n':
            case '\t':
            case '\r':
            {
              break;
            }
            default:
            {
              Logger::error("Illegal char in FINISHED state: '", buffer[i], "'");
              return;
            }
          }
          break;
        }
      }
    }
    if (!file)
      break;
  }
  d_ok = d_rootnode.is_closed;
}

#undef CASE_NUMBER
#undef CASE_ALPHA_LOWER
#undef CASE_ALPHA_UPPER
#undef CASE_ALPHA
#undef CASE_ALPHANUM
