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

#include "oatpp-mongocxxmapper/ObjectMapper.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

#include "oatpp/core/base/Environment.hpp"

#include <iostream>
#include <string>
#include <algorithm>
#include <bsoncxx/json.hpp>

namespace {

class Test : public oatpp::test::UnitTest {
public:
  Test() : oatpp::test::UnitTest("MyTag")
  {}

  std::shared_ptr<oatpp::parser::json::mapping::ObjectMapper> jsonmapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
  std::shared_ptr<oatpp::parser::mongocxx::mapping::ObjectMapper> mongomapper = oatpp::parser::mongocxx::mapping::ObjectMapper::createShared();

  static const unsigned ITERATIONS = 10000;

  static oatpp::test::mongocxxmapper::TestDto::ObjectWrapper makeTestDto() {
    auto dto = oatpp::test::mongocxxmapper::TestDto::createShared();
    dto->testValue = "FooBar";
    dto->testList = dto->testList->createShared();
    dto->testList->pushBack("1");
    dto->testList->pushBack("2");
    dto->testList->pushBack("3");
    dto->testMap = dto->testMap->createShared();
    dto->testMap->put("Foo", "Bar");

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

    auto dto = makeTestDto();

    { // speedtest of mapper
      long long unsigned serialization = 0;
      long long unsigned deserialization = 0;
      for (int i = 0; i < ITERATIONS; ++i) {
        auto serializeStart = std::chrono::system_clock::now();
        auto mongo = mongomapper->writeAsDocument(dto);
        auto serializeEnd = std::chrono::system_clock::now();

        std::string omstr = jsonmapper->writeToString(dto)->std_str();
        omstr.erase(std::remove(omstr.begin(), omstr.end(), ' '), omstr.end());
        std::string bsonstr = bsoncxx::to_json(mongo.view());
        bsonstr.erase(std::remove(bsonstr.begin(), bsonstr.end(), ' '), bsonstr.end());

        if (i == 0) {
          OATPP_LOGV(TAG, "OATPP   JSON: %s", omstr.c_str());
          OATPP_LOGV(TAG, "BSONCXX JSON: %s", bsonstr.c_str());
          OATPP_ASSERT(omstr == bsonstr);
        }

        auto deserializeStart = std::chrono::system_clock::now();
        auto desdto = mongomapper->readFromDocument<oatpp::test::mongocxxmapper::TestDto>(mongo);
        auto deserializeEnd = std::chrono::system_clock::now();

        std::string deomstr = jsonmapper->writeToString(desdto)->std_str();
        deomstr.erase(std::remove(deomstr.begin(), deomstr.end(), ' '), deomstr.end());
        if (i == 0) {
          OATPP_LOGV(TAG, "DESDTO  JSON: %s", deomstr.c_str());
          OATPP_ASSERT(omstr == deomstr);
        }
        serialization += std::chrono::duration_cast<std::chrono::nanoseconds>(serializeEnd - serializeStart).count();
        deserialization +=
            std::chrono::duration_cast<std::chrono::nanoseconds>(deserializeEnd - deserializeStart).count();
      }
      OATPP_LOGV(TAG,
                 "Mapper-Timings: %u DTO-to-Mongo %llums, %u Mongo-to-DTO %llums",
                 ITERATIONS, serialization / 1000000,
                 ITERATIONS, deserialization / 100000)
    }

    { // speedtest of basic string mapping
      long long unsigned serialization = 0;
      long long unsigned deserialization = 0;
      for(int i = 0; i < ITERATIONS; ++i) {
        auto serializationStart = std::chrono::system_clock::now();
        std::string serjson = jsonmapper->writeToString(dto)->std_str();
        auto serview = bsoncxx::from_json(bsoncxx::stdx::string_view(serjson));
        auto serializationEnd = std::chrono::system_clock::now();
        std::string desjson = bsoncxx::to_json(serview);
        auto desdto = jsonmapper->readFromString<oatpp::test::mongocxxmapper::TestDto>(desjson.c_str());
        auto deserializationEnd = std::chrono::system_clock::now();
        serialization += std::chrono::duration_cast<std::chrono::nanoseconds>(serializationEnd - serializationStart).count();
        deserialization +=
            std::chrono::duration_cast<std::chrono::nanoseconds>(deserializationEnd - serializationEnd).count();
      }
      OATPP_LOGV(TAG,
                 "String-Parsing-Timings: %u DTO-to-Mongo %llums, %u Mongo-to-DTO %llums",
                 ITERATIONS, serialization / 1000000,
                 ITERATIONS, deserialization / 100000)
    }
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
