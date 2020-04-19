/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *                         Benedikt-Alexander Mokroß <bam@icognize.de>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************************/

#include "TestUtils.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/data/stream/BufferStream.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"

#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>

#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/types.hpp>

#include <iostream>

namespace oatpp { namespace mongo { namespace test {

void TestUtils::writeBinary(const void *voiddata, v_int32 length, const char* tag) {
  p_char8 data = (p_char8) voiddata;

  auto lenStrLen = oatpp::utils::conversion::int32ToStr(length)->getSize();

  oatpp::data::stream::BufferOutputStream stream;
  stream << "\nbegin:----------------------------------------" << tag << "\n";

  for (v_int32 i = 0; i < length; i++) {

    v_char8 a = data[i];

    auto indexStr = oatpp::utils::conversion::int32ToStr(i);

    for (int i = 0; i < lenStrLen - indexStr->getSize(); i++) {
      stream.writeCharSimple(' ');
    }

    auto str = oatpp::utils::conversion::int32ToStr(a);
    stream << indexStr << "] ";
    stream << oatpp::utils::conversion::primitiveToStr((v_uint8)a, " %02X  ");
    stream << str;

    if (a >= ' ' && a <= 'z') {
      for (int i = 0; i < 5 - str->getSize(); i++) {
        stream.writeCharSimple(' ');
      }
      stream << "'" << oatpp::String((const char *) (&data[i]), 1, false) << "'";
    }
    stream << "\n";
  }

  stream << "end-------------------------------------------" << tag << "\n";
  std::cout << stream.toString()->c_str() << std::endl;
}

void TestUtils::writeBinary(const oatpp::String &data, const char* tag) {
  writeBinary(data->getData(), data->getSize(), tag);
}

void TestUtils::hardcodeBinary(const oatpp::String& data, const oatpp::String& varname) {

  oatpp::data::stream::BufferOutputStream stream;
  stream << "std::vector<v_uint8> " << varname << " = {";

  for(v_int32 i = 0; i < data->getSize(); i++) {
    v_char8 a = data->getData()[i];
    stream << (v_uint8) a;
    if(i < data->getSize() - 1) {
      stream << ", ";
    }
  }

  stream << "}";

  OATPP_LOGD("HARDCODE", "%s", stream.toString()->getData());

}

oatpp::String TestUtils::writeJsonToBsonCXX(const AbstractObjectWrapper& polymorph) {

  oatpp::parser::json::mapping::ObjectMapper jsonMapper;
  oatpp::String jsonStr = jsonMapper.writeToString(polymorph);

  auto strv = bsoncxx::stdx::string_view(jsonStr->c_str());
  bsoncxx::document::view view = bsoncxx::from_json(strv);

  return oatpp::String((const char*)view.data(), view.length(), true);

}

}}}