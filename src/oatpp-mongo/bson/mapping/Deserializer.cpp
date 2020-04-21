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

#include "oatpp/core/utils/ConversionUtils.hpp"

namespace oatpp { namespace mongo { namespace bson { namespace mapping {

Deserializer::Deserializer(const std::shared_ptr<Config>& config)
  : m_config(config)
{

  m_methods.resize(data::mapping::type::ClassId::getClassCount(), nullptr);

  setDeserializerMethod(oatpp::data::mapping::type::__class::String::CLASS_ID, &Deserializer::deserializeString);

  setDeserializerMethod(oatpp::data::mapping::type::__class::Int8::CLASS_ID, &Deserializer::deserializePrimitive<oatpp::Int8>);
  setDeserializerMethod(oatpp::data::mapping::type::__class::UInt8::CLASS_ID, &Deserializer::deserializePrimitive<oatpp::UInt8>);

  setDeserializerMethod(oatpp::data::mapping::type::__class::Int16::CLASS_ID, &Deserializer::deserializePrimitive<oatpp::Int16>);
  setDeserializerMethod(oatpp::data::mapping::type::__class::UInt16::CLASS_ID, &Deserializer::deserializePrimitive<oatpp::UInt16>);

  setDeserializerMethod(oatpp::data::mapping::type::__class::Int32::CLASS_ID, &Deserializer::deserializePrimitive<oatpp::Int32>);
  setDeserializerMethod(oatpp::data::mapping::type::__class::UInt32::CLASS_ID, &Deserializer::deserializePrimitive<oatpp::UInt32>);

  setDeserializerMethod(oatpp::data::mapping::type::__class::Int64::CLASS_ID, &Deserializer::deserializePrimitive<oatpp::Int64>);
  setDeserializerMethod(oatpp::data::mapping::type::__class::UInt64::CLASS_ID, &Deserializer::deserializePrimitive<oatpp::UInt64>);

  setDeserializerMethod(oatpp::data::mapping::type::__class::Float32::CLASS_ID, &Deserializer::deserializePrimitive<Float32>);
  setDeserializerMethod(oatpp::data::mapping::type::__class::Float64::CLASS_ID, &Deserializer::deserializePrimitive<Float64>);
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

void Deserializer::skipCString(parser::Caret& caret) {
  caret.findChar(0);
  if(!caret.canContinueAtChar(0, 1)) {
    caret.setError("[oatpp::mongo::bson::mapping::Deserializer::skipCString()]: Error. Unterminated CString.");
  }
}

void Deserializer::skipSizedElement(parser::Caret& caret, v_int32 additionalBytes) {
  v_int32 size = Utils::readInt32(caret);
  if (size + caret.getPosition() + additionalBytes > caret.getDataSize() || size + additionalBytes < 0) {
    caret.setError("[oatpp::mongo::bson::mapping::Deserializer::skipSizedElement()]: Error. Invalid element size.");
  }
  caret.inc(size + additionalBytes);
}

void Deserializer::skipElement(parser::Caret& caret, v_char8 bsonTypeCode) {

  switch(bsonTypeCode) {

    case TypeCode::DOUBLE: caret.inc(8);                            break;
    case TypeCode::STRING: skipSizedElement(caret);                 break;
    case TypeCode::DOCUMENT_EMBEDDED: skipSizedElement(caret, -4);  break;
    case TypeCode::DOCUMENT_ARRAY: skipSizedElement(caret, -4);     break;
    case TypeCode::BINARY: skipSizedElement(caret, 1);              break;
    case TypeCode::UNDEFINED:                                       break;
    case TypeCode::OBJECT_ID: caret.inc(12);                        break;
    case TypeCode::BOOLEAN: caret.inc();                            break;
    case TypeCode::DATE_TIME: caret.inc(8);                         break;
    case TypeCode::NULL_VALUE:                                      break;
    case TypeCode::REGEXP: skipCString(caret); skipCString(caret);  break;
    case TypeCode::BD_POINTER: skipSizedElement(caret, 12);         break;
    case TypeCode::JAVASCRIPT_CODE: skipSizedElement(caret);        break;
    case TypeCode::SYMBOL: skipSizedElement(caret);                 break;
    case TypeCode::JAVASCRIPT_CODE_WS: skipSizedElement(caret, -4); break;
    case TypeCode::INT_32: caret.inc(4);                            break;
    case TypeCode::TIMESTAMP: caret.inc(8);                         break;
    case TypeCode::INT_64: caret.inc(8);                            break;
    case TypeCode::DECIMAL_128: caret.inc(16);                      break;

    case TypeCode::MIN_KEY:                                         break;
    case TypeCode::MAX_KEY:                                         break;

    default:
      caret.setError("[oatpp::mongo::bson::mapping::Deserializer::skipElement()]: Error. Unknown element type-code.");
      return;
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
        return nullptr;
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

      v_int32 docSize = Utils::readInt32(caret);
      if (docSize - 4 + caret.getPosition() > caret.getDataSize() || docSize < 4) {
        caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeList()]: Error. Invalid document size.");
        return nullptr;
      }

      parser::Caret innerCaret(caret.getCurrData(), docSize - 4);

      auto listWrapper = type->creator();
      oatpp::data::mapping::type::PolymorphicWrapper<AbstractList>
        list(std::static_pointer_cast<AbstractList>(listWrapper.getPtr()), listWrapper.valueType);

      Type* itemType = *type->params.begin();
      v_int32 expectedIndex = 0;
      while(innerCaret.canContinue() && innerCaret.getPosition() < innerCaret.getDataSize() - 1) {

        v_char8 valueTypeCode;
        auto key = Utils::readKey(innerCaret, valueTypeCode);
        if(innerCaret.hasError()){
          caret.inc(innerCaret.getPosition());
          caret.setError(innerCaret.getErrorMessage(), innerCaret.getErrorCode());
          return nullptr;
        }

        bool success;
        v_int32 keyIntValue = utils::conversion::strToInt32(key, success);
        if(!success || keyIntValue != expectedIndex) {
          caret.inc(innerCaret.getPosition());
          caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeList()]: Error. Array invalid index value. Looks like it's not an array.");
          return nullptr;
        }

        auto item = deserializer->deserialize(innerCaret, itemType, valueTypeCode);
        if(innerCaret.hasError()){
          caret.inc(innerCaret.getPosition());
          caret.setError(innerCaret.getErrorMessage(), innerCaret.getErrorCode());
          return nullptr;
        }

        list->addPolymorphicItem(item);
        ++ expectedIndex;

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

      v_int32 docSize = Utils::readInt32(caret);
      if (docSize - 4 + caret.getPosition() > caret.getDataSize() || docSize < 4) {
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

      v_int32 docSize = Utils::readInt32(caret);
      if (docSize - 4 + caret.getPosition() > caret.getDataSize() || docSize < 4) {
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
          skipElement(innerCaret, valueType);
          if(innerCaret.hasError()){
            caret.inc(innerCaret.getPosition());
            caret.setError(innerCaret.getErrorMessage(), innerCaret.getErrorCode());
            return nullptr;
          }
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
