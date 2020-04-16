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

#include "Deserializer.hpp"

#include "oatpp-mongo/bson/Utils.hpp"

#include "oatpp/parser/json/Utils.hpp" // delete


#include "oatpp/core/utils/ConversionUtils.hpp"

namespace oatpp { namespace mongo { namespace bson { namespace mapping {

Deserializer::Deserializer(const std::shared_ptr<Config>& config)
  : m_config(config)
{

  m_methods.resize(data::mapping::type::ClassId::getClassCount(), nullptr);

  setDeserializerMethod(oatpp::data::mapping::type::__class::String::CLASS_ID, &Deserializer::deserializeString);

  setDeserializerMethod(oatpp::data::mapping::type::__class::Int8::CLASS_ID, &Deserializer::deserializeInt<oatpp::Int8>);
  setDeserializerMethod(oatpp::data::mapping::type::__class::UInt8::CLASS_ID, &Deserializer::deserializeUInt<oatpp::UInt8>);

  setDeserializerMethod(oatpp::data::mapping::type::__class::Int16::CLASS_ID, &Deserializer::deserializeInt<oatpp::Int16>);
  setDeserializerMethod(oatpp::data::mapping::type::__class::UInt16::CLASS_ID, &Deserializer::deserializeUInt<oatpp::UInt16>);

  setDeserializerMethod(oatpp::data::mapping::type::__class::Int32::CLASS_ID, &Deserializer::deserializeInt<oatpp::Int32>);
  setDeserializerMethod(oatpp::data::mapping::type::__class::UInt32::CLASS_ID, &Deserializer::deserializeUInt<oatpp::UInt32>);

  setDeserializerMethod(oatpp::data::mapping::type::__class::Int64::CLASS_ID, &Deserializer::deserializeInt<oatpp::Int64>);
  setDeserializerMethod(oatpp::data::mapping::type::__class::UInt64::CLASS_ID, &Deserializer::deserializeUInt<oatpp::UInt64>);

  setDeserializerMethod(oatpp::data::mapping::type::__class::Float32::CLASS_ID, &Deserializer::deserializeFloat32);
  setDeserializerMethod(oatpp::data::mapping::type::__class::Float64::CLASS_ID, &Deserializer::deserializeFloat64);
  setDeserializerMethod(oatpp::data::mapping::type::__class::Boolean::CLASS_ID, &Deserializer::deserializeBoolean);

  setDeserializerMethod(oatpp::data::mapping::type::__class::AbstractList::CLASS_ID, &Deserializer::deserializeList);
  setDeserializerMethod(oatpp::data::mapping::type::__class::AbstractListMap::CLASS_ID, &Deserializer::deserializeFieldsMap);
  setDeserializerMethod(oatpp::data::mapping::type::__class::AbstractObject::CLASS_ID, &Deserializer::deserializeObject);

}

void Deserializer::setDeserializerMethod(const data::mapping::type::ClassId& classId, DeserializerMethod method) {
  const v_uint32 id = classId.id;
  if(id < m_methods.size()) {
    m_methods[id] = method;
  } else {
    throw std::runtime_error("[oatpp::mongo::bson::mapping::Deserializer::setDeserializerMethod()]: Error. Unknown classId");
  }
}

void Deserializer::skipScope(oatpp::parser::Caret& caret, v_char8 charOpen, v_char8 charClose){

  p_char8 data = caret.getData();
  v_buff_size size = caret.getDataSize();
  v_buff_size pos = caret.getPosition();
  v_int32 scopeCounter = 0;

  bool isInString = false;

  while(pos < size){
    v_char8 a = data[pos];
    if(a == charOpen){
      if(!isInString){
        scopeCounter ++;
      }
    } else if(a == charClose){
      if(!isInString){
        scopeCounter --;
        if(scopeCounter == 0){
          caret.setPosition(pos + 1);
          return;
        }
      }
    } else if(a == '"') {
      isInString = !isInString;
    } else if(a == '\\'){
      pos ++;
    }

    pos ++;

  }
}

void Deserializer::skipString(oatpp::parser::Caret& caret){
  p_char8 data = caret.getData();
  v_buff_size size = caret.getDataSize();
  v_buff_size pos = caret.getPosition();
  v_int32 scopeCounter = 0;
  while(pos < size){
    v_char8 a = data[pos];
    if(a == '"'){
      scopeCounter ++;
      if(scopeCounter == 2) {
        caret.setPosition(pos + 1);
        return;
      }
    } else if(a == '\\'){
      pos ++;
    }
    pos ++;
  }
}

void Deserializer::skipToken(oatpp::parser::Caret& caret){
  p_char8 data = caret.getData();
  v_buff_size size = caret.getDataSize();
  v_buff_size pos = caret.getPosition();
  while(pos < size){
    v_char8 a = data[pos];
    if(a == ' ' || a == '\t' || a == '\n' || a == '\r' || a == '\b' || a == '\f' ||
       a == '}' || a == ',' || a == ']') {
      caret.setPosition(pos);
      return;
    }
    pos ++;
  }
}

void Deserializer::skipValue(oatpp::parser::Caret& caret){
  if(caret.isAtChar('{')){
    skipScope(caret, '{', '}');
  } else if(caret.isAtChar('[')){
    skipScope(caret, '[', ']');
  } else if(caret.isAtChar('"')){
    skipString(caret);
  } else {
    skipToken(caret);
  }
}

data::mapping::type::AbstractObjectWrapper Deserializer::deserializeFloat32(Deserializer* deserializer,
                                                                            parser::Caret& caret,
                                                                            const Type* const type,
                                                                            v_char8 bsonTypeCode)
{

  (void) deserializer;
  (void) type;

  if(caret.isAtText("null", true)){
    return AbstractObjectWrapper(Float32::ObjectWrapper::Class::getType());
  } else {
    return AbstractObjectWrapper(Float32::ObjectType::createAbstract(caret.parseFloat32()), Float32::ObjectWrapper::Class::getType());
  }
}

data::mapping::type::AbstractObjectWrapper Deserializer::deserializeFloat64(Deserializer* deserializer,
                                                                            parser::Caret& caret,
                                                                            const Type* const type,
                                                                            v_char8 bsonTypeCode)
{

  (void) deserializer;
  (void) type;

  if(caret.isAtText("null", true)){
    return AbstractObjectWrapper(Float64::ObjectWrapper::Class::getType());
  } else {
    return AbstractObjectWrapper(Float64::ObjectType::createAbstract(caret.parseFloat64()), Float64::ObjectWrapper::Class::getType());
  }

}

data::mapping::type::AbstractObjectWrapper Deserializer::deserializeBoolean(Deserializer* deserializer,
                                                                            parser::Caret& caret,
                                                                            const Type* const type,
                                                                            v_char8 bsonTypeCode)
{

  (void) deserializer;
  (void) type;

  switch(bsonTypeCode) {

    case TypeCode::NULL_VALUE:
      return AbstractObjectWrapper(Boolean::ObjectWrapper::Class::getType());

    case TypeCode::BOOLEAN:
      if(caret.canContinueAtChar(0, 1)) {
        return AbstractObjectWrapper(Boolean::ObjectType::createAbstract(false), Boolean::ObjectWrapper::Class::getType());
      } else if(caret.canContinueAtChar(1, 1)) {
        return AbstractObjectWrapper(Boolean::ObjectType::createAbstract(true), Boolean::ObjectWrapper::Class::getType());
      }
      caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeBoolean()]: Error. Invalid boolean value.");
      return AbstractObjectWrapper(Boolean::ObjectWrapper::Class::getType());

    default:
      caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeBoolean()]: Error. Type-code doesn't match boolean.");
      return AbstractObjectWrapper(Boolean::ObjectWrapper::Class::getType());

  }

}

data::mapping::type::AbstractObjectWrapper Deserializer::deserializeString(Deserializer* deserializer,
                                                                           parser::Caret& caret,
                                                                           const Type* const type,
                                                                           v_char8 bsonTypeCode)
{

  (void) deserializer;
  (void) type;

  switch(bsonTypeCode) {

    case TypeCode::NULL_VALUE:
      return AbstractObjectWrapper(String::Class::getType());

    case TypeCode::STRING: {
      v_int32 size = Utils::readInt32(caret);
      if (size + caret.getPosition() > caret.getDataSize() || size < 1) {
        caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeString()]: Error. Invalid string size.");
      }
      auto label = caret.putLabel();
      caret.inc(size);
      return AbstractObjectWrapper(base::StrBuffer::createShared(label.getData(), label.getSize() - 1), String::Class::getType());

    }

    default:
      caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeString()]: Error. Type-code doesn't match string.");
      return AbstractObjectWrapper(Boolean::ObjectWrapper::Class::getType());

  }

}

