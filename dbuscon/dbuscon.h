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

/*
 *  !! NOTE !!
 *
 * This class is by no means a complete dbus service interface
 * it (hopefully) works for the current purpose of this program.
 *
 * There are certainly limitations in sending/receiving
 * complex nested types (DICTS of ARRAYS of DICTS...)
 *
 */

#ifndef DBUSCONNECTION_H_
#define DBUSCONNECTION_H_

#if !defined(_WIN32) && !defined(__MINGW64__) && (!defined(__APPLE__) || !defined(__MACH__))

#if !defined WITHOUT_DBUS

#include <dbus/dbus.h>
#include <cstring>
#include <memory>
#include <variant>
#include <type_traits>
#include <iomanip>
#include <vector>
#include <map>

#include "../logger/logger.h"

template<typename>
struct is_std_map : std::false_type {};

template<typename T, typename U>
struct is_std_map<std::map<T, U>> : std::true_type {};

template <typename T>
class recursive_wrapper
{
  // Wrapper over unique_ptr.
  std::unique_ptr<T> d_impl;

 public:
  // Automatic construction from a `T`, not a `T*`.
  recursive_wrapper(T &&obj) : d_impl(new T(std::move(obj))) {}
  recursive_wrapper(T const &obj) : d_impl(new T(obj)) {}

  // Copy constructor copies `T`.
  recursive_wrapper(const recursive_wrapper &other) : recursive_wrapper(*other.d_impl) {}
  recursive_wrapper &operator=(const recursive_wrapper &other)
  {
    *d_impl = *other.d_impl;
    return *this;
  }

  // unique_ptr destroys `T` for us.
  ~recursive_wrapper() = default;

  // Access propagates constness.
  T &operator*() { return *d_impl; }
  T const &operator*() const { return *d_impl; }

  T *operator->() { return d_impl.get(); }
  const T *operator->() const { return d_impl.get(); }
};


using DBusArg = std::variant<std::string, int32_t, int64_t, bool, recursive_wrapper<struct DBusVariant>, struct DBusObjectPath,
                             struct DBusArray, recursive_wrapper<struct DBusDict>>;

struct DBusObjectPath
{
  std::string d_value;
};

struct DBusArray : public std::vector<DBusArg>
{
  using std::vector<DBusArg>::vector;
};

struct DBusVariant
{
  DBusArg d_value;
};

struct DBusDictElement
{
  DBusArg d_key; // NOTE: only basic types are allowed as dict key by dbus spec
  DBusArg d_value;
};

struct DBusDict : public std::vector<DBusDictElement>
{
  using std::vector<DBusDictElement>::vector;
};

class DBusCon
{
  DBusError d_error;
  DBusConnection *d_connection;
  std::unique_ptr<DBusMessage, decltype(&::dbus_message_unref)> d_reply;
  bool d_dbus_verbose;
  bool d_ok;

 public:
  inline DBusCon(bool dbus_verbose);
  inline DBusCon(DBusCon const &other) = delete;
  inline DBusCon &operator=(DBusCon const &other) = delete;
  inline ~DBusCon();
  inline bool ok() const;

  inline void callMethod(std::string const &destination, std::string const &path, std::string const &interface, std::string const &method, std::vector<DBusArg> const &args);
  inline void callMethod(std::string const &destination, std::string const &path, std::string const &interface, std::string const &method);
  inline void showResponse(DBusMessage *reply);

  inline bool matchSignal(std::string const &matchingrule);
  inline bool waitSignal(int attempts, int timeoutms_per_attempt, std::string const &interface, std::string const &name);

  template <typename T>
  inline T get(std::string const &sig, std::vector<int> const &idx, T def = T{});

  template <typename T>
  inline T get(std::string const &sig, int idx, T def = T{});

