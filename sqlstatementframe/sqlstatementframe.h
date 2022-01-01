/*
    Copyright (C) 2019-2022  Selwin van Dijk

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

#ifndef SQLSTATEMENTFRAME_H_
#define SQLSTATEMENTFRAME_H_

#include <locale>
#include <vector>
#include <memory>
#include <vector>
#include <any>

#include "../backupframe/backupframe.h"

struct Period final : std::numpunct<char>
{
  char do_decimal_point() const override { return '.'; } // make sure std::to_string always uses period for
};                                                       // decimal point, regardless of locale

class SqlStatementFrame : public BackupFrame
{
 public:
  enum PARAMETER_FIELD
  {
    STRING = 1, // string
    INT = 2,    // uint64
    DOUBLE = 3, // double
    BLOB = 4,   // bytes
    NULLPARAMETER = 5 // bool
  };

  enum FIELD
  {
    STATEMENT = 1,  // string
    PARAMETERS = 2  // PARAMETER_FIELD (repeated)
  };

 private:
  static Registrar s_registrar;

  std::vector<std::tuple<unsigned int, unsigned char *, uint64_t>> d_parameterdata; // PARAMETER_FIELD, bytes, size
  std::string d_statement;

 public:
  inline SqlStatementFrame();
  inline SqlStatementFrame(unsigned char *data, size_t length, uint64_t count = 0);
  inline SqlStatementFrame(SqlStatementFrame &&other);
  inline SqlStatementFrame &operator=(SqlStatementFrame &&other);
  inline SqlStatementFrame(SqlStatementFrame const &other);
  inline SqlStatementFrame &operator=(SqlStatementFrame const &other);
  inline virtual ~SqlStatementFrame();

  inline virtual FRAMETYPE frameType() const override;
  inline static BackupFrame *create(unsigned char *data, size_t length, uint64_t count = 0);
  inline virtual void printInfo() const override;
  inline void printInfo(std::vector<std::string> const &paramternames) const;

  inline std::string const &statement();
  inline std::pair<unsigned char *, uint64_t> getData() const override;

  inline void setStatementField(std::string const &val);
  inline void addStringParameter(std::string const &val);
  inline void addBlobParameter(std::pair<std::shared_ptr<unsigned char []>, size_t> const &val);
  inline void addIntParameter(int64_t val);
  inline void addNullParameter();
  inline void addDoubleParameter(double val);
  inline void addParameterField(PARAMETER_FIELD field, std::string const &val);

  inline std::string bindStatement() const;
  inline std::vector<std::any> parameters() const;

  // inline void setParameter(uint idx, unsigned char *data, uint32_t length);
  // inline void getParameter(uint idx) const;
  // inline std::string getParameterAsString(uint idx) const;
  // inline uint64_t getParameterAsUint64(uint idx) const;

  inline virtual bool validate() const override;

 private:
  void buildStatement();
  inline uint64_t dataSize() const;
};

inline SqlStatementFrame::SqlStatementFrame()
  :
  BackupFrame(-1)
{}

inline SqlStatementFrame::SqlStatementFrame(unsigned char *data, size_t length, uint64_t count)
  :
  BackupFrame(data, length, count)
{
  //std::cout << "CREATING SQLSTATEMENTFRAME" << std::endl;
  for (auto const &p : d_framedata)
    if (std::get<0>(p) == FIELD::PARAMETERS)
    {
      //std::cout << "INITIALIZING PARAMATERS: " << std::get<2>(p) << " bytes" << std::endl;
      [[unlikely]]if (!init(std::get<1>(p), std::get<2>(p), &d_parameterdata))
      {
        d_ok = false;
        break;
      }
    }
}

inline SqlStatementFrame::SqlStatementFrame(SqlStatementFrame &&other)
  :
  BackupFrame(std::move(other)),
  d_parameterdata(std::move(other.d_parameterdata))
{
  other.d_parameterdata.clear();
}

inline SqlStatementFrame &SqlStatementFrame::operator=(SqlStatementFrame &&other)
{
  if (this != &other)
  {
    // properly delete any data this is holding
    for (uint i = 0; i < d_parameterdata.size(); ++i)
      if (std::get<1>(d_parameterdata[i]))
        delete[] std::get<1>(d_parameterdata[i]);
    d_parameterdata.clear();

    BackupFrame::operator=(std::move(other));
    d_parameterdata = std::move(other.d_parameterdata);
    other.d_parameterdata.clear();
  }
  return *this;
}

inline SqlStatementFrame::SqlStatementFrame(SqlStatementFrame const &other)
  :
  BackupFrame(other),
  d_statement(other.d_statement)
{
  for (uint i = 0; i < other.d_parameterdata.size(); ++i)
  {
    unsigned char *datacpy = nullptr;
    if (std::get<1>(other.d_parameterdata[i]))
    {
      datacpy = new unsigned char[std::get<2>(other.d_parameterdata[i])];
      std::memcpy(datacpy, std::get<1>(other.d_parameterdata[i]), std::get<2>(other.d_parameterdata[i]));
    }
    d_parameterdata.emplace_back(std::make_tuple(std::get<0>(other.d_parameterdata[i]), datacpy, std::get<2>(other.d_parameterdata[i])));
  }
}

inline SqlStatementFrame &SqlStatementFrame::operator=(SqlStatementFrame const &other)
{
  if (this != &other)
  {
    BackupFrame::operator=(other);
    d_statement = other.d_statement;
    for (uint i = 0; i < other.d_parameterdata.size(); ++i)
    {
      unsigned char *datacpy = nullptr;
      if (std::get<1>(other.d_parameterdata[i]))
      {
        datacpy = new unsigned char[std::get<2>(other.d_parameterdata[i])];
        std::memcpy(datacpy, std::get<1>(other.d_parameterdata[i]), std::get<2>(other.d_parameterdata[i]));
      }
      d_parameterdata.emplace_back(std::make_tuple(std::get<0>(other.d_parameterdata[i]), datacpy, std::get<2>(other.d_parameterdata[i])));
    }
  }
  return *this;
}

inline SqlStatementFrame::~SqlStatementFrame()
{
  //std::cout << "DESTROYING SQLSTATEMENTFRAME" << std::endl;
  for (uint i = 0; i < d_parameterdata.size(); ++i)
    if (std::get<1>(d_parameterdata[i]))
      delete[] std::get<1>(d_parameterdata[i]);
  d_parameterdata.clear();
}

inline BackupFrame::FRAMETYPE SqlStatementFrame::frameType() const // virtual override
{
  return FRAMETYPE::SQLSTATEMENT;
}

inline BackupFrame *SqlStatementFrame::create(unsigned char *data, size_t length, uint64_t count) // static
{
  return new SqlStatementFrame(data, length, count);
}

inline void SqlStatementFrame::printInfo() const
{
  //DEBUGOUT("TYPE: SQLSTATEMENTFRAME");
  std::cout << "Frame number: " << d_count << std::endl;
  std::cout << "        Size: " << d_constructedsize << std::endl;
  std::cout << "        Type: SQLSTATEMENT" << std::endl;
  uint param_ctr = 0;

  for (auto const &p : d_framedata)
    if (std::get<0>(p) == FIELD::STATEMENT)
    {
      std::cout << "         - (statement: \"" << bepaald::bytesToString(std::get<1>(p), std::get<2>(p)) << "\" (" << std::get<2>(p) << " bytes)" << std::endl;
      break; // ONLY ONE FIELD::STATEMENT
    }

  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) == FIELD::PARAMETERS)
    {
      if (param_ctr < d_parameterdata.size())
      {
        switch (std::get<0>(d_parameterdata[param_ctr]))
        {
        case PARAMETER_FIELD::STRING:
          std::cout << "         - (string parameter): \"" << bepaald::bytesToString(std::get<1>(d_parameterdata[param_ctr]), std::get<2>(d_parameterdata[param_ctr])) << "\"" << std::endl;
          break;
        case PARAMETER_FIELD::INT:
          std::cout << "         - (uint64 parameter): \"" << bytesToUint64(std::get<1>(d_parameterdata[param_ctr]), std::get<2>(d_parameterdata[param_ctr])) << "\"" << std::endl;
          break;
        case PARAMETER_FIELD::DOUBLE:
          std::cout << "         - (double parameter): \"" << bepaald::toString(*reinterpret_cast<double *>(std::get<1>(d_parameterdata[param_ctr]))) << "\" " << bepaald::bytesToHexString(std::get<1>(d_parameterdata[param_ctr]), std::get<2>(d_parameterdata[param_ctr])) << std::endl;
          break;
        case PARAMETER_FIELD::BLOB:
          std::cout << "         - (binary parameter): \"" << bepaald::bytesToHexString(std::get<1>(d_parameterdata[param_ctr]), std::get<2>(d_parameterdata[param_ctr])) << "\"" << std::endl;
          break;
        case PARAMETER_FIELD::NULLPARAMETER:
          std::cout << "         - (bool parameter)  : \"" << std::boolalpha << (bytesToUint64(std::get<1>(d_parameterdata[param_ctr]), std::get<2>(d_parameterdata[param_ctr])) ? true : false) << "\" (value: \"" << bytesToUint64(std::get<1>(d_parameterdata[param_ctr]), std::get<2>(d_parameterdata[param_ctr])) << "\")" << std::endl;
          break;
        }
        ++param_ctr;
      }
    }
  }
}

inline void SqlStatementFrame::printInfo(std::vector<std::string> const &parameternames) const
{
  if (d_parameterdata.size() != parameternames.size())
  {
    std::cout << d_parameterdata.size() << std::endl;
    std::cout << parameternames.size() << std::endl;
    return printInfo();
  }

  //DEBUGOUT("TYPE: SQLSTATEMENTFRAME");
  std::cout << "Frame number: " << d_count << std::endl;
  std::cout << "        Type: SQLSTATEMENT" << std::endl;
  uint param_ctr = 0;

  for (auto const &p : d_framedata)
    if (std::get<0>(p) == FIELD::STATEMENT)
    {
      std::cout << "         - (statement: \"" << bepaald::bytesToString(std::get<1>(p), std::get<2>(p)) << "\" (" << std::get<2>(p) << " bytes)" << std::endl;
      break; // only one FIELD::STATEMENT
    }

  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) == FIELD::PARAMETERS)
    {
      if (param_ctr < d_parameterdata.size())
      {
        switch (std::get<0>(d_parameterdata[param_ctr]))
        {
        case PARAMETER_FIELD::STRING:
          std::cout << "         - " << parameternames[param_ctr] << " (string parameter): \""
                    << bepaald::bytesToString(std::get<1>(d_parameterdata[param_ctr]), std::get<2>(d_parameterdata[param_ctr])) << "\"" << std::endl;
          break;
        case PARAMETER_FIELD::INT:
          std::cout << "         - " << parameternames[param_ctr] << " (uint64 parameter): \""
                    << bytesToUint64(std::get<1>(d_parameterdata[param_ctr]), std::get<2>(d_parameterdata[param_ctr])) << "\"" << std::endl;
          break;
        case PARAMETER_FIELD::DOUBLE:
          std::cout << "         - " << parameternames[param_ctr] << " (double parameter): \""
                    << *reinterpret_cast<double *>(std::get<1>(d_parameterdata[param_ctr]))<< "\" " << bepaald::bytesToHexString(std::get<1>(d_parameterdata[param_ctr]), std::get<2>(d_parameterdata[param_ctr])) << std::endl;
          break;
        case PARAMETER_FIELD::BLOB:
          std::cout << "         - " << parameternames[param_ctr] << " (binary parameter): \""
                    << bepaald::bytesToHexString(std::get<1>(d_parameterdata[param_ctr]), std::get<2>(d_parameterdata[param_ctr])) << "\"" << std::endl;
          break;
        case PARAMETER_FIELD::NULLPARAMETER:
          std::cout << "         - " << parameternames[param_ctr] << " (bool parameter): \""
                    << std::boolalpha << (bytesToUint64(std::get<1>(d_parameterdata[param_ctr]), std::get<2>(d_parameterdata[param_ctr])) ? true : false)
                    << "\" (value: \"" << bytesToUint64(std::get<1>(d_parameterdata[param_ctr]), std::get<2>(d_parameterdata[param_ctr])) << "\")" << std::endl;
          break;
        }
        ++param_ctr;
      }
    }
  }
}

std::string const &SqlStatementFrame::statement()
{
  if (d_statement.empty())
    buildStatement();
  return d_statement;
}

inline uint64_t SqlStatementFrame::dataSize() const
{
  uint64_t size = 0;
  for (auto const &fd : d_framedata)
  {
    if (std::get<0>(fd) == FIELD::STATEMENT)
    {
      uint64_t statementsize = std::get<2>(fd); // length of actual data
      // length of length
      size += varIntSize(statementsize);
      size += statementsize + 1; // plus one for fieldtype + wiretype
    }
  }
  for (auto const &pd : d_parameterdata)
  {
    switch (std::get<0>(pd))
    {
    case PARAMETER_FIELD::STRING:
      {
        uint64_t stringsize = std::get<2>(pd);
        size += varIntSize(stringsize);
        size += stringsize + 1; // +1 for fieldtype + wiretype

        size += varIntSize(stringsize + 2); // to write size of parameter field into

        break;
      }
    case PARAMETER_FIELD::INT:
      {
        uint64_t value = bytesToUint64(std::get<1>(pd), std::get<2>(pd));
        size += varIntSize(value);
        size += 1; // for fieldtype + wiretype
        size += varIntSize(varIntSize(value) + 1); // to write size of parameter field into
        break;
      }
    case PARAMETER_FIELD::DOUBLE:
      {
        size += 9; // fixed64 + 1 for field and wiretype?
        size += varIntSize(9 + 1); // for parameter_field + type
        break;
      }
    case PARAMETER_FIELD::BLOB:
      {
        uint64_t stringsize = std::get<2>(pd);
        size += varIntSize(stringsize);
        size += stringsize + 1;
        size += varIntSize(stringsize + 2); // to write size of parameter field into
        break;
      }
    case PARAMETER_FIELD::NULLPARAMETER:
      {
        uint64_t value = bytesToUint64(std::get<1>(pd), std::get<2>(pd));
        size += varIntSize(value);
        size += 1; // for fieldtype + wiretype
        size += varIntSize(varIntSize(value) + 1); // to write size of parameter field into
        break;
      }
    }
    size += 1; // for fieldtype + wiretype?
  }
  // for size of this entire frame.
  size += varIntSize(size);

  return ++size;  // for frametype and wiretype
}

inline std::pair<unsigned char *, uint64_t> SqlStatementFrame::getData() const
{
  uint64_t size = dataSize();
  unsigned char *data = new unsigned char[size];
  uint64_t datapos = 0;

  datapos += setFieldAndWire(FRAMETYPE::SQLSTATEMENT, WIRETYPE::LENGTHDELIM, data + datapos);
  datapos += setFrameSize(size, data + datapos);

  uint param_ctr = 0;

  for (auto const &fd : d_framedata)
    if (std::get<0>(fd) == FIELD::STATEMENT)
    {
      datapos += putLengthDelimType(fd, data + datapos);
      break; // only one FIELD::STATEMENT
    }

  for (auto const &fd : d_framedata)
  {
    if (std::get<0>(fd) == FIELD::PARAMETERS)
    {
      if (param_ctr < d_parameterdata.size())
      {
        switch (std::get<0>(d_parameterdata[param_ctr]))
        {
        case PARAMETER_FIELD::STRING:
        case PARAMETER_FIELD::BLOB:
        {
          datapos += setFieldAndWire(FIELD::PARAMETERS, WIRETYPE::LENGTHDELIM, data + datapos);
          //                                     size of string    field+wire   number of bytes for length of string
          //                                           v                 v       v                    v
          datapos += putVarInt(std::get<2>(d_parameterdata[param_ctr]) + 1 + varIntSize(std::get<2>(d_parameterdata[param_ctr])), data + datapos);

          datapos += putLengthDelimType(d_parameterdata[param_ctr], data + datapos);
          break;
        }
        case PARAMETER_FIELD::INT:
        case PARAMETER_FIELD::NULLPARAMETER:
        {
          uint64_t value = bytesToUint64(std::get<1>(d_parameterdata[param_ctr]), std::get<2>(d_parameterdata[param_ctr]));
          datapos += setFieldAndWire(FIELD::PARAMETERS, WIRETYPE::LENGTHDELIM, data + datapos);
          datapos += putVarInt(varIntSize(value) + 1, data + datapos);

          datapos += putVarIntType(d_parameterdata[param_ctr], data + datapos);
          break;
        }
        case PARAMETER_FIELD::DOUBLE:
        {
          datapos += setFieldAndWire(FIELD::PARAMETERS, WIRETYPE::LENGTHDELIM, data + datapos);
          datapos += putVarInt(8 + 1, data + datapos);

          datapos += putFixed64Type(d_parameterdata[param_ctr], data + datapos);
          break;
        }
        }
      }
      ++param_ctr;
    }
  }
  //std::cout << "2 DID " << param_ctr << " PARAMETERS!" << std::endl;

  //std::cout << "               " << bepaald::bytesToHexString(data, size) << std::endl;

  return {data, size};
}

inline void SqlStatementFrame::setStatementField(std::string const &val)
{
  unsigned char *temp = new unsigned char[val.length()];
  std::memcpy(temp, val.c_str(), val.length());
  d_framedata.emplace_back(std::make_tuple(static_cast<unsigned int>(FIELD::STATEMENT), temp, val.length()));
}

inline void SqlStatementFrame::addStringParameter(std::string const &val)
{
  //std::cout << "Adding string parameter: " << val << std::endl;
  unsigned char *temp = new unsigned char[val.length()];
  std::memcpy(temp, val.c_str(), val.length());
  d_parameterdata.emplace_back(std::make_tuple(PARAMETER_FIELD::STRING, temp, val.length()));
  d_framedata.emplace_back(std::make_tuple(FIELD::PARAMETERS, nullptr, 0));
}

inline void SqlStatementFrame::addBlobParameter(std::pair<std::shared_ptr<unsigned char []>, size_t> const &val)
{
  unsigned char *temp = new unsigned char[val.second];
  std::memcpy(temp, val.first.get(), val.second);
  d_parameterdata.emplace_back(std::make_tuple(PARAMETER_FIELD::BLOB, temp, val.second));
  d_framedata.emplace_back(std::make_tuple(FIELD::PARAMETERS, nullptr, 0));
}

inline void SqlStatementFrame::addIntParameter(int64_t val)
{
  val = bepaald::swap_endian(val);
  unsigned char *temp = new unsigned char[sizeof(val)];
  std::memcpy(temp, reinterpret_cast<unsigned char *>(&val), sizeof(val));
  d_parameterdata.emplace_back(std::make_tuple(PARAMETER_FIELD::INT, temp, sizeof(val)));
  d_framedata.emplace_back(std::make_tuple(FIELD::PARAMETERS, nullptr, 0));
}

inline void SqlStatementFrame::addNullParameter()
{
  int64_t num = 1;
  int64_t val = bepaald::swap_endian(num);
  unsigned char *temp = new unsigned char[sizeof(val)];
  std::memcpy(temp, reinterpret_cast<unsigned char *>(&val), sizeof(val));
  d_parameterdata.emplace_back(std::make_tuple(PARAMETER_FIELD::NULLPARAMETER, temp, sizeof(val)));
  d_framedata.emplace_back(std::make_tuple(FIELD::PARAMETERS, nullptr, 0));
}

inline void SqlStatementFrame::addDoubleParameter(double val)
{
  unsigned char *temp = new unsigned char[sizeof(val)];
  std::memcpy(temp, reinterpret_cast<unsigned char *>(&val), sizeof(val));
  d_parameterdata.emplace_back(std::make_tuple(PARAMETER_FIELD::DOUBLE, temp, sizeof(val)));
  d_framedata.emplace_back(std::make_tuple(FIELD::PARAMETERS, nullptr, 0));
}

inline void SqlStatementFrame::addParameterField(PARAMETER_FIELD field, std::string const &val)
{
  switch (field)
  {
  case PARAMETER_FIELD::STRING:
  case PARAMETER_FIELD::BLOB:
    {
      unsigned char *temp = new unsigned char[val.length()];
      std::memcpy(temp, val.c_str(), val.length());
      d_parameterdata.emplace_back(std::make_tuple(field, temp, val.length()));
      d_framedata.emplace_back(std::make_tuple(FIELD::PARAMETERS, nullptr, 0));
      break;
    }
  case PARAMETER_FIELD::INT:
  case PARAMETER_FIELD::NULLPARAMETER:
    {
      uint64_t num = std::stoull(val);
      num = bepaald::swap_endian(num);
      unsigned char *temp = new unsigned char[sizeof(num)];
      std::memcpy(temp, reinterpret_cast<unsigned char *>(&num), sizeof(num));
      d_parameterdata.emplace_back(std::make_tuple(field, temp, sizeof(num)));
      d_framedata.emplace_back(std::make_tuple(FIELD::PARAMETERS, nullptr, 0));
      break;
    }
  case PARAMETER_FIELD::DOUBLE:
    {
      double num = std::stod(val);
      unsigned char *temp = new unsigned char[sizeof(num)];
      std::memcpy(temp, reinterpret_cast<unsigned char *>(&num), sizeof(num));
      d_parameterdata.emplace_back(std::make_tuple(field, temp, sizeof(num)));
      d_framedata.emplace_back(std::make_tuple(FIELD::PARAMETERS, nullptr, 0));
      break;
    }
  }
}

/*
inline void SqlStatementFrame::setParameter(uint idx, unsigned char *data, uint32_t length)
{
  if (std::get<1>(d_parameterdata[idx]))
  {
    delete[] std::get<1>(d_parameterdata[idx]);
    std::get<1>(d_parameterdata[idx]) = new unsigned char[length];
    std::memcpy(std::get<1>(d_parameterdata[idx]), data, length);
    std::get<2>(d_parameterdata[idx]) = length;
  }
}

inline void SqlStatementFrame::getParameter(uint idx) const
{
  if (std::get<1>(d_parameterdata[idx]))
  {
    std::cout << bepaald::bytesToString(std::get<1>(d_parameterdata[idx]), std::get<2>(d_parameterdata[idx])) << std::endl;
  }
  else
    std::cout << "nullptr" << std::endl;
}

inline std::string SqlStatementFrame::getParameterAsString(uint idx) const
{
  if (std::get<1>(d_parameterdata[idx]))
    return bepaald::bytesToString(std::get<1>(d_parameterdata[idx]), std::get<2>(d_parameterdata[idx]));
  return "";
}

inline uint64_t SqlStatementFrame::getParameterAsUint64(uint idx) const
{
  if (std::get<1>(d_parameterdata[idx]))
    return bytesToUint64(std::get<1>(d_parameterdata[idx]), std::get<2>(d_parameterdata[idx]));
  return 0;
}
*/