data::mapping::type::AbstractObjectWrapper Deserializer::deserializeList(Deserializer* deserializer,
                                                                         parser::Caret& caret,
                                                                         const Type* const type,
                                                                         v_char8 bsonTypeCode)
{

  switch(bsonTypeCode) {

    case TypeCode::NULL_VALUE:
      return AbstractObjectWrapper(type);

    case TypeCode::DOCUMENT_ROOT:
    case TypeCode::DOCUMENT_EMBEDDED:
    case TypeCode::DOCUMENT_ARRAY:
    {

      v_int32 docSize = Utils::readInt32(caret, Utils::BO_TYPE::UNKNOWN);
      if (docSize - 4 + caret.getPosition() > caret.getDataSize()) {
        caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeList()]: Error. Invalid document size.");
        return nullptr;
      }

      parser::Caret innerCaret(caret.getCurrData(), docSize - 4);

      auto listWrapper = type->creator();
      oatpp::data::mapping::type::PolymorphicWrapper<AbstractList>
        list(std::static_pointer_cast<AbstractList>(listWrapper.getPtr()), listWrapper.valueType);

      Type* itemType = *type->params.begin();

      while(innerCaret.canContinue() && innerCaret.getPosition() < innerCaret.getDataSize() - 1) {

        v_char8 valueTypeCode;
        auto key = Utils::readKey(innerCaret, valueTypeCode);
        if(innerCaret.hasError()){
          caret.inc(innerCaret.getPosition());
          caret.setError(innerCaret.getErrorMessage(), innerCaret.getErrorCode());
          return nullptr;
        }

        auto item = deserializer->deserialize(innerCaret, itemType, valueTypeCode);
        if(innerCaret.hasError()){
          caret.inc(innerCaret.getPosition());
          caret.setError(innerCaret.getErrorMessage(), innerCaret.getErrorCode());
        }

        list->addPolymorphicItem(item);

      }

      if(!innerCaret.canContinueAtChar(0, 1)){
        caret.inc(innerCaret.getPosition());
        if(innerCaret.hasError()) {
          caret.setError(innerCaret.getErrorMessage(), innerCaret.getErrorCode());
        } else {
          caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeList()]: Error. '\\0' - expected");
        }
        return nullptr;
      }

      if(innerCaret.getPosition() != innerCaret.getDataSize()) {
        caret.inc(innerCaret.getPosition());
        caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeList()]: Error. Document parsing failed.");
      }

      caret.inc(innerCaret.getPosition());
      return AbstractObjectWrapper(list.getPtr(), list.valueType);

    }

    default:
      caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeList()]: Error. Invalid type code.");
      return nullptr;
  }

}

