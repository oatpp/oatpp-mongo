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

#include "ObjectTest.hpp"

#include "oatpp-mongo/TestUtils.hpp"
#include "oatpp-mongo/bson/mapping/ObjectMapper.hpp"

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

namespace oatpp { namespace mongo { namespace test { namespace bson {

namespace {

#include OATPP_CODEGEN_BEGIN(DTO)

class Nested1 : public oatpp::DTO {

  DTO_INIT(Nested1, DTO)

  DTO_FIELD(String, f1) = nullptr;
  DTO_FIELD(String, f2) = "Hello";
  DTO_FIELD(String, f3) = "World!!!";

public:

  static bool cmp(const oatpp::Object<Nested1>& a, const oatpp::Object<Nested1>& b) {
    return  a->f1 == b->f1 &&
            a->f2 == b->f2 &&
            a->f3 == b->f3;
  }

};

class Nested2 : public oatpp::DTO {

  DTO_INIT(Nested2, DTO)

  DTO_FIELD(String, f1) = "Oat++";
  DTO_FIELD(String, f2) = nullptr;
  DTO_FIELD(String, f3) = "BSON Ser/De";

public:

  static bool cmp(const oatpp::Object<Nested2>& a, const oatpp::Object<Nested2>& b) {
    return  a->f1 == b->f1 &&
            a->f2 == b->f2 &&
            a->f3 == b->f3;
  }

};

class Nested3 : public oatpp::DTO {

  DTO_INIT(Nested3, DTO)

  DTO_FIELD(String, f1);
  DTO_FIELD(String, f2);
  DTO_FIELD(String, f3);

public:

  static bool cmp(const oatpp::Object<Nested3>& a, const oatpp::Object<Nested3>& b) {
    return  a->f1 == b->f1 &&
            a->f2 == b->f2 &&
            a->f3 == b->f3;
  }

};

class Nested4 : public oatpp::DTO {

  DTO_INIT(Nested4, DTO)

  DTO_FIELD(String, f1) = "DB";
  DTO_FIELD(String, f2) = nullptr;
  DTO_FIELD(String, f3) = "Mongo";

public:

  static bool cmp(const oatpp::Object<Nested4>& a, const oatpp::Object<Nested4>& b) {
    return  a->f1 == b->f1 &&
            a->f2 == b->f2 &&
            a->f3 == b->f3;
  }

};

/* Complete object */
class Obj : public oatpp::DTO {

  DTO_INIT(Obj, DTO)

  DTO_FIELD(oatpp::Object<Nested1>, f1) = Nested1::createShared();
  DTO_FIELD(oatpp::Object<Nested2>, f2) = Nested2::createShared();
  DTO_FIELD(oatpp::Object<Nested3>, f3) = nullptr;
  DTO_FIELD(oatpp::Object<Nested4>, f4) = Nested4::createShared();

};

/* No first field */
class Sub1 : public oatpp::DTO {

DTO_INIT(Sub1, DTO)

  DTO_FIELD(oatpp::Object<Nested2>, f2);
  DTO_FIELD(oatpp::Object<Nested3>, f3);
  DTO_FIELD(oatpp::Object<Nested4>, f4);

};

/* No second field */
class Sub2 : public oatpp::DTO {

  DTO_INIT(Sub2, DTO)

  DTO_FIELD(oatpp::Object<Nested1>, f1);
  DTO_FIELD(oatpp::Object<Nested3>, f3);
  DTO_FIELD(oatpp::Object<Nested4>, f4);

};

/* No null field */
class Sub3 : public oatpp::DTO {

  DTO_INIT(Sub3, DTO)

  DTO_FIELD(oatpp::Object<Nested1>, f1);
  DTO_FIELD(oatpp::Object<Nested2>, f2);
  DTO_FIELD(oatpp::Object<Nested4>, f4);

};

/* No last field */
class Sub4 : public oatpp::DTO {

  DTO_INIT(Sub4, DTO)

  DTO_FIELD(oatpp::Object<Nested1>, f1);
  DTO_FIELD(oatpp::Object<Nested2>, f2);
  DTO_FIELD(oatpp::Object<Nested3>, f3);

};

#include OATPP_CODEGEN_END(DTO)

}

void ObjectTest::onRun() {

  oatpp::mongo::bson::mapping::ObjectMapper bsonMapper;

  auto obj = Obj::createShared();

  auto bson = bsonMapper.writeToString(obj);
  auto bcxx = TestUtils::writeJsonToBsonCXX(obj);

  if(bson != bcxx) {
    TestUtils::writeBinary(bson, "oatpp-mongo::bson");
    TestUtils::writeBinary(bcxx, "bsoncxx");
  }

  OATPP_ASSERT(bson == bcxx);


  {
    OATPP_LOGI(TAG, "sub0...");
    auto sub = bsonMapper.readFromString<oatpp::Object<Obj>>(bson);

    OATPP_ASSERT(Nested1::cmp(sub->f1, obj->f1));
    OATPP_ASSERT(Nested2::cmp(sub->f2, obj->f2));
    OATPP_ASSERT(!sub->f3.getPtr() && !obj->f3.getPtr());
    OATPP_ASSERT(Nested4::cmp(sub->f4, obj->f4));

    OATPP_LOGI(TAG, "sub0 - OK");
  }

  {
    OATPP_LOGI(TAG, "sub1...");
    auto sub = bsonMapper.readFromString<oatpp::Object<Sub1>>(bson);

    OATPP_ASSERT(Nested2::cmp(sub->f2, obj->f2));
    OATPP_ASSERT(!sub->f3.getPtr() && !obj->f3.getPtr());
    OATPP_ASSERT(Nested4::cmp(sub->f4, obj->f4));

    OATPP_LOGI(TAG, "sub1 - OK");
  }

  {
    OATPP_LOGI(TAG, "sub2...");
    auto sub = bsonMapper.readFromString<oatpp::Object<Sub2>>(bson);

    OATPP_ASSERT(Nested1::cmp(sub->f1, obj->f1));
    OATPP_ASSERT(!sub->f3.getPtr() && !obj->f3.getPtr());
    OATPP_ASSERT(Nested4::cmp(sub->f4, obj->f4));

    OATPP_LOGI(TAG, "sub2 - OK");
  }

  {
    OATPP_LOGI(TAG, "sub3...");
    auto sub = bsonMapper.readFromString<oatpp::Object<Sub3>>(bson);

    OATPP_ASSERT(Nested1::cmp(sub->f1, obj->f1));
    OATPP_ASSERT(Nested2::cmp(sub->f2, obj->f2));
    OATPP_ASSERT(Nested4::cmp(sub->f4, obj->f4));

    OATPP_LOGI(TAG, "sub3 - OK");
  }

  {
    OATPP_LOGI(TAG, "sub4...");
    auto sub = bsonMapper.readFromString<oatpp::Object<Sub4>>(bson);

    OATPP_ASSERT(Nested1::cmp(sub->f1, obj->f1));
    OATPP_ASSERT(Nested2::cmp(sub->f2, obj->f2));
    OATPP_ASSERT(!sub->f3.getPtr() && !obj->f3.getPtr());

    OATPP_LOGI(TAG, "sub4 - OK");
  }

}

}}}}