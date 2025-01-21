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

#ifndef XMLDOCUMENT_H_
#define XMLDOCUMENT_H_

#include <vector>
#include <map>
#include <string>

#include "../logger/logger.h"
#include "../common_be.h"

class XmlDocument
{
 public:
  class Node
  {
   public:
    struct StringOrRef
    {
      std::string value; // for now
      std::string file;
      long long int pos = -1;
      long long int size = -1;
    };

   private:
    static int constexpr s_maxsize = 1024;

    Node *d_parent;
    std::vector<Node> d_children;
    std::map<std::string, StringOrRef> d_attributes;
    std::string d_name;
    std::string d_value; // make this a separate thing, so it can refer to file and position/size if size is too big
    //bool d_value_contains_ampersand; // just a helper if we have read the value during parsing anyway...
    bool is_closed;
    bool is_text_node;
   public:
    inline explicit Node(Node *parent = nullptr);
    void print(int indent = 0) const;

    inline bool isTextNode() const;
    inline std::string const &name() const;
    inline bool hasAttribute(std::string const &name) const;
    inline std::string getAttribute(std::string const &name) const;
    inline StringOrRef getAttributeStringOrRef(std::string const &name) const;

    inline auto begin() const;
    inline auto end() const;

    friend class XmlDocument;
  };

 private:
  enum State
  {
    INITIAL,
    FINISHED,
    PROLOG_ELEMENT_DTD_COMMENT,
    DTD_COMMENT,
    COMMENT,
    COMMENT_FIRST_OPEN_HYPHEN,
    COMMENT_FIRST_CLOSE_HYPHEN,
    COMMENT_SECOND_CLOSE_HYPHEN,
    //ATTRIBUTE_ELEMENT_END,
    ELEMENT_SELFCLOSING_END,
    ELEMENT_AFTER_TAGNAME,
    ATTRIBUTE_NAME,  // [a-zA-Z_]{1}[^ <>&]* (inside ELEMENT_TAG)
    ATTRIBUTE_WAIT_QUOTE,
    ATTRIBUTE_VALUE_SINGLE, // ['] (ends with ['], whichever opened it), after ATTRIBUTE_NAME and '='
    ATTRIBUTE_VALUE_DOUBLE, // ["] (ends with ["], whichever opened it), after ATTRIBUTE_NAME and '='
    ELEMENT_TAG,     // [a-zA-Z_]{1}[a-zA-Z_-.]*  , also not starting with (icase)"xml"
    ELEMENT_CLOSING_TAG_START,
    ELEMENT_CLOSING_TAG,
    ELEMENT_VALUE,
    PROLOG,          // <? (ends with ?>)
    PROLOG_READ_QUESTIONMARK,
    DTD,             // <!DOCTYPE
    DTD_LIST,        // [ (inside DTD)
    //DTD_ELEMENT, // <!ELEMENT (inside DTD_LIST)
    //DTD_ENTITY, //  <!ENTITY (inside DTD_LIST)
  };

  Node d_rootnode;
  Node *d_currentnode;
  std::string d_prolog;
  bool d_ok;

 public:
  explicit XmlDocument(std::string const &filename);
  inline bool ok() const;
  inline void print() const;
  inline Node const &root() const;
};

inline XmlDocument::Node::Node(Node *parent)
  :
  d_parent(parent),
  is_closed(false),
  is_text_node(false)
{}

inline void XmlDocument::Node::print(int indent) const
{
  if (!is_text_node)
  {
    Logger::message_start(std::string(indent, ' '), "<", d_name);
    for (auto const &[key, value] : d_attributes)
    {
      if (value.pos == -1) [[likely]]
        Logger::message_continue(" ", key, "=\"", value.value, "\""); // note, we should maybe scan for " and use ' if found
      else
        Logger::message_continue(" ", key, "=\"", "[large]", "\"");
    }
    Logger::message_continue((d_children.empty() && d_value.empty()) ? " />" : ">");
    if (d_value.empty())
      Logger::message_end();
    for (auto const &n : d_children)
      n.print(indent + 2);
    if (!d_children.empty() || !d_value.empty())
      Logger::message((d_value.empty() ? std::string(indent, ' ') : ""), "</", d_name, ">");
  }
  else
    Logger::message("'", d_value, "'");
}

inline bool XmlDocument::Node::isTextNode() const
{
  return is_text_node;
}

inline std::string const &XmlDocument::Node::name() const
{
  return d_name;
}

inline bool XmlDocument::Node::hasAttribute(std::string const &name) const
{
  return bepaald::contains(d_attributes, name);
}

inline std::string XmlDocument::Node::getAttribute(std::string const &name) const
{
  if (auto it = d_attributes.find(name); it != d_attributes.end()) [[likely]]
  {
    if (it->second.pos == -1)
      return it->second.value;
    else
    {
      std::ifstream tmp(it->second.file, std::ios_base::in | std::ios_base::binary);
      tmp.seekg(it->second.pos);
      std::unique_ptr<char[]> v(new char[it->second.size]);
      tmp.read(v.get(), it->second.size);
      return std::string(tmp.get(), it->second.size);
    }
  }
  return std::string();
}

inline XmlDocument::Node::StringOrRef XmlDocument::Node::getAttributeStringOrRef(std::string const &name) const
{
  if (auto it = d_attributes.find(name); it != d_attributes.end()) [[likely]]
    return it->second;
  return StringOrRef{};
}

inline auto XmlDocument::Node::begin() const
{
  return d_children.begin();
}

inline auto XmlDocument::Node::end() const
{
  return d_children.end();
}

inline bool XmlDocument::ok() const
{
  return d_ok;
}

inline void XmlDocument::print() const
{
  if (!d_prolog.empty())
    Logger::message("<?", d_prolog, "?>");
  d_rootnode.print();
}

inline XmlDocument::Node const &XmlDocument::root() const
{
  return d_rootnode;
}

#endif
