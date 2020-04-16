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


#include "oatpp-test/UnitTest.hpp"

#include "DTO.hpp"

#include "oatpp-mongo/bson/mapping/ObjectMapper.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

#include "oatpp/core/data/stream/BufferStream.hpp"

#include "oatpp/core/base/Environment.hpp"

#include <iostream>
#include <string>
#include <algorithm>

#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>


#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/types.hpp>

namespace {

#include OATPP_CODEGEN_BEGIN(DTO)

class Test1 : public oatpp::Object {

  DTO_INIT(Test1, Object)

  DTO_FIELD(String, str) = "hello!";

//  DTO_FIELD(Int8, i8) = std::numeric_limits<v_int8>::max();
//  DTO_FIELD(UInt8, u8) = std::numeric_limits<v_uint8>::max();
//
//  DTO_FIELD(Int16, i16) = std::numeric_limits<v_int16>::max();
//  DTO_FIELD(UInt16, u16) = std::numeric_limits<v_uint16>::max();
//
//  DTO_FIELD(Int32, i32) = std::numeric_limits<v_int32>::max();
//  DTO_FIELD(UInt32, u32) = std::numeric_limits<v_uint32>::max();
//
//  DTO_FIELD(Int64, i64) = std::numeric_limits<v_int64>::max();

//  DTO_FIELD(Float32, f32) = std::numeric_limits<v_float32>::max();
//  DTO_FIELD(Float64, f64) = std::numeric_limits<v_float64>::max();

//  DTO_FIELD(Boolean, b1) = true;
//  DTO_FIELD(Boolean, b0) = false;


//  DTO_FIELD(List<Int8>::ObjectWrapper, list_i8) = List<Int8>::createShared();
//  DTO_FIELD(List<UInt8>::ObjectWrapper, list_u8) = List<UInt8>::createShared();
//
//  DTO_FIELD(List<Int16>::ObjectWrapper, list_i16) = List<Int16>::createShared();
//  DTO_FIELD(List<UInt16>::ObjectWrapper, list_u16) = List<UInt16>::createShared();
//
//  DTO_FIELD(List<Int32>::ObjectWrapper, list_i32) = List<Int32>::createShared();
//  DTO_FIELD(List<UInt32>::ObjectWrapper, list_u32) = List<UInt32>::createShared();
//
//  DTO_FIELD(List<Int64>::ObjectWrapper, list_i64) = List<Int64>::createShared();
//
//  DTO_FIELD(List<Boolean>::ObjectWrapper, list_b) = List<Boolean>::createShared();

//  DTO_FIELD(List<Float32>::ObjectWrapper, list_f32) = List<Float32>::createShared();
//  DTO_FIELD(List<Float64>::ObjectWrapper, list_f64) = List<Float64>::createShared();

};

#include OATPP_CODEGEN_END(DTO)

class Test : public oatpp::test::UnitTest {
public:
  Test() : oatpp::test::UnitTest("MyTag")
  {}

  void writeBin(const void* voiddata, v_int32 length) {
    p_char8 data = (p_char8)voiddata;
    oatpp::data::stream::BufferOutputStream stream;
    stream << "\nbegin:\n";
    for(v_int32 i = 0; i < length; i ++) {

      v_char8 a = data[i];
      if(a >= ' ' && a <= 'z') {
        stream << "'" << oatpp::String((const char*) (&data[i]), 1, false) << "'";
      } else {
        stream << (v_uint8) a;
      }
      stream << "\n";
    }
    OATPP_LOGD("AAA", "='%s'\n", stream.toString()->getData());
  }

  void writeBin(const oatpp::String& data) {
    writeBin(data->getData(), data->getSize());
  }

  void binDouble(double value) {
    constexpr v_uint64 one = 1;
    v_uint64 word = *((p_uint64) &value);
    oatpp::data::stream::BufferOutputStream stream;
    for(v_uint64 i = 0; i < 64; i ++) {

      v_uint64 bit = (one << i) & word;
      if(bit > 0) {
        stream << "1";
      } else {
        stream << "0";
      }

      if((i + 1) % 8 == 0) {
        stream << ",";
      }

    }
    OATPP_LOGD("DBL", "'%s' = %f", stream.toString()->getData(), value);
  }

  static oatpp::test::mongocxxmapper::TestDto::ObjectWrapper makeTestDto() {
    auto dto = oatpp::test::mongocxxmapper::TestDto::createShared();
    dto->testValue = "FooBar";
    dto->testList = dto->testList->createShared();
    dto->testList->pushBack("1");
    dto->testList->pushBack("2");
    dto->testList->pushBack("3");
    dto->testMap = dto->testMap->createShared();
    dto->testMap->put("Foo", "Bar");

    dto->i8Value = INT8_MAX;
    dto->i16Value = INT16_MAX;
    dto->i32Value = INT32_MAX;
    dto->i64Value = INT64_MAX;
    dto->boolValue = false;

    dto->subObject = dto->subObject->createShared();
    dto->subObject->sub = "sup?";

    dto->testMapList = dto->testMapList->createShared();
    auto map = dto->testMap->createShared();
    map->put("hurr","durr");
    dto->testMapMap = dto->testMapMap->createShared();
    dto->testMapMap->put("test", map);
    dto->testMapList->pushBack(map);
    dto->testMapList->pushBack(dto->testMap);


    return dto;
  }

  void onRun() override {

    oatpp::parser::json::mapping::ObjectMapper jsonMapper;
    oatpp::mongo::bson::mapping::ObjectMapper bsonMapper;

    auto obj = Test1::createShared();
//    obj->list_i8->pushBack(8);
//    obj->list_u8->pushBack(8);
//    obj->list_i16->pushBack(1024);
//    obj->list_u16->pushBack(1024);
//    obj->list_i32->pushBack(2000000000);
//    obj->list_u32->pushBack(4000000000);
//    obj->list_b->pushBack(true);
//    obj->list_b->pushBack(false);

//    obj->list_f32->pushBack(0.32);
//    obj->list_f64->pushBack(0.64);

    auto json = jsonMapper.writeToString(obj);
    auto bson = bsonMapper.writeToString(obj);

    auto obj2 = bsonMapper.readFromString<Test1>(bson);

    OATPP_LOGD("json", "'%s'", json->getData());

    auto strv = bsoncxx::stdx::string_view(json->c_str());
    bsoncxx::document::view view = bsoncxx::from_json(strv);

    auto original = oatpp::String((const char*)view.data(), view.length(), true);

    writeBin(original);
    writeBin(bson);

    if(original != bson) {
      OATPP_LOGE("json", "ERROR");
    }

    binDouble(2.0);
    binDouble(0);

    v_char8 t[8] = {0, 0, 0, 224, 255, 255, 239, 'G'};
    p_float64 r = (p_float64)&t[0];

    OATPP_LOGD("A", "R=%g", *r);

  }
};

void runTests() {
  OATPP_RUN_TEST(Test);
}

}

int main() {

  oatpp::base::Environment::init();

  runTests();

  /* Print how much objects were created during app running, and what have left-probably leaked */
  /* Disable object counting for release builds using '-D OATPP_DISABLE_ENV_OBJECT_COUNTERS' flag for better performance */
  std::cout << "\nEnvironment:\n";
  std::cout << "objectsCount = " << oatpp::base::Environment::getObjectsCount() << "\n";
  std::cout << "objectsCreated = " << oatpp::base::Environment::getObjectsCreated() << "\n\n";

  OATPP_ASSERT(oatpp::base::Environment::getObjectsCount() == 0);

  oatpp::base::Environment::destroy();

  return 0;
}