inline std::string SqlStatementFrame::bindStatement() const
{
  for (auto const &p : d_framedata)
    if (std::get<0>(p) == FIELD::STATEMENT)
      return bepaald::bytesToString(std::get<1>(p), std::get<2>(p));
  return std::string();
}

inline std::vector<std::any> SqlStatementFrame::parameters() const
{
  std::vector<std::any> parameters;
  for (auto const &p : d_parameterdata)
  {
    switch (std::get<0>(p))
    {
    case PARAMETER_FIELD::INT:
      {
        parameters.emplace_back(static_cast<long long int>(bytesToUint64(std::get<1>(p), std::get<2>(p))));
        break;
      }
    case PARAMETER_FIELD::STRING:
      {
        //if ()
        //std::cout << "Retunring string paramter: " << bepaald::bytesToString(std::get<1>(p), std::get<2>(p)) << std::endl;
        parameters.emplace_back(bepaald::bytesToString(std::get<1>(p), std::get<2>(p)));
        /*
          std::string rep = bepaald::bytesToString(std::get<1>(p), std::get<2>(p));
          std::string::size_type pos2 = 0;
          while ((pos2 = rep.find('\'', pos2)) != std::string::npos)
          {
          rep.replace(pos2, 1, "''");
          pos2 += 2;
          }
          rep = '\'' + rep + '\'';

          d_statement.replace(pos, 1, rep);
          pos += rep.length();
        */
        break;
      }
    case PARAMETER_FIELD::DOUBLE:
      {
        parameters.emplace_back(*reinterpret_cast<double *>(std::get<1>(p)));
        break;
      }
    case PARAMETER_FIELD::BLOB:
      {
        std::pair<std::shared_ptr<unsigned char []>, size_t> data{new unsigned char[std::get<2>(p)], std::get<2>(p)};
        std::memcpy(data.first.get(), std::get<1>(p), std::get<2>(p));
        parameters.emplace_back(std::move(data));
        break;
      }
    case PARAMETER_FIELD::NULLPARAMETER:
      {
        //parameters.emplace_back(static_cast<long long int>(bytesToUint64(std::get<1>(p), std::get<2>(p))));
        parameters.emplace_back(nullptr);
        break;
      }
    }
  }
  return parameters;
}

inline bool SqlStatementFrame::validate() const
{
  if (d_framedata.empty())
    return false;

  bool foundstatement = false;
  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) != FIELD::STATEMENT &&
        std::get<0>(p) != FIELD::PARAMETERS)
      return false;

    if (std::get<0>(p) == FIELD::STATEMENT)
      foundstatement = true;
  }

  for (auto const &pd : d_parameterdata)
  {
    if (std::get<0>(pd) != PARAMETER_FIELD::STRING &&
        std::get<0>(pd) != PARAMETER_FIELD::INT &&
        std::get<0>(pd) != PARAMETER_FIELD::DOUBLE &&
        std::get<0>(pd) != PARAMETER_FIELD::BLOB &&
        std::get<0>(pd) != PARAMETER_FIELD::NULLPARAMETER)
      return false;
  }

  return foundstatement;
}

#endif