 private:
  template <typename T>
  inline void addBasic(T const &t, DBusMessageIter *dbus_iter, bool isvar = false, bool isarray = false);
  inline void passArg(DBusDictElement const &arg, DBusMessageIter *dbus_iter);
  inline void passArg(DBusArg const &arg, DBusMessageIter *dbus_iter, bool isvar = false, bool isarray = false);
  inline void showresponse2(DBusMessageIter *iter, int indent);

  template <typename T>
  inline bool setBasicTypeReturn(T *target, int current_type, DBusMessageIter *iter);
  template <typename T>
  inline void set(T *ret, DBusMessageIter *iter, int current_type);
  template <typename T>
  inline T get2(DBusMessageIter *iter, std::vector<int> const &idx, T def = T{});
};

inline DBusCon::DBusCon(bool dbus_verbose)
  :
  d_connection(nullptr),
  d_reply(nullptr, &::dbus_message_unref),
  d_dbus_verbose(dbus_verbose),
  d_ok(false)
{
  dbus_error_init(&d_error);
  d_connection = dbus_bus_get_private(DBUS_BUS_SESSION, &d_error);

  if (d_connection)
    d_ok = true;
}

inline DBusCon::~DBusCon()
{
  if (d_connection)
  {
    dbus_connection_close(d_connection);
    dbus_connection_unref(d_connection);
  }
  if (dbus_error_is_set(&d_error))
    dbus_error_free(&d_error);
}

inline bool DBusCon::ok() const
{
  return d_ok;
}

inline bool DBusCon::matchSignal(std::string const &matchingrule)
{
  //Rules are specified as a string of comma separated key/value pairs. An example is "type='signal',sender='org.freedesktop.DBus', interface='org.freedesktop.DBus',member='Foo', path='/bar/foo',destination=':452345.34'"
  // Possible keys you can match on are type, sender, interface, member, path, destination and numbered keys to match message args (keys are 'arg0', 'arg1', etc.).
  dbus_bus_add_match(d_connection, matchingrule.c_str(), &d_error);
  if (dbus_error_is_set(&d_error))
  {
    Logger::error("::dbus_message_new_method_call - Unable to allocate memory for the message!");
    return false;
  }
  dbus_connection_flush(d_connection);
  return true;
}

inline bool DBusCon::waitSignal(int attempts, int timeoutms_per_attempt, std::string const &interface, std::string const &name)
{
  if (d_dbus_verbose) Logger::message_start("(waitSignal)");
  std::unique_ptr<DBusMessage, decltype(&::dbus_message_unref)> dbus_signal_msg(nullptr, &::dbus_message_unref);
  for (int i = 0; i < attempts; ++i)
  {
    if (d_dbus_verbose) Logger::message_continue(".");
    dbus_connection_read_write(d_connection, timeoutms_per_attempt);
    while (true)
    {
      dbus_signal_msg.reset(dbus_connection_pop_message(d_connection));
      if (!dbus_signal_msg)
        break;

      // showResponse(dbus_signal_msg.get());
      // std::cout << "sender " << dbus_message_get_sender(dbus_signal_msg.get()));
      // std::cout << "path   " << dbus_message_get_path(dbus_signal_msg.get()));
      // std::cout << "iface  " << dbus_message_get_interface(dbus_signal_msg.get()));
      // std::cout << "dest   " << dbus_message_get_destination(dbus_signal_msg.get()));
      // std::cout << "member " << dbus_message_get_member(dbus_signal_msg.get()));
      // std::cout << "type   " << dbus_message_get_type(dbus_signal_msg.get()));

      // check if the message is a signal from the correct interface and with the correct name
      if (dbus_message_is_signal(dbus_signal_msg.get(), interface.c_str(), name.c_str()))
      {
        if (d_dbus_verbose)
        {
          Logger::message(" *** RECEIVED SIGNAL WE WERE WATING FOR...");
          showResponse(dbus_signal_msg.get());
        }
        return true;
      }
      // else
      // {
      //   std::cout << "(different message received)");
      // }
    }
  }
  if (d_dbus_verbose) Logger::message_end();
  return false;
}

