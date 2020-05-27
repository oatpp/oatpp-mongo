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

#include "Int16Test.hpp"

#include "oatpp-mongo/TestUtils.hpp"
#include "oatpp-mongo/bson/mapping/ObjectMapper.hpp"

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

namespace oatpp { namespace mongo { namespace test { namespace bson {

namespace {

#include OATPP_CODEGEN_BEGIN(DTO)

/* Complete object */
class Obj : public oatpp::DTO {

  DTO_INIT(Obj, DTO)

  DTO_FIELD(Int16, f1) = std::numeric_limits<v_int16>::max();
  DTO_FIELD(Int16, f2) = 16;
  DTO_FIELD(Int16, f3) = nullptr;
  DTO_FIELD(Int16, f4) = std::numeric_limits<v_int16>::min();

};

/* No first field */
class Sub1 : public oatpp::DTO {

  DTO_INIT(Sub1, DTO)

  DTO_FIELD(Int16, f2);
  DTO_FIELD(Int16, f3);
  DTO_FIELD(Int16, f4);

};

/* No second field */
class Sub2 : public oatpp::DTO {

  DTO_INIT(Sub2, DTO)

  DTO_FIELD(Int16, f1);
  DTO_FIELD(Int16, f3);
  DTO_FIELD(Int16, f4);

};

/* No null field */
class Sub3 : public oatpp::DTO {

  DTO_INIT(Sub3, DTO)

  DTO_FIELD(Int16, f1);
  DTO_FIELD(Int16, f2);
  DTO_FIELD(Int16, f4);

};

/* No last field */
class Sub4 : public oatpp::DTO {

  DTO_INIT(Sub4, DTO)

  DTO_FIELD(Int16, f1);
  DTO_FIELD(Int16, f2);
  DTO_FIELD(Int16, f3);

};

////////////////////////////////////////
// Unsigned

/* Complete object */
class UObj : public oatpp::DTO {

  DTO_INIT(UObj, DTO)

  DTO_FIELD(UInt16, f1) = std::numeric_limits<v_uint16>::max();
  DTO_FIELD(UInt16, f2) = 16;
  DTO_FIELD(UInt16, f3) = nullptr;
  DTO_FIELD(UInt16, f4) = std::numeric_limits<v_uint16>::min();

};

/* No first field */
class USub1 : public oatpp::DTO {

  DTO_INIT(USub1, DTO)

  DTO_FIELD(UInt16, f2);
  DTO_FIELD(UInt16, f3);
  DTO_FIELD(UInt16, f4);

};

/* No second field */
class USub2 : public oatpp::DTO {

  DTO_INIT(USub2, DTO)

  DTO_FIELD(UInt16, f1);
  DTO_FIELD(UInt16, f3);
  DTO_FIELD(UInt16, f4);

};

/* No null field */
class USub3 : public oatpp::DTO {

  DTO_INIT(USub3, DTO)

  DTO_FIELD(UInt16, f1);
  DTO_FIELD(UInt16, f2);
  DTO_FIELD(UInt16, f4);

};

/* No last field */
class USub4 : public oatpp::DTO {

  DTO_INIT(USub4, DTO)

