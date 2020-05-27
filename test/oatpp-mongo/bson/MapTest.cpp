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

#include "MapTest.hpp"

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

  DTO_FIELD(Fields<String>, f1) = {};
  DTO_FIELD(Fields<String>, f2) = {};
  DTO_FIELD(Fields<String>, f3) = nullptr;
  DTO_FIELD(Fields<String>, f4) = {};

public:

  void fill() {

    f1->push_back({"f1-key1", "f1_1"});
    f1->push_back({"f1-key2", nullptr});
    f1->push_back({"f1-key3", "f1_3_abc"});

    f2->push_back({"f2-key1", "f2_1_2"});
    f2->push_back({"f2-key2", nullptr});
    f2->push_back({"f2-key3", "f2_3_abc_2"});

    f4->push_back({"f4-key1", "f3_1_2_3"});
    f4->push_back({"f4-key2", nullptr});
    f4->push_back({"f4-key3", "f3_3_abc_2_3"});

  }

};

/* No first field */
class Sub1 : public oatpp::DTO {

  DTO_INIT(Sub1, DTO)

  DTO_FIELD(Fields<String>, f2);
  DTO_FIELD(Fields<String>, f3);
  DTO_FIELD(Fields<String>, f4);

};

/* No second field */
class Sub2 : public oatpp::DTO {

  DTO_INIT(Sub2, DTO)

  DTO_FIELD(Fields<String>, f1);
  DTO_FIELD(Fields<String>, f3);
  DTO_FIELD(Fields<String>, f4);

};

/* No null field */
class Sub3 : public oatpp::DTO {

  DTO_INIT(Sub3, DTO)

  DTO_FIELD(Fields<String>, f1);
  DTO_FIELD(Fields<String>, f2);
  DTO_FIELD(Fields<String>, f4);

};

/* No last field */
class Sub4 : public oatpp::DTO {

  DTO_INIT(Sub4, DTO)

  DTO_FIELD(Fields<String>, f3);
  DTO_FIELD(Fields<String>, f2);
  DTO_FIELD(Fields<String>, f1);

};

#include OATPP_CODEGEN_END(DTO)


bool cmpLists(const oatpp::Fields<oatpp::String>& a, const oatpp::Fields<oatpp::String>& b, v_int32 expectedCount) {

  if(a->size() != b->size() || a->size() != expectedCount) {
    return false;
  }

  auto currA = a->begin();
  auto currB = b->begin();

  while(currA != a->end()) {

    auto keyA = currA->first;
    auto keyB = currB->first;

    if(keyA != keyB) {
      return false;
    }

    auto strA = currA->second;
    auto strB = currB->second;

    if(strA && strB) {
      if(strA != strB) {
        return false;
      }
    } else if(strA || strB) {
      return false;
    }

    currA ++;
    currB ++;
  }

  return true;

}


}

void MapTest::onRun() {

  oatpp::mongo::bson::mapping::ObjectMapper bsonMapper;

  {

    auto obj = Obj::createShared();
    obj->fill();

    auto bson = bsonMapper.writeToString(obj);
    auto bcxx = TestUtils::writeJsonToBsonCXX(obj);

    if (bson != bcxx) {
      TestUtils::writeBinary(bson, "oatpp-mongo::bson");
      TestUtils::writeBinary(bcxx, "bsoncxx");
    }

    OATPP_ASSERT(bson == bcxx);


    {
      OATPP_LOGI(TAG, "sub0...");
      auto sub = bsonMapper.readFromString<oatpp::Object<Obj>>(bson);

      OATPP_ASSERT(cmpLists(sub->f1, obj->f1, 3));
      OATPP_ASSERT(cmpLists(sub->f2, obj->f2, 3));
      OATPP_ASSERT(!sub->f3.getPtr() && !obj->f3.getPtr());
      OATPP_ASSERT(cmpLists(sub->f4, obj->f4, 3));

      OATPP_LOGI(TAG, "sub0 - OK");
    }

    {
      OATPP_LOGI(TAG, "sub1...");
      auto sub = bsonMapper.readFromString<oatpp::Object<Sub1>>(bson);

      OATPP_ASSERT(cmpLists(sub->f2, obj->f2, 3));
      OATPP_ASSERT(!sub->f3.getPtr() && !obj->f3.getPtr());
      OATPP_ASSERT(cmpLists(sub->f4, obj->f4, 3));

      OATPP_LOGI(TAG, "sub1 - OK");
    }

    {
      OATPP_LOGI(TAG, "sub2...");
      auto sub = bsonMapper.readFromString<oatpp::Object<Sub2>>(bson);

      OATPP_ASSERT(cmpLists(sub->f1, obj->f1, 3));
      OATPP_ASSERT(!sub->f3.getPtr() && !obj->f3.getPtr());
      OATPP_ASSERT(cmpLists(sub->f4, obj->f4, 3));

      OATPP_LOGI(TAG, "sub2 - OK");
    }

    {
      OATPP_LOGI(TAG, "sub3...");
      auto sub = bsonMapper.readFromString<oatpp::Object<Sub3>>(bson);

      OATPP_ASSERT(cmpLists(sub->f1, obj->f1, 3));
      OATPP_ASSERT(cmpLists(sub->f2, obj->f2, 3));
      OATPP_ASSERT(cmpLists(sub->f4, obj->f4, 3));

      OATPP_LOGI(TAG, "sub3 - OK");
    }

    {
      OATPP_LOGI(TAG, "sub4...");
      auto sub = bsonMapper.readFromString<oatpp::Object<Sub4>>(bson);

      OATPP_ASSERT(cmpLists(sub->f1, obj->f1, 3));
      OATPP_ASSERT(cmpLists(sub->f2, obj->f2, 3));
      OATPP_ASSERT(!sub->f3.getPtr() && !obj->f3.getPtr());

      OATPP_LOGI(TAG, "sub4 - OK");
    }

  }

  {

    oatpp::String s0 = "Hello";
    oatpp::String s1 = "World!";
    oatpp::String s2 = "Oat++ & Mongo";

    auto map = oatpp::Fields<oatpp::String>::createShared();
    map->push_back({"k1", s0});
    map->push_back({"k2", s1});
    map->push_back({"k3", s2});

    auto bson = bsonMapper.writeToString(map);
    auto bcxx = TestUtils::writeJsonToBsonCXX(map);

    if (bson != bcxx) {
      TestUtils::writeBinary(bson, "oatpp-mongo::bson (list root)");
      TestUtils::writeBinary(bcxx, "bsoncxx (list root)");
    }

    OATPP_ASSERT(bson == bcxx);

    auto deMap = bsonMapper.readFromString<oatpp::Fields<oatpp::String>>(bson);

    OATPP_ASSERT(map->size() == deMap->size());
    OATPP_ASSERT(deMap.getValueByKey("k1", "") == s0);
    OATPP_ASSERT(deMap.getValueByKey("k2", "") == s1);
    OATPP_ASSERT(deMap.getValueByKey("k3", "") == s2);

  }

}

}}}}