data::mapping::type::AbstractObjectWrapper Deserializer::deserializeFieldsMap(Deserializer* deserializer,
                                                                              parser::Caret& caret,
                                                                              const Type* const type,
                                                                              v_char8 bsonTypeCode)
{

  switch(bsonTypeCode) {

    case TypeCode::NULL_VALUE:
      return AbstractObjectWrapper(type);

    case TypeCode::DOCUMENT_ROOT:
    case TypeCode::DOCUMENT_EMBEDDED:
    case TypeCode::DOCUMENT_ARRAY:
    {

      v_int32 docSize = Utils::readInt32(caret, Utils::BO_TYPE::UNKNOWN);
      if (docSize - 4 + caret.getPosition() > caret.getDataSize()) {
        caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeFieldsMap()]: Error. Invalid document size.");
        return nullptr;
      }

      parser::Caret innerCaret(caret.getCurrData(), docSize - 4);

      auto mapWrapper = type->creator();
      oatpp::data::mapping::type::PolymorphicWrapper<AbstractFieldsMap>
        map(std::static_pointer_cast<AbstractFieldsMap>(mapWrapper.getPtr()), mapWrapper.valueType);

      auto it = type->params.begin();
      Type* keyType = *it ++;
      if(keyType->classId.id != oatpp::data::mapping::type::__class::String::CLASS_ID.id){
        throw std::runtime_error("[oatpp::mongo::bson::mapping::Deserializer::deserializeFieldsMap()]: Invalid json map key. Key should be String");
      }
      Type* valueType = *it;

      while(innerCaret.canContinue() && innerCaret.getPosition() < innerCaret.getDataSize() - 1) {

        v_char8 valueTypeCode;
        auto key = Utils::readKey(innerCaret, valueTypeCode);
        if(innerCaret.hasError()){
          caret.inc(innerCaret.getPosition());
          caret.setError(innerCaret.getErrorMessage(), innerCaret.getErrorCode());
          return nullptr;
        }

        map->putPolymorphicItem(key, deserializer->deserialize(innerCaret, valueType, valueTypeCode));

      }

      if(!innerCaret.canContinueAtChar(0, 1)){
        caret.inc(innerCaret.getPosition());
        if(innerCaret.hasError()) {
          caret.setError(innerCaret.getErrorMessage(), innerCaret.getErrorCode());
        } else {
          caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeFieldsMap()]: Error. '\\0' - expected");
        }
        return nullptr;
      }

      if(innerCaret.getPosition() != innerCaret.getDataSize()) {
        caret.inc(innerCaret.getPosition());
        caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeFieldsMap()]: Error. Document parsing failed.");
      }

      caret.inc(innerCaret.getPosition());
      return AbstractObjectWrapper(map.getPtr(), map.valueType);

    }

    default:
      caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeFieldsMap()]: Error. Invalid type code.");
      return nullptr;
  }

}

