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

#include "FloatTest.hpp"

#include "oatpp-mongo/TestUtils.hpp"
#include "oatpp-mongo/bson/mapping/ObjectMapper.hpp"

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include <vector>

namespace oatpp { namespace mongo { namespace test { namespace bson {

namespace {

#include OATPP_CODEGEN_BEGIN(DTO)

/* Complete object */
class F32_Obj : public oatpp::DTO {

DTO_INIT(F32_Obj, DTO)

  DTO_FIELD(Float32, f1) = 3.14159265359;
  DTO_FIELD(Float32, f2) = 3.14159265359 / 2.0;
  DTO_FIELD(Float32, f3) = nullptr;
  DTO_FIELD(Float32, f4) = -3.14159265359;

};

/* No first field */
class F32_Sub1 : public oatpp::DTO {

DTO_INIT(F32_Sub1, DTO)

  DTO_FIELD(Float32, f2);
  DTO_FIELD(Float32, f3);
  DTO_FIELD(Float32, f4);

};

/* No second field */
class F32_Sub2 : public oatpp::DTO {

DTO_INIT(F32_Sub2, DTO)

  DTO_FIELD(Float32, f1);
  DTO_FIELD(Float32, f3);
  DTO_FIELD(Float32, f4);

};

/* No null field */
class F32_Sub3 : public oatpp::DTO {

DTO_INIT(F32_Sub3, DTO)

  DTO_FIELD(Float32, f1);
  DTO_FIELD(Float32, f2);
  DTO_FIELD(Float32, f4);

};

/* No last field */
class F32_Sub4 : public oatpp::DTO {

DTO_INIT(F32_Sub4, DTO)

  DTO_FIELD(Float32, f1);
  DTO_FIELD(Float32, f2);
  DTO_FIELD(Float32, f3);

};

////////////////////////////////////////
// Float64

/* Complete object */
class F64_Obj : public oatpp::DTO {

DTO_INIT(F64_Obj, DTO)

  DTO_FIELD(Float64, f1) = 3.14159265359;
  DTO_FIELD(Float64, f2) = 3.14159265359 / 2.0;
  DTO_FIELD(Float64, f3) = nullptr;
  DTO_FIELD(Float64, f4) = -3.14159265359;

};

/* No first field */
class F64_Sub1 : public oatpp::DTO {

DTO_INIT(F64_Sub1, DTO)

  DTO_FIELD(Float64, f2);
  DTO_FIELD(Float64, f3);
  DTO_FIELD(Float64, f4);

};

/* No second field */
class F64_Sub2 : public oatpp::DTO {

DTO_INIT(F64_Sub2, DTO)

  DTO_FIELD(Float64, f1);
  DTO_FIELD(Float64, f3);
  DTO_FIELD(Float64, f4);

};

/* No null field */
class F64_Sub3 : public oatpp::DTO {

DTO_INIT(F64_Sub3, DTO)

  DTO_FIELD(Float64, f1);
  DTO_FIELD(Float64, f2);
  DTO_FIELD(Float64, f4);

};

/* No last field */
class F64_Sub4 : public oatpp::DTO {

DTO_INIT(F64_Sub4, DTO)