template <typename T>
inline void DBusCon::addBasic(T const &t, DBusMessageIter *dbus_iter, bool isvar, bool isarray [[maybe_unused]])
{
  if constexpr (std::is_same_v<T, DBusObjectPath>)
  {
    if (isvar)
    {
      DBusMessageIter dbus_iter_sub;
      dbus_message_iter_open_container(dbus_iter, DBUS_TYPE_VARIANT, DBUS_TYPE_OBJECT_PATH_AS_STRING, &dbus_iter_sub);
      dbus_message_iter_append_basic(&dbus_iter_sub, DBUS_TYPE_OBJECT_PATH, &t.d_value);
      dbus_message_iter_close_container(dbus_iter, &dbus_iter_sub);
    }
    else
      dbus_message_iter_append_basic(dbus_iter, DBUS_TYPE_OBJECT_PATH, &t.d_value);
  }
  if constexpr (std::is_same_v<T, std::string>)
  {
    if (isvar)
    {
      DBusMessageIter dbus_iter_sub;
      dbus_message_iter_open_container(dbus_iter, DBUS_TYPE_VARIANT, DBUS_TYPE_STRING_AS_STRING, &dbus_iter_sub);
      dbus_message_iter_append_basic(&dbus_iter_sub, DBUS_TYPE_STRING, &t);
      dbus_message_iter_close_container(dbus_iter, &dbus_iter_sub);
    }
    else
      dbus_message_iter_append_basic(dbus_iter, DBUS_TYPE_STRING, &t);
  }
  else if constexpr (std::is_same_v<T, int32_t>)
  {
    if (isvar)
    {
      DBusMessageIter dbus_iter_sub;
      dbus_message_iter_open_container(dbus_iter, DBUS_TYPE_VARIANT, DBUS_TYPE_INT32_AS_STRING, &dbus_iter_sub);
      dbus_message_iter_append_basic(&dbus_iter_sub, DBUS_TYPE_INT32, &t);
      dbus_message_iter_close_container(dbus_iter, &dbus_iter_sub);
    }
    dbus_message_iter_append_basic(dbus_iter, DBUS_TYPE_INT32, &t);
  }
  else if constexpr (std::is_same_v<T, int64_t>)
  {
    if (isvar)
    {
      DBusMessageIter dbus_iter_sub;
      dbus_message_iter_open_container(dbus_iter, DBUS_TYPE_VARIANT, DBUS_TYPE_INT64_AS_STRING, &dbus_iter_sub);
      dbus_message_iter_append_basic(&dbus_iter_sub, DBUS_TYPE_INT64, &t);
      dbus_message_iter_close_container(dbus_iter, &dbus_iter_sub);
    }
    dbus_message_iter_append_basic(dbus_iter, DBUS_TYPE_INT64, &t);
  }
  else if constexpr (std::is_same_v<T, bool>)
  {
    if (isvar)
    {
      DBusMessageIter dbus_iter_sub;
      dbus_message_iter_open_container(dbus_iter, DBUS_TYPE_VARIANT, DBUS_TYPE_BOOLEAN_AS_STRING, &dbus_iter_sub);
      dbus_message_iter_append_basic(&dbus_iter_sub, DBUS_TYPE_BOOLEAN, &t);
      dbus_message_iter_close_container(dbus_iter, &dbus_iter_sub);
    }
    int32_t b = t ? 1 : 0;
    dbus_message_iter_append_basic(dbus_iter, DBUS_TYPE_BOOLEAN, &b);
  }
}

inline void DBusCon::passArg(DBusDictElement const &arg, DBusMessageIter *dbus_iter)
{
  if (d_dbus_verbose) Logger::message("Got arg : DICTELEM");

  DBusMessageIter dbus_iter_dict;
  dbus_message_iter_open_container(dbus_iter, DBUS_TYPE_DICT_ENTRY, NULL, &dbus_iter_dict);
  passArg(arg.d_key, &dbus_iter_dict, false, false);
  passArg(arg.d_value, &dbus_iter_dict, false, false);
  dbus_message_iter_close_container(dbus_iter, &dbus_iter_dict);
}