data::mapping::type::AbstractObjectWrapper Deserializer::deserializeObject(Deserializer* deserializer,
                                                                           parser::Caret& caret,
                                                                           const Type* const type,
                                                                           v_char8 bsonTypeCode)
{

  switch(bsonTypeCode) {

    case TypeCode::NULL_VALUE:
      return AbstractObjectWrapper(type);

    case TypeCode::DOCUMENT_ROOT:
    case TypeCode::DOCUMENT_EMBEDDED:
    case TypeCode::DOCUMENT_ARRAY:
    {

      v_int32 docSize = Utils::readInt32(caret, Utils::BO_TYPE::UNKNOWN);
      if (docSize - 4 + caret.getPosition() > caret.getDataSize()) {
        caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeObject()]: Error. Invalid document size.");
        return nullptr;
      }

      parser::Caret innerCaret(caret.getCurrData(), docSize - 4);

      auto object = type->creator();
      const auto& fieldsMap = type->properties->getMap();

      while(innerCaret.canContinue() && innerCaret.getPosition() < innerCaret.getDataSize() - 1) {

        v_char8 valueType;
        auto key = Utils::readKey(innerCaret, valueType);
        if(innerCaret.hasError()){
          caret.inc(innerCaret.getPosition());
          caret.setError(innerCaret.getErrorMessage(), innerCaret.getErrorCode());
          return nullptr;
        }

        auto fieldIterator = fieldsMap.find(key->std_str());
        if(fieldIterator != fieldsMap.end()){

          auto field = fieldIterator->second;
          field->set(object.get(), deserializer->deserialize(innerCaret, field->type, valueType));

        } else if (deserializer->getConfig()->allowUnknownFields) {
          // TODO - skip value; <---------------------------------------------------------------------------------------!!!!!!
        } else {
          caret.inc(innerCaret.getPosition());
          caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeObject()]: Error. Unknown field");
          return nullptr;
        }

      }

      if(!innerCaret.canContinueAtChar(0, 1)){
        caret.inc(innerCaret.getPosition());
        if(innerCaret.hasError()) {
          caret.setError(innerCaret.getErrorMessage(), innerCaret.getErrorCode());
        } else {
          caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeObject()]: Error. '\\0' - expected");
        }
        return nullptr;
      }

      if(innerCaret.getPosition() != innerCaret.getDataSize()) {
        caret.inc(innerCaret.getPosition());
        caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeObject()]: Error. Document parsing failed.");
      }

      caret.inc(innerCaret.getPosition());
      return object;

    }

    default:
      caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeObject()]: Error. Invalid type code.");
      return nullptr;
  }

}

data::mapping::type::AbstractObjectWrapper Deserializer::deserialize(parser::Caret& caret, const Type* const type, v_char8 bsonTypeCode) {
  auto id = type->classId.id;
  auto& method = m_methods[id];
  if(method) {
    return (*method)(this, caret, type, bsonTypeCode);
  } else {
    throw std::runtime_error("[oatpp::mongo::bson::mapping::Deserializer::deserialize()]: "
                             "Error. No deserialize method for type '" + std::string(type->classId.name) + "'");
  }
}

const std::shared_ptr<Deserializer::Config>& Deserializer::getConfig() {
  return m_config;
}

}}}}
