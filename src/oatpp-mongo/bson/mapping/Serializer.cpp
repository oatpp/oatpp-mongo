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

#include "Serializer.hpp"

#include "oatpp/core/data/stream/BufferStream.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"
#include "oatpp/core/parser/Caret.hpp"

namespace oatpp { namespace mongo { namespace bson { namespace mapping {

Serializer::Serializer(const std::shared_ptr<Config>& config)
  : m_config(config)
{

  m_methods.resize(data::mapping::type::ClassId::getClassCount(), nullptr);

  setSerializerMethod(oatpp::data::mapping::type::__class::String::CLASS_ID, &Serializer::serializeString);

  setSerializerMethod(oatpp::data::mapping::type::__class::Int8::CLASS_ID, &Serializer::serializePrimitive<oatpp::Int8>);
  setSerializerMethod(oatpp::data::mapping::type::__class::UInt8::CLASS_ID, &Serializer::serializePrimitive<oatpp::UInt8>);

  setSerializerMethod(oatpp::data::mapping::type::__class::Int16::CLASS_ID, &Serializer::serializePrimitive<oatpp::Int16>);
  setSerializerMethod(oatpp::data::mapping::type::__class::UInt16::CLASS_ID, &Serializer::serializePrimitive<oatpp::UInt16>);

  setSerializerMethod(oatpp::data::mapping::type::__class::Int32::CLASS_ID, &Serializer::serializePrimitive<oatpp::Int32>);
  setSerializerMethod(oatpp::data::mapping::type::__class::UInt32::CLASS_ID, &Serializer::serializePrimitive<oatpp::UInt32>);

  setSerializerMethod(oatpp::data::mapping::type::__class::Int64::CLASS_ID, &Serializer::serializePrimitive<oatpp::Int64>);
  setSerializerMethod(oatpp::data::mapping::type::__class::UInt64::CLASS_ID, &Serializer::serializePrimitive<oatpp::UInt64>);

  setSerializerMethod(oatpp::data::mapping::type::__class::Float32::CLASS_ID, &Serializer::serializePrimitive<oatpp::Float32>);
  setSerializerMethod(oatpp::data::mapping::type::__class::Float64::CLASS_ID, &Serializer::serializePrimitive<oatpp::Float64>);
  setSerializerMethod(oatpp::data::mapping::type::__class::Boolean::CLASS_ID, &Serializer::serializePrimitive<oatpp::Boolean>);

  setSerializerMethod(oatpp::data::mapping::type::__class::AbstractObject::CLASS_ID, &Serializer::serializeObject);
  setSerializerMethod(oatpp::data::mapping::type::__class::AbstractList::CLASS_ID, &Serializer::serializeList);
  setSerializerMethod(oatpp::data::mapping::type::__class::AbstractListMap::CLASS_ID, &Serializer::serializeFieldsMap);

  setSerializerMethod(oatpp::mongo::bson::__class::InlineDocument::CLASS_ID, &Serializer::serializeInlineDocument);
  setSerializerMethod(oatpp::mongo::bson::__class::InlineArray::CLASS_ID, &Serializer::serializeInlineArray);

}

void Serializer::setSerializerMethod(const data::mapping::type::ClassId& classId, SerializerMethod method) {
  const v_uint32 id = classId.id;
  if(id < m_methods.size()) {
    m_methods[id] = method;
  } else {
    throw std::runtime_error("[oatpp::mongo::bson::mapping::Serializer::setSerializerMethod()]: Error. Unknown classId");
  }
}

void Serializer::serializeString(Serializer* serializer,
                                 data::stream::ConsistentOutputStream* stream,
                                 const data::share::StringKeyLabel& key,
                                 const data::mapping::type::AbstractObjectWrapper& polymorph)
{

  (void) serializer;

  if(!key) {
    throw std::runtime_error("[oatpp::mongo::bson::mapping::Serializer::serializeString()]: Error. The key can't be null.");
  }

  if(polymorph) {

    bson::Utils::writeKey(stream, TypeCode::STRING, key);

    auto str = static_cast<oatpp::base::StrBuffer *>(polymorph.get());
    bson::Utils::writeInt32(stream, str->getSize() + 1);
    stream->writeSimple(str->getData(), str->getSize());
    stream->writeCharSimple(0);

  } else {
    bson::Utils::writeKey(stream, TypeCode::NULL_VALUE, key);
  }

}

void Serializer::serializeInlineDocs(Serializer* serializer,
                                     data::stream::ConsistentOutputStream* stream,
                                     const data::share::StringKeyLabel& key,
                                     TypeCode typeCode,
                                     const data::mapping::type::AbstractObjectWrapper& polymorph)
{
  if(polymorph) {

    auto str = static_cast<oatpp::base::StrBuffer*>(polymorph.get());
    if(str->getSize() < 5) {
      throw std::runtime_error("[oatpp::mongo::bson::mapping::Serializer::serializeInlineDocs()]: Error. Invalid inline object size.");
    }

    oatpp::parser::Caret caret(str->getData(), str->getSize());
    v_int32 inlineSize = bson::Utils::readInt32(caret);

    if(inlineSize != str->getSize()) {
      throw std::runtime_error("[oatpp::mongo::bson::mapping::Serializer::serializeInlineDocs()]: Error. Invalid inline object.");
    }

    bson::Utils::writeKey(stream, typeCode, key);
    stream->writeSimple(str->getData(), str->getSize());

  } else if(key) {
    bson::Utils::writeKey(stream, TypeCode::NULL_VALUE, key);
  } else {
    throw std::runtime_error("[oatpp::mongo::bson::mapping::Serializer::serializeInlineDocs()]: Error. null object with null key.");
  }
}

void Serializer::serializeInlineDocument(Serializer* serializer,
                                         data::stream::ConsistentOutputStream* stream,
                                         const data::share::StringKeyLabel& key,
                                         const data::mapping::type::AbstractObjectWrapper& polymorph)
{
  serializeInlineDocs(serializer, stream, key, TypeCode::DOCUMENT_EMBEDDED, polymorph);
}

void Serializer::serializeInlineArray(Serializer* serializer,
                                      data::stream::ConsistentOutputStream* stream,
                                      const data::share::StringKeyLabel& key,
                                      const data::mapping::type::AbstractObjectWrapper& polymorph)
{
  serializeInlineDocs(serializer, stream, key, TypeCode::DOCUMENT_ARRAY, polymorph);
}

void Serializer::serializeList(Serializer* serializer,
                               data::stream::ConsistentOutputStream* stream,
                               const data::share::StringKeyLabel& key,
                               const data::mapping::type::AbstractObjectWrapper& polymorph)
{

  if(polymorph) {

    bson::Utils::writeKey(stream, TypeCode::DOCUMENT_ARRAY, key);

    data::stream::BufferOutputStream innerStream;

    auto *list = static_cast<AbstractList *>(polymorph.get());
    auto curr = list->getFirstNode();

    v_int32 index = 0;

    while (curr != nullptr) {
      auto value = curr->getData();
      if (value || serializer->getConfig()->includeNullFields) {
        serializer->serialize(&innerStream, utils::conversion::int32ToStr(index), curr->getData());
        index ++;
      }
      curr = curr->getNext();
    }

    bson::Utils::writeInt32(stream, innerStream.getCurrentPosition() + 5);
    stream->writeSimple(innerStream.getData(), innerStream.getCurrentPosition());
    stream->writeCharSimple(0);

  } else if(key) {
    bson::Utils::writeKey(stream, TypeCode::NULL_VALUE, key);
  } else {
    throw std::runtime_error("[oatpp::mongo::bson::mapping::Serializer::serializeList()]: Error. null object with null key.");
  }

}

void Serializer::serializeFieldsMap(Serializer* serializer,
                                    data::stream::ConsistentOutputStream* stream,
                                    const data::share::StringKeyLabel& key,
                                    const data::mapping::type::AbstractObjectWrapper& polymorph)
{

  if(polymorph) {

    bson::Utils::writeKey(stream, TypeCode::DOCUMENT_EMBEDDED, key);

    data::stream::BufferOutputStream innerStream;

    auto map = static_cast<AbstractFieldsMap *>(polymorph.get());

    auto curr = map->getFirstEntry();

    while (curr != nullptr) {
      auto value = curr->getValue();
      if (value || serializer->getConfig()->includeNullFields) {
        auto key = curr->getKey();
        serializer->serialize(&innerStream, key, curr->getValue());
      }
      curr = curr->getNext();
    }

    bson::Utils::writeInt32(stream, innerStream.getCurrentPosition() + 5);
    stream->writeSimple(innerStream.getData(), innerStream.getCurrentPosition());
    stream->writeCharSimple(0);

  } else if(key) {
    bson::Utils::writeKey(stream, TypeCode::NULL_VALUE, key);
  } else {
    throw std::runtime_error("[oatpp::mongo::bson::mapping::Serializer::serializeFieldsMap()]: Error. null object with null key.");
  }

}

void Serializer::serializeObject(Serializer* serializer,
                                 data::stream::ConsistentOutputStream* stream,
                                 const data::share::StringKeyLabel& key,
                                 const data::mapping::type::AbstractObjectWrapper& polymorph)
{

  if(polymorph) {

    bson::Utils::writeKey(stream, TypeCode::DOCUMENT_EMBEDDED, key);

    data::stream::BufferOutputStream innerStream;

    auto fields = polymorph.valueType->properties->getList();
    Object *object = static_cast<Object *>(polymorph.get());

    for (auto const &field : fields) {

      auto value = field->get(object);
      if (value || serializer->getConfig()->includeNullFields) {
        serializer->serialize(&innerStream, field->name, value);
      }

    }

    bson::Utils::writeInt32(stream, innerStream.getCurrentPosition() + 5);
    stream->writeSimple(innerStream.getData(), innerStream.getCurrentPosition());
    stream->writeCharSimple(0);

  } else if(key) {
    bson::Utils::writeKey(stream, TypeCode::NULL_VALUE, key);
  } else {
    throw std::runtime_error("[oatpp::mongo::bson::mapping::Serializer::serializeObject()]: Error. null object with null key.");
  }

}

void Serializer::serialize(data::stream::ConsistentOutputStream* stream,
                           const data::share::StringKeyLabel& key,
                           const data::mapping::type::AbstractObjectWrapper& polymorph)
{
  auto id = polymorph.valueType->classId.id;
  auto& method = m_methods[id];
  if(method) {
    (*method)(this, stream, key, polymorph);
  } else {
    throw std::runtime_error("[oatpp::mongo::bson::mapping::Serializer::serialize()]: "
                             "Error. No serialize method for type '" + std::string(polymorph.valueType->classId.name) + "'");
  }
}

void Serializer::serializeToStream(data::stream::ConsistentOutputStream* stream,
                                   const data::mapping::type::AbstractObjectWrapper& polymorph)
{
  serialize(stream, nullptr, polymorph);
}

const std::shared_ptr<Serializer::Config>& Serializer::getConfig() {
  return m_config;
}

}}}}