inline void DBusCon::passArg(DBusArg const &arg, DBusMessageIter *dbus_iter, bool isvar, bool isarray)
{
  if (std::holds_alternative<int64_t>(arg))
  {
    if (d_dbus_verbose) Logger::message("Got arg : ", std::get<int64_t>(arg));
    addBasic(std::get<int64_t>(arg), dbus_iter, isvar, isarray);
  }
  else if (std::holds_alternative<int32_t>(arg))
  {
    if (d_dbus_verbose) Logger::message("Got arg : ", std::get<int32_t>(arg));
    addBasic(std::get<int32_t>(arg), dbus_iter, isvar, isarray);
  }
  else if (std::holds_alternative<std::string>(arg))
  {
    if (d_dbus_verbose) Logger::message("Got arg : '", std::get<std::string>(arg), "'");
    addBasic(std::get<std::string>(arg), dbus_iter, isvar, isarray);
  }
  else if (std::holds_alternative<bool>(arg))
  {
    if (d_dbus_verbose) Logger::message("Got arg : ", std::boolalpha, std::get<bool>(arg), std::noboolalpha);
    addBasic(std::get<bool>(arg), dbus_iter, isvar, isarray);
  }
  else if (std::holds_alternative<DBusArray>(arg))
  {
    if (d_dbus_verbose) Logger::message("Got arg : ", "ARRAY");

    DBusMessageIter dbus_array_iter;

    char const *arraytype = DBUS_TYPE_INVALID_AS_STRING;
    DBusArray const *array = &std::get<DBusArray>(arg);
    if (array->empty()) // something
      ;
    else
    {
      if (std::holds_alternative<int64_t>(array->at(0)))
        arraytype = DBUS_TYPE_INT64_AS_STRING;
      else if (std::holds_alternative<int32_t>(array->at(0)))
        arraytype = DBUS_TYPE_INT32_AS_STRING;
      else if (std::holds_alternative<std::string>(array->at(0)))
        arraytype = DBUS_TYPE_STRING_AS_STRING;
      else if (std::holds_alternative<bool>(array->at(0)))
        arraytype = DBUS_TYPE_BOOLEAN_AS_STRING;
      else if (std::holds_alternative<DBusArray>(array->at(0)))
        arraytype = DBUS_TYPE_ARRAY_AS_STRING;
      else if (std::holds_alternative<DBusObjectPath>(array->at(0)))
        arraytype = DBUS_TYPE_OBJECT_PATH_AS_STRING;
      else if (std::holds_alternative<recursive_wrapper<DBusVariant>>(array->at(0)))
        arraytype = DBUS_TYPE_VARIANT_AS_STRING;
      // to do?
      //else if (std::holds_alternative<recursive_wrapper<DBusDict>>(array->at(0)))
      //  arraytype = DBUS_TYPE_VARIANT_AS_STRING;
    }
    if (std::strcmp(arraytype, DBUS_TYPE_INVALID_AS_STRING) == 0)
      return;

    dbus_message_iter_open_container(dbus_iter, DBUS_TYPE_ARRAY, arraytype, &dbus_array_iter);
    for (unsigned int i = 0; i < array->size(); ++i)
      passArg(array->at(i), &dbus_array_iter, isvar, isarray);
    dbus_message_iter_close_container(dbus_iter, &dbus_array_iter);
  }
  else if (std::holds_alternative<DBusObjectPath>(arg))
  {
    if (d_dbus_verbose) Logger::message("Got arg : (o)'", std::get<DBusObjectPath>(arg).d_value, "'");
    addBasic(std::get<DBusObjectPath>(arg), dbus_iter, isvar, isarray);
  }
  else if (std::holds_alternative<recursive_wrapper<DBusVariant>>(arg))
  {
    if (d_dbus_verbose) Logger::message("Got arg : VARIANT");
    passArg(std::get<recursive_wrapper<DBusVariant>>(arg)->d_value, dbus_iter, true, isarray);
  }
  else if (std::holds_alternative<recursive_wrapper<DBusDict>>(arg))
  {
    if (d_dbus_verbose) Logger::message("Got arg : DICT");

    DBusMessageIter dbus_array_iter;
    std::string dictspec = DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING;
    if (std::get<recursive_wrapper<DBusDict>>(arg)->empty())
      ; // handle this somehow?
    else
    {
      DBusDictElement elem = std::get<recursive_wrapper<DBusDict>>(arg)->at(0);
      // key
      if (std::holds_alternative<int64_t>(elem.d_key))
        dictspec += DBUS_TYPE_INT64_AS_STRING;
      else if (std::holds_alternative<int32_t>(elem.d_key))
        dictspec += DBUS_TYPE_INT32_AS_STRING;
      else if (std::holds_alternative<std::string>(elem.d_key))
        dictspec += DBUS_TYPE_STRING_AS_STRING;
      else if (std::holds_alternative<bool>(elem.d_key))
        dictspec += DBUS_TYPE_BOOLEAN_AS_STRING;

      // value
      if (std::holds_alternative<int64_t>(elem.d_value))
        dictspec += DBUS_TYPE_INT64_AS_STRING;
      else if (std::holds_alternative<int32_t>(elem.d_value))
        dictspec += DBUS_TYPE_INT32_AS_STRING;
      else if (std::holds_alternative<std::string>(elem.d_value))
        dictspec += DBUS_TYPE_STRING_AS_STRING;
      else if (std::holds_alternative<bool>(elem.d_value))
        dictspec += DBUS_TYPE_BOOLEAN_AS_STRING;
      else if (std::holds_alternative<DBusObjectPath>(elem.d_value))
        dictspec += DBUS_TYPE_OBJECT_PATH_AS_STRING;
      else if (std::holds_alternative<recursive_wrapper<DBusVariant>>(elem.d_value)) // RECURSE! (?)
        dictspec += DBUS_TYPE_VARIANT_AS_STRING;
      else if (std::holds_alternative<DBusArray>(elem.d_value))                      // RECURSE! (?)
        dictspec += DBUS_TYPE_ARRAY_AS_STRING;
    }

    dictspec += DBUS_DICT_ENTRY_END_CHAR_AS_STRING;

    dbus_message_iter_open_container(dbus_iter, DBUS_TYPE_ARRAY, dictspec.c_str(), &dbus_array_iter);
    for (unsigned int i = 0; i < std::get<recursive_wrapper<DBusDict>>(arg)->size(); ++i)
      passArg(std::get<recursive_wrapper<DBusDict>>(arg)->at(i), &dbus_array_iter);
    dbus_message_iter_close_container(dbus_iter, &dbus_array_iter);
  }
}

