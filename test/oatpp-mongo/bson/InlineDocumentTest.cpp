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

#include "InlineDocumentTest.hpp"

#include "oatpp-mongo/TestUtils.hpp"
#include "oatpp-mongo/bson/mapping/ObjectMapper.hpp"

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

namespace oatpp { namespace mongo { namespace test { namespace bson {

namespace {

#include OATPP_CODEGEN_BEGIN(DTO)

class ObjWithInlineDocument : public oatpp::Object {

  DTO_INIT(ObjWithInlineDocument, Object)

  DTO_FIELD(String, f1) = "Hello";
  DTO_FIELD(oatpp::mongo::bson::InlineDocument, inlineDocument);
  DTO_FIELD(String, f2) = "Obj";

};

class ObjWithInlineArray : public oatpp::Object {

  DTO_INIT(ObjWithInlineArray, Object)

  DTO_FIELD(String, f1) = "Hello";
  DTO_FIELD(oatpp::mongo::bson::InlineArray, inlineArray);
  DTO_FIELD(String, f2) = "Obj";

};

class InlineObj : public oatpp::Object {

  DTO_INIT(InlineObj, Object)

  DTO_FIELD(String, f1) = "Hello";
  DTO_FIELD(String, f2) = "Inline";

};


#include OATPP_CODEGEN_END(DTO)

}

void InlineDocumentTest::onRun() {

  oatpp::mongo::bson::mapping::ObjectMapper bsonMapper;

  {
    auto obj = ObjWithInlineDocument::createShared();
    auto bson = bsonMapper.writeToString(obj);
    auto clone = bsonMapper.readFromString<ObjWithInlineDocument>(bson);

    OATPP_ASSERT(clone);
    OATPP_ASSERT(clone->f1 == obj->f1);
    OATPP_ASSERT(!clone->inlineDocument);
    OATPP_ASSERT(clone->f2 == obj->f2);
  }

  {
    auto obj = ObjWithInlineArray::createShared();
    auto bson = bsonMapper.writeToString(obj);
    auto clone = bsonMapper.readFromString<ObjWithInlineArray>(bson);

    OATPP_ASSERT(clone);
    OATPP_ASSERT(clone->f1 == obj->f1);
    OATPP_ASSERT(!clone->inlineArray);
    OATPP_ASSERT(clone->f2 == obj->f2);
  }

  {
    auto inlineObj = InlineObj::createShared();
    auto inlineBson = bsonMapper.writeToString(inlineObj);

    auto obj = ObjWithInlineDocument::createShared();
    obj->inlineDocument = oatpp::mongo::bson::InlineDocument(inlineBson.getPtr());

    auto bson = bsonMapper.writeToString(obj);
    auto clone = bsonMapper.readFromString<ObjWithInlineDocument>(bson);

    OATPP_ASSERT(clone);
    OATPP_ASSERT(clone->f1 == obj->f1);
    OATPP_ASSERT(clone->inlineDocument);
    OATPP_ASSERT(clone->f2 == obj->f2);

    auto inlineClone = bsonMapper.readFromString<ObjWithInlineDocument>(clone->inlineDocument.getPtr());

    OATPP_ASSERT(inlineClone);
    OATPP_ASSERT(inlineClone->f1 == inlineObj->f1);
    OATPP_ASSERT(inlineClone->f2 == inlineObj->f2);

  }

  {
    auto inlineArr = oatpp::List<InlineObj::ObjectWrapper>::createShared();
    auto item = InlineObj::createShared();
    inlineArr->pushBack(item);
    auto inlineBson = bsonMapper.writeToString(inlineArr);

    auto obj = ObjWithInlineArray::createShared();
    obj->inlineArray = oatpp::mongo::bson::InlineArray(inlineBson.getPtr());

    auto bson = bsonMapper.writeToString(obj);
    auto clone = bsonMapper.readFromString<ObjWithInlineArray>(bson);

    OATPP_ASSERT(clone);
    OATPP_ASSERT(clone->f1 == obj->f1);
    OATPP_ASSERT(clone->inlineArray);
    OATPP_ASSERT(clone->f2 == obj->f2);

    auto inlineClone = bsonMapper.readFromString<oatpp::List<InlineObj::ObjectWrapper>>(clone->inlineArray.getPtr());

    OATPP_ASSERT(inlineClone);
    OATPP_ASSERT(inlineClone->count() == 1);

    auto itemClone = inlineClone->get(0);

    OATPP_ASSERT(itemClone->f1 == item->f1);
    OATPP_ASSERT(itemClone->f2 == item->f2);

  }

}

}}}}