  DTO_FIELD(UInt16, f1);
  DTO_FIELD(UInt16, f2);
  DTO_FIELD(UInt16, f3);

};

#include OATPP_CODEGEN_END(DTO)

}

void Int16Test::onRun() {

  oatpp::mongo::bson::mapping::ObjectMapper bsonMapper;

  /////////////////////////////////////////////////////
  // Signed

  {

    auto obj = Obj::createShared();

    auto bson = bsonMapper.writeToString(obj);
    auto bcxx = TestUtils::writeJsonToBsonCXX(obj);

    if (bson != bcxx) {
      TestUtils::writeBinary(bson, "oatpp-mongo::bson (signed)");
      TestUtils::writeBinary(bcxx, "bsoncxx (signed)");
    }

    OATPP_ASSERT(bson == bcxx);

    {
      OATPP_LOGI(TAG, "sub0...");
      auto sub = bsonMapper.readFromString<oatpp::Object<Obj>>(bson);

      OATPP_ASSERT(sub->f1 == obj->f1);
      OATPP_ASSERT(sub->f2 == obj->f2);
      OATPP_ASSERT(!sub->f3.getPtr() && !obj->f3.getPtr());
      OATPP_ASSERT(sub->f4 == obj->f4);

      OATPP_LOGI(TAG, "sub0 - OK");
    }

    {
      OATPP_LOGI(TAG, "sub1...");
      auto sub = bsonMapper.readFromString<oatpp::Object<Sub1>>(bson);

      OATPP_ASSERT(sub->f2 == obj->f2);
      OATPP_ASSERT(!sub->f3.getPtr() && !obj->f3.getPtr());
      OATPP_ASSERT(sub->f4 == obj->f4);

      OATPP_LOGI(TAG, "sub1 - OK");
    }

    {
      OATPP_LOGI(TAG, "sub2...");
      auto sub = bsonMapper.readFromString<oatpp::Object<Sub2>>(bson);

      OATPP_ASSERT(sub->f1 == obj->f1);
      OATPP_ASSERT(!sub->f3.getPtr() && !obj->f3.getPtr());
      OATPP_ASSERT(sub->f4 == obj->f4);

      OATPP_LOGI(TAG, "sub2 - OK");
    }

    {
      OATPP_LOGI(TAG, "sub3...");
      auto sub = bsonMapper.readFromString<oatpp::Object<Sub3>>(bson);

      OATPP_ASSERT(sub->f1 == obj->f1);
      OATPP_ASSERT(sub->f2 == obj->f2);
      OATPP_ASSERT(sub->f4 == obj->f4);

      OATPP_LOGI(TAG, "sub3 - OK");
    }

    {
      OATPP_LOGI(TAG, "sub4...");
      auto sub = bsonMapper.readFromString<oatpp::Object<Sub4>>(bson);

      OATPP_ASSERT(sub->f1 == obj->f1);
      OATPP_ASSERT(sub->f2 == obj->f2);
      OATPP_ASSERT(!sub->f3.getPtr() && !obj->f3.getPtr());

      OATPP_LOGI(TAG, "sub4 - OK");
    }

  }

  /////////////////////////////////////////////////////
  // Unsigned

  {

    auto obj = UObj::createShared();

    auto bson = bsonMapper.writeToString(obj);
    auto bcxx = TestUtils::writeJsonToBsonCXX(obj);

    if (bson != bcxx) {
      TestUtils::writeBinary(bson, "oatpp-mongo::bson (unsigned)");
      TestUtils::writeBinary(bcxx, "bsoncxx (unsigned)");
    }

    OATPP_ASSERT(bson == bcxx);

    {
      OATPP_LOGI(TAG, "usub0...");
      auto sub = bsonMapper.readFromString<oatpp::Object<UObj>>(bson);

      OATPP_ASSERT(sub->f1 == obj->f1);
      OATPP_ASSERT(sub->f2 == obj->f2);
      OATPP_ASSERT(!sub->f3.getPtr() && !obj->f3.getPtr());
      OATPP_ASSERT(sub->f4 == obj->f4);

      OATPP_LOGI(TAG, "usub0 - OK");
    }

    {
      OATPP_LOGI(TAG, "usub1...");
      auto sub = bsonMapper.readFromString<oatpp::Object<USub1>>(bson);

      OATPP_ASSERT(sub->f2 == obj->f2);
      OATPP_ASSERT(!sub->f3.getPtr() && !obj->f3.getPtr());
      OATPP_ASSERT(sub->f4 == obj->f4);

      OATPP_LOGI(TAG, "usub1 - OK");
    }

    {
      OATPP_LOGI(TAG, "usub2...");
      auto sub = bsonMapper.readFromString<oatpp::Object<USub2>>(bson);

      OATPP_ASSERT(sub->f1 == obj->f1);
      OATPP_ASSERT(!sub->f3.getPtr() && !obj->f3.getPtr());
      OATPP_ASSERT(sub->f4 == obj->f4);

      OATPP_LOGI(TAG, "usub2 - OK");
    }

    {
      OATPP_LOGI(TAG, "usub3...");
      auto sub = bsonMapper.readFromString<oatpp::Object<USub3>>(bson);

      OATPP_ASSERT(sub->f1 == obj->f1);
      OATPP_ASSERT(sub->f2 == obj->f2);
      OATPP_ASSERT(sub->f4 == obj->f4);

      OATPP_LOGI(TAG, "usub3 - OK");
    }

    {
      OATPP_LOGI(TAG, "usub4...");
      auto sub = bsonMapper.readFromString<oatpp::Object<USub4>>(bson);

      OATPP_ASSERT(sub->f1 == obj->f1);
      OATPP_ASSERT(sub->f2 == obj->f2);
      OATPP_ASSERT(!sub->f3.getPtr() && !obj->f3.getPtr());

      OATPP_LOGI(TAG, "usub4 - OK");
    }

  }

}

}}}}