inline void DBusCon::callMethod(std::string const &destination, std::string const &path, std::string const &interface, std::string const &method, std::vector<DBusArg> const &args)
{
  std::unique_ptr<DBusMessage, decltype(&::dbus_message_unref)> dbus_message(dbus_message_new_method_call(destination.c_str(), path.c_str(), interface.c_str(), method.c_str()), &::dbus_message_unref);
  if (!dbus_message)
  {
    Logger::error("::dbus_message_new_method_call - Unable to allocate memory for the message!");
    return;
  }

  // set args
  if (!args.empty())
  {
    DBusMessageIter dbus_iter;
    dbus_message_iter_init_append(dbus_message.get(), &dbus_iter);

    for (auto const &a : args)
      passArg(a, &dbus_iter);
  }

  // get reply
  d_reply.reset(dbus_connection_send_with_reply_and_block(d_connection, dbus_message.get(), DBUS_TIMEOUT_USE_DEFAULT, &d_error));
  if (!d_reply)
  {
    Logger::error(d_error.name, " : ", d_error.message);
    return;
  }

  // show output
  if (d_dbus_verbose)
    showResponse(d_reply.get());
}


inline void DBusCon::callMethod(std::string const &destination, std::string const &path, std::string const &interface, std::string const &method)
{
  return callMethod(destination, path, interface, method, {});
}

