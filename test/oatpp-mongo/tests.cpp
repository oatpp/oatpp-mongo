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

#include "oatpp-mongo/bson/StringTest.hpp"

#include "oatpp-mongo/bson/Int8Test.hpp"
#include "oatpp-mongo/bson/Int16Test.hpp"
#include "oatpp-mongo/bson/Int32Test.hpp"
#include "oatpp-mongo/bson/Int64Test.hpp"
#include "oatpp-mongo/bson/FloatTest.hpp"
#include "oatpp-mongo/bson/BooleanTest.hpp"

#include "oatpp-mongo/bson/ListTest.hpp"
#include "oatpp-mongo/bson/MapTest.hpp"
#include "oatpp-mongo/bson/ObjectTest.hpp"

#include "oatpp-test/UnitTest.hpp"

#include <iostream>

namespace {

void runTests() {

  OATPP_RUN_TEST(oatpp::mongo::test::bson::StringTest);

  OATPP_RUN_TEST(oatpp::mongo::test::bson::Int8Test);
  OATPP_RUN_TEST(oatpp::mongo::test::bson::Int16Test);
  OATPP_RUN_TEST(oatpp::mongo::test::bson::Int32Test);
  OATPP_RUN_TEST(oatpp::mongo::test::bson::Int64Test);
  OATPP_RUN_TEST(oatpp::mongo::test::bson::FloatTest);
  OATPP_RUN_TEST(oatpp::mongo::test::bson::BooleanTest);

  OATPP_RUN_TEST(oatpp::mongo::test::bson::ListTest);
  OATPP_RUN_TEST(oatpp::mongo::test::bson::MapTest);
  OATPP_RUN_TEST(oatpp::mongo::test::bson::ObjectTest);

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
