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

#include "ListTest.hpp"

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

  DTO_FIELD(List<String>, f1) = List<String>::createShared();
  DTO_FIELD(List<String>, f2) = List<String>::createShared();
  DTO_FIELD(List<String>, f3) = nullptr;
  DTO_FIELD(List<String>, f4) = List<String>::createShared();

public:

  void fill() {

    f1->push_back("f1_1");
    f1->push_back(nullptr);
    f1->push_back("f1_3_abc");

    f2->push_back("f2_1_2");
    f2->push_back(nullptr);
    f2->push_back("f2_3_abc_2");

    f4->push_back("f3_1_2_3");
    f4->push_back(nullptr);
    f4->push_back("f3_3_abc_2_3");

  }

};

/* No first field */
class Sub1 : public oatpp::DTO {

  DTO_INIT(Sub1, DTO)

  DTO_FIELD(List<String>, f2);
  DTO_FIELD(List<String>, f3);
  DTO_FIELD(List<String>, f4);

};

/* No second field */
class Sub2 : public oatpp::DTO {

  DTO_INIT(Sub2, DTO)

  DTO_FIELD(List<String>, f1);
  DTO_FIELD(List<String>, f3);
  DTO_FIELD(List<String>, f4);

};

/* No null field */
class Sub3 : public oatpp::DTO {

  DTO_INIT(Sub3, DTO)

  DTO_FIELD(List<String>, f1);
  DTO_FIELD(List<String>, f2);
  DTO_FIELD(List<String>, f4);

};

/* No last field */
class Sub4 : public oatpp::DTO {

  DTO_INIT(Sub4, DTO)

  DTO_FIELD(List<String>, f3);
  DTO_FIELD(List<String>, f2);
  DTO_FIELD(List<String>, f1);

};

#include OATPP_CODEGEN_END(DTO)

bool cmpLists(const oatpp::List<oatpp::String>& a, const oatpp::List<oatpp::String>& b, v_int32 expectedCount) {

  if(a->size() != b->size() || a->size() != expectedCount) {
    return false;
  }

  auto currA = a->begin();
  auto currB = b->begin();

  while(currA != a->end()) {

    auto strA = *currA;
    auto strB = *currB;

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

void ListTest::onRun() {

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

    OATPP_LOGI(TAG, "Mongo array to list...");

    oatpp::String s0 = "Hello";
    oatpp::String s1 = "World!";
    oatpp::String s2 = "Oat++ & Mongo";

    auto list = oatpp::List<oatpp::String>::createShared();
    list->push_back(s0);
    list->push_back(s1);
    list->push_back(s2);

    auto bson = bsonMapper.writeToString(list);
    auto bcxx = TestUtils::writeJsonToBsonCXX(list);

    if (bson != bcxx) {
      TestUtils::writeBinary(bson, "oatpp-mongo::bson (list root)");
      TestUtils::writeBinary(bcxx, "bsoncxx (list root)");
    }

    OATPP_ASSERT(bson == bcxx);

    auto deList = bsonMapper.readFromString<oatpp::List<oatpp::String>>(bson);

    OATPP_ASSERT(list->size() == deList->size());
    OATPP_ASSERT(deList[0] == s0);
    OATPP_ASSERT(deList[1] == s1);
    OATPP_ASSERT(deList[2] == s2);

    OATPP_LOGI(TAG, "OK");

  }

  {
    OATPP_LOGI(TAG, "Mongo array to Vector...");

    auto bson = bsonMapper.writeToString(oatpp::Vector<oatpp::String>({
      "hello", "world", "vector"
    }));

    auto c = bsonMapper.readFromString<oatpp::Vector<oatpp::String>>(bson);
    OATPP_ASSERT(c);
    OATPP_ASSERT(c->size() == 3);
    OATPP_ASSERT(c[0] == "hello");
    OATPP_ASSERT(c[1] == "world");
    OATPP_ASSERT(c[2] == "vector");

    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Mongo array to UnorderedSet...");

    auto bson = bsonMapper.writeToString(oatpp::UnorderedSet<oatpp::String>({
      "hello", "world", "unordered_set"
    }));

    auto c = bsonMapper.readFromString<oatpp::UnorderedSet<oatpp::String>>(bson);
    OATPP_ASSERT(c);
    OATPP_ASSERT(c->size() == 3);
    OATPP_ASSERT(c["hello"]);
    OATPP_ASSERT(c["world"]);
    OATPP_ASSERT(c["unordered_set"]);

    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Mongo array to Any...");

    auto bson = bsonMapper.writeToString(oatpp::Any(
      oatpp::List<oatpp::Any>({oatpp::String("hello"), oatpp::String("world"), oatpp::String("any")})
    ));

    auto any = bsonMapper.readFromString<oatpp::Any>(bson);
    OATPP_ASSERT(any);
    OATPP_ASSERT(any.getStoredType() == oatpp::Fields<oatpp::Any>::Class::getType());
    auto c = any.retrieve<oatpp::Fields<oatpp::Any>>();
    OATPP_ASSERT(c);
    OATPP_ASSERT(c->size() == 3);

    OATPP_ASSERT(c["0"].retrieve<oatpp::String>() == "hello");
    OATPP_ASSERT(c["1"].retrieve<oatpp::String>() == "world");
    OATPP_ASSERT(c["2"].retrieve<oatpp::String>() == "any");

    OATPP_LOGI(TAG, "OK");
  }

}

}}}}