inline void DBusCon::showresponse2(DBusMessageIter *iter, int indent)
{
  // auto charsinnumber = [](int num)
  // {
  //   int i = 0;
  //   while (num /= 10)
  //     ++i;
  //   return i + 1;
  // };

  int current_type;
  int idx = 0;
  while ((current_type = dbus_message_iter_get_arg_type(iter)) != DBUS_TYPE_INVALID)
  {
    char *cursig = dbus_message_iter_get_signature(iter);
    Logger::message_start(std::string(indent, ' '), idx++, ". Got reply (", (char)current_type, ") (sig: \"", cursig, "\") : ");
    dbus_free(cursig);

    if (current_type == DBUS_TYPE_VARIANT || current_type == DBUS_TYPE_ARRAY || current_type == DBUS_TYPE_DICT_ENTRY || current_type == DBUS_TYPE_STRUCT)
    {
      Logger::message_continue(" -> recursing... ");
      if (current_type == DBUS_TYPE_ARRAY) Logger::message_continue("(", dbus_message_iter_get_element_count(iter), ")");
      Logger::message_end();

      DBusMessageIter iter_sub;
      dbus_message_iter_recurse(iter, &iter_sub);
      showresponse2(&iter_sub, indent + 4);
    }
    else if (current_type == DBUS_TYPE_OBJECT_PATH)
    {
      char *path;
      dbus_message_iter_get_basic(iter, &path);
      Logger::message_continue("VALUE (o): '", path, "'");
    }
    else if (current_type == DBUS_TYPE_STRING)
    {
      char *str;
      dbus_message_iter_get_basic(iter, &str);
      Logger::message_continue("VALUE (s): '", str, "'");
    }
    else if (current_type == DBUS_TYPE_INT32)
    {
      int32_t i = 0;
      dbus_message_iter_get_basic(iter, &i);
      Logger::message_continue("VALUE (i32): ", i);
    }
    else if (current_type == DBUS_TYPE_INT64)
    {
      int64_t i = 0;
      dbus_message_iter_get_basic(iter, &i);
      Logger::message_continue("VALUE (i64): ", i);
    }
    else if (current_type == DBUS_TYPE_BOOLEAN)
    {
      bool i = 0;
      dbus_message_iter_get_basic(iter, &i);
      Logger::message_continue("VALUE (b): ", std::boolalpha, i, std::noboolalpha);
    }
    else if (current_type == DBUS_TYPE_BYTE)
    {
      unsigned char b = '\0';
      dbus_message_iter_get_basic(iter, &b);
      std::isprint(b) ?
        Logger::message_continue("VALUE (byte): '", b, "' (0x", std::hex, std::setfill('0'), std::setw(2), (static_cast<int32_t>(b) & 0xFF), std::dec, ")") :
        Logger::message_continue("VALUE (byte): 0x", std::hex, std::setfill('0'), std::setw(2), (static_cast<int32_t>(b) & 0xFF), std::dec);
    }
    else
    {
      Logger::message_continue("[?]");
    }
    dbus_message_iter_next(iter);
  }
}

inline void DBusCon::showResponse(DBusMessage *reply)
{
  Logger::message(" -> Reply signature: ", dbus_message_get_signature(reply));

  DBusMessageIter dbus_iter_reply;
  dbus_message_iter_init(reply, &dbus_iter_reply);
  showresponse2(&dbus_iter_reply, 4);
}

template <typename T>
inline bool DBusCon::setBasicTypeReturn(T *target [[maybe_unused]], int current_type, DBusMessageIter *iter [[maybe_unused]])
{
  if constexpr (std::is_same_v<T, bool>)
  {
    if (current_type == DBUS_TYPE_BOOLEAN)
    {
      int32_t b = 0;
      dbus_message_iter_get_basic(iter, &b);
      *target = b;
      return true;
    }
  }
  else if constexpr (std::is_same_v<T, std::string>)
  {
    if ((current_type == DBUS_TYPE_STRING || current_type == DBUS_TYPE_OBJECT_PATH) && std::is_same_v<T, std::string>)
    {
      char *str;
      dbus_message_iter_get_basic(iter, &str);
      *target = str;
      return true;
    }
  }
  else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, int64_t>)
  {
    if (current_type == DBUS_TYPE_INT32 ||
        current_type == DBUS_TYPE_INT64)
    {
      dbus_message_iter_get_basic(iter, target);
      return true;
    }
  }
  return false;
}


template <typename T>
inline void DBusCon::set(T *ret, DBusMessageIter *iter, int current_type1)
{
  if constexpr (std::is_same_v<T, std::vector<std::string>>)
  {
    if (current_type1 == DBUS_TYPE_ARRAY)
    {
      DBusMessageIter iter_sub;
      dbus_message_iter_recurse(iter, &iter_sub);
      int current_type2;
      while ((current_type2 = dbus_message_iter_get_arg_type(&iter_sub)) != DBUS_TYPE_INVALID)
      {
        if (current_type2 == DBUS_TYPE_STRING || current_type2 == DBUS_TYPE_OBJECT_PATH)
        {
          char *val;
          dbus_message_iter_get_basic(&iter_sub, &val);
          ret->push_back(val);
        }
        else
        {
          ret->clear();
          return;
        }
        dbus_message_iter_next(&iter_sub);
      }
    }
  }

  if constexpr (std::is_same_v<T, std::vector<unsigned char>>)
  {
    if (current_type1 == DBUS_TYPE_ARRAY)
    {
      DBusMessageIter iter_sub;
      dbus_message_iter_recurse(iter, &iter_sub);
      int current_type2;
      while ((current_type2 = dbus_message_iter_get_arg_type(&iter_sub)) != DBUS_TYPE_INVALID)
      {
        if (current_type2 == DBUS_TYPE_BYTE)
        {
          unsigned char val;
          dbus_message_iter_get_basic(&iter_sub, &val);
          ret->push_back(val);
        }
        else
        {
          ret->clear();
          return;
        }
        dbus_message_iter_next(&iter_sub);
      }
    }
  }

  if constexpr (is_std_map<T>::value)
  {
    if (current_type1 == DBUS_TYPE_ARRAY)
    {
      DBusMessageIter iter_sub;
      dbus_message_iter_recurse(iter, &iter_sub);
      int current_type2;
      while ((current_type2 = dbus_message_iter_get_arg_type(&iter_sub)) != DBUS_TYPE_INVALID) // iterate the array
      {
        if (current_type2 != DBUS_TYPE_DICT_ENTRY)
        {
          ret->clear();
          return;
        }
        DBusMessageIter iter_sub2;
        dbus_message_iter_recurse(&iter_sub, &iter_sub2);

        typename T::key_type newkey;
        typename T::mapped_type newvalue;

        if ((current_type2 = dbus_message_iter_get_arg_type(&iter_sub2)) == DBUS_TYPE_INVALID)
        {
          ret->clear();
          return;
        }

        //std::cout << (char) current_type);

        // set key (must be basic type as per spec
        if (!setBasicTypeReturn(&newkey, current_type2, &iter_sub2))
        {
          ret->clear();
          return;
        }

        // next, get the mapped value. each DICT_ENTRY _must_ contain exactly 2 elements, as per spec
        dbus_message_iter_next(&iter_sub2);
        if ((current_type2 = dbus_message_iter_get_arg_type(&iter_sub2)) == DBUS_TYPE_INVALID)
        {
          ret->clear();
          return;
        }

        //std::cout << (char) current_type);

        if (setBasicTypeReturn(&newvalue, current_type2, &iter_sub2))
          ;
        else if (current_type2 == DBUS_TYPE_VARIANT) // recurse for this one
        {
          DBusMessageIter iter_sub3;
          dbus_message_iter_recurse(&iter_sub2, &iter_sub3);

          if ((current_type2 = dbus_message_iter_get_arg_type(&iter_sub3)) == DBUS_TYPE_INVALID)
          {
            ret->clear();
            return;
          }

          // std::cout << (char) current_type);

          if (setBasicTypeReturn(&newvalue, current_type2, &iter_sub3))
            ;
          else //(current_type == ...)
          { // only basic types inside variant for now...
            ret->clear();
            return;
          }
        }
        else // ALL OTHER TYPES NOT YET SUPPORTED FOR MAPPED VALUE
        {
          ret->clear();
          return;
        }

        // add pair...
        ret->insert({newkey, newvalue});

        dbus_message_iter_next(&iter_sub);
      }
    }
    return;
  }

  setBasicTypeReturn(ret, current_type1, iter);

  /*
  if constexpr (std::is_same_v<T, std::string>)
  {
    if (current_type == DBUS_TYPE_STRING || current_type == DBUS_TYPE_OBJECT_PATH)
    {
      char *val;
      dbus_message_iter_get_basic(iter, &val);
      *ret = val;
    }
  }

  if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, int64_t>)
  {
    if (current_type == DBUS_TYPE_INT32)
      dbus_message_iter_get_basic(iter, ret);
    else if (current_type == DBUS_TYPE_INT64)
      dbus_message_iter_get_basic(iter, ret);
  }

  if constexpr (std::is_same_v<T, bool>)
  {
    if (current_type == DBUS_TYPE_BOOLEAN)
      dbus_message_iter_get_basic(iter, ret);
  }
  */
}