  DTO_FIELD(Float64, f1);
  DTO_FIELD(Float64, f2);
  DTO_FIELD(Float64, f3);

};

#include OATPP_CODEGEN_END(DTO)

bool readValues(const oatpp::String &binary, v_float64 &f1, v_float64 &f2, v_float64 &f4) {

  if (binary->getSize() == 45) {

    oatpp::parser::Caret caret(binary);

    caret.setPosition(8);
    f1 = oatpp::mongo::bson::Utils::readFloat64(caret);

    caret.setPosition(20);
    f2 = oatpp::mongo::bson::Utils::readFloat64(caret);

    caret.setPosition(36);
    f4 = oatpp::mongo::bson::Utils::readFloat64(caret);

    return true;

  }

  return false;

}

bool almostEqual(v_float64 a, v_float64 b, v_float64 d = 0.001) {
  return (a + d) >= b && (a - d) <= b;
}

}

void FloatTest::onRun() {

  oatpp::mongo::bson::mapping::ObjectMapper bsonMapper;

  /////////////////////////////////////////////////////
  // Float32

  {

    auto obj = F32_Obj::createShared();

    auto bson = bsonMapper.writeToString(obj);
    auto bcxx = TestUtils::writeJsonToBsonCXX(obj);

    v_float64 bson_f1;
    v_float64 bson_f2;
    v_float64 bson_f4;

    bool bsonValuesOK = readValues(bson, bson_f1, bson_f2, bson_f4);

    v_float64 bcxx_f1;
    v_float64 bcxx_f2;
    v_float64 bcxx_f4;

    bool bcxxValuesOK = readValues(bcxx, bcxx_f1, bcxx_f2, bcxx_f4);

    bool checkEquals =
      almostEqual(bson_f1, bcxx_f1) &&
      almostEqual(bson_f2, bcxx_f2) &&
      almostEqual(bson_f4, bcxx_f4);

    if (!bsonValuesOK || !bcxxValuesOK || !checkEquals) {
      TestUtils::writeBinary(bson, "oatpp-mongo::bson (Float32)");
      TestUtils::writeBinary(bcxx, "bsoncxx (Float32)");
    }

    OATPP_ASSERT(bsonValuesOK && bcxxValuesOK);
    OATPP_ASSERT(checkEquals);

    {
      OATPP_LOGI(TAG, "f32 sub0...");
      auto sub = bsonMapper.readFromString<oatpp::Object<F32_Obj>>(bson);

      OATPP_ASSERT(sub->f1 == obj->f1);
      OATPP_ASSERT(sub->f2 == obj->f2);
      OATPP_ASSERT(!sub->f3.getPtr() && !obj->f3.getPtr());
      OATPP_ASSERT(sub->f4 == obj->f4);

      OATPP_LOGI(TAG, "f32 sub0 - OK");
    }

    {
      OATPP_LOGI(TAG, "f32 sub1...");
      auto sub = bsonMapper.readFromString<oatpp::Object<F32_Sub1>>(bson);

      OATPP_ASSERT(sub->f2 == obj->f2);
      OATPP_ASSERT(!sub->f3.getPtr() && !obj->f3.getPtr());
      OATPP_ASSERT(sub->f4 == obj->f4);

      OATPP_LOGI(TAG, "f32 sub1 - OK");
    }

    {
      OATPP_LOGI(TAG, "f32 sub2...");
      auto sub = bsonMapper.readFromString<oatpp::Object<F32_Sub2>>(bson);

      OATPP_ASSERT(sub->f1 == obj->f1);
      OATPP_ASSERT(!sub->f3.getPtr() && !obj->f3.getPtr());
      OATPP_ASSERT(sub->f4 == obj->f4);

      OATPP_LOGI(TAG, "f32 sub2 - OK");
    }

    {
      OATPP_LOGI(TAG, "f32 sub3...");
      auto sub = bsonMapper.readFromString<oatpp::Object<F32_Sub3>>(bson);

      OATPP_ASSERT(sub->f1 == obj->f1);
      OATPP_ASSERT(sub->f2 == obj->f2);
      OATPP_ASSERT(sub->f4 == obj->f4);

      OATPP_LOGI(TAG, "f32 sub3 - OK");
    }

    {
      OATPP_LOGI(TAG, "f32 sub4...");
      auto sub = bsonMapper.readFromString<oatpp::Object<F32_Sub4>>(bson);

      OATPP_ASSERT(sub->f1 == obj->f1);
      OATPP_ASSERT(sub->f2 == obj->f2);
      OATPP_ASSERT(!sub->f3.getPtr() && !obj->f3.getPtr());

      OATPP_LOGI(TAG, "f32 sub4 - OK");
    }

  }

  /////////////////////////////////////////////////////
  // Float64

  {

    auto obj = F64_Obj::createShared();

    auto bson = bsonMapper.writeToString(obj);
    auto bcxx = TestUtils::writeJsonToBsonCXX(obj);

    v_float64 bson_f1;
    v_float64 bson_f2;
    v_float64 bson_f4;

    bool bsonValuesOK = readValues(bson, bson_f1, bson_f2, bson_f4);

    v_float64 bcxx_f1;
    v_float64 bcxx_f2;
    v_float64 bcxx_f4;

    bool bcxxValuesOK = readValues(bcxx, bcxx_f1, bcxx_f2, bcxx_f4);

    bool checkEquals =
      almostEqual(bson_f1, bcxx_f1) &&
      almostEqual(bson_f2, bcxx_f2) &&
      almostEqual(bson_f4, bcxx_f4);

    if (!bsonValuesOK || !bcxxValuesOK || !checkEquals) {
      TestUtils::writeBinary(bson, "oatpp-mongo::bson (Float64)");
      TestUtils::writeBinary(bcxx, "bsoncxx (Float64)");
    }

    OATPP_ASSERT(bsonValuesOK && bcxxValuesOK);
    OATPP_ASSERT(checkEquals);

    {
      OATPP_LOGI(TAG, " F64 sub0...");
      auto sub = bsonMapper.readFromString<oatpp::Object<F64_Obj>>(bson);

      OATPP_ASSERT(sub->f1 == obj->f1);
      OATPP_ASSERT(sub->f2 == obj->f2);
      OATPP_ASSERT(!sub->f3.getPtr() && !obj->f3.getPtr());
      OATPP_ASSERT(sub->f4 == obj->f4);

      OATPP_LOGI(TAG, " F64 sub0 - OK");
    }

    {
      OATPP_LOGI(TAG, " F64 sub1...");
      auto sub = bsonMapper.readFromString<oatpp::Object<F64_Sub1>>(bson);

      OATPP_ASSERT(sub->f2 == obj->f2);
      OATPP_ASSERT(!sub->f3.getPtr() && !obj->f3.getPtr());
      OATPP_ASSERT(sub->f4 == obj->f4);

      OATPP_LOGI(TAG, " F64 sub1 - OK");
    }

    {
      OATPP_LOGI(TAG, " F64 sub2...");
      auto sub = bsonMapper.readFromString<oatpp::Object<F64_Sub2>>(bson);

      OATPP_ASSERT(sub->f1 == obj->f1);
      OATPP_ASSERT(!sub->f3.getPtr() && !obj->f3.getPtr());
      OATPP_ASSERT(sub->f4 == obj->f4);

      OATPP_LOGI(TAG, " F64 sub2 - OK");
    }

    {
      OATPP_LOGI(TAG, " F64 sub3...");
      auto sub = bsonMapper.readFromString<oatpp::Object<F64_Sub3>>(bson);

      OATPP_ASSERT(sub->f1 == obj->f1);
      OATPP_ASSERT(sub->f2 == obj->f2);
      OATPP_ASSERT(sub->f4 == obj->f4);

      OATPP_LOGI(TAG, " F64 sub3 - OK");
    }

    {
      OATPP_LOGI(TAG, " F64 sub4...");
      auto sub = bsonMapper.readFromString<oatpp::Object<F64_Sub4>>(bson);

      OATPP_ASSERT(sub->f1 == obj->f1);
      OATPP_ASSERT(sub->f2 == obj->f2);
      OATPP_ASSERT(!sub->f3.getPtr() && !obj->f3.getPtr());

      OATPP_LOGI(TAG, " F64 sub4 - OK");
    }

  }

}

}}}}