template <typename T>
inline T DBusCon::get2(DBusMessageIter *iter, std::vector<int> const &idx, T def)
{
  T ret{def};
  int i = 0;
  int current_type = dbus_message_iter_get_arg_type(iter);
  while (i < idx.front())
  {
    //Logger::message("YO " << i << " TYPE: " << (char)current_type);

    dbus_message_iter_next(iter);
    current_type = dbus_message_iter_get_arg_type(iter);
    if (current_type == DBUS_TYPE_INVALID)
      return ret;

    ++i;
  }

  if (current_type == DBUS_TYPE_STRUCT)
  {
    DBusMessageIter iter_sub;
    dbus_message_iter_recurse(iter, &iter_sub);
    if (idx.size() < 2)
    {
      Logger::error("Missing next index");
      return ret;
    }
    std::vector idx2 = idx;
    idx2.erase(idx2.begin());
    ret = get2<T>(&iter_sub, idx2, def);
    return ret;
  }

  if (current_type == DBUS_TYPE_VARIANT)
  {
    DBusMessageIter iter_sub;
    dbus_message_iter_recurse(iter, &iter_sub);
    current_type = dbus_message_iter_get_arg_type(&iter_sub);
    set<T>(&ret, &iter_sub, current_type);
    return ret;
  }

  set<T>(&ret, iter, current_type);
  return ret;
}

template <typename T>
inline T DBusCon::get(std::string const &sig, std::vector<int> const &idx, T def)
{
  if (!d_reply || dbus_message_get_signature(d_reply.get()) != sig)
  {
    if (/*verbose && */d_reply)
      Logger::warning("Unexpected reply signature (got '", dbus_message_get_signature(d_reply.get()),
                      "', expected '", sig, "')");
    return T{def};
  }

  // get iterator
  DBusMessageIter iter;
  dbus_message_iter_init(d_reply.get(), &iter);

  return get2<T>(&iter, idx, def);
}

template <typename T>
inline T DBusCon::get(std::string const &sig, int idx, T def)
{
  return get<T>(sig, std::vector<int>{idx}, def);
}

#endif

#endif

#endif
