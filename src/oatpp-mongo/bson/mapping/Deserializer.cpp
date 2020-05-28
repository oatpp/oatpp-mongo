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

namespace oatpp { namespace mongo { namespace bson { namespace mapping {

Deserializer::Deserializer(const std::shared_ptr<Config>& config)
  : m_config(config)
{

  m_methods.resize(data::mapping::type::ClassId::getClassCount(), nullptr);

  setDeserializerMethod(data::mapping::type::__class::String::CLASS_ID, &Deserializer::deserializeString);

  setDeserializerMethod(data::mapping::type::__class::Int8::CLASS_ID, &Deserializer::deserializePrimitive<oatpp::Int8>);
  setDeserializerMethod(data::mapping::type::__class::UInt8::CLASS_ID, &Deserializer::deserializePrimitive<oatpp::UInt8>);

  setDeserializerMethod(data::mapping::type::__class::Int16::CLASS_ID, &Deserializer::deserializePrimitive<oatpp::Int16>);
  setDeserializerMethod(data::mapping::type::__class::UInt16::CLASS_ID, &Deserializer::deserializePrimitive<oatpp::UInt16>);

  setDeserializerMethod(data::mapping::type::__class::Int32::CLASS_ID, &Deserializer::deserializePrimitive<oatpp::Int32>);
  setDeserializerMethod(data::mapping::type::__class::UInt32::CLASS_ID, &Deserializer::deserializePrimitive<oatpp::UInt32>);

  setDeserializerMethod(data::mapping::type::__class::Int64::CLASS_ID, &Deserializer::deserializePrimitive<oatpp::Int64>);
  setDeserializerMethod(data::mapping::type::__class::UInt64::CLASS_ID, &Deserializer::deserializePrimitive<oatpp::UInt64>);

  setDeserializerMethod(data::mapping::type::__class::Float32::CLASS_ID, &Deserializer::deserializePrimitive<Float32>);
  setDeserializerMethod(data::mapping::type::__class::Float64::CLASS_ID, &Deserializer::deserializePrimitive<Float64>);
  setDeserializerMethod(data::mapping::type::__class::Boolean::CLASS_ID, &Deserializer::deserializeBoolean);

  setDeserializerMethod(data::mapping::type::__class::Any::CLASS_ID, &Deserializer::deserializeAny);
  setDeserializerMethod(data::mapping::type::__class::AbstractEnum::CLASS_ID, &Deserializer::deserializeEnum);
  setDeserializerMethod(data::mapping::type::__class::AbstractObject::CLASS_ID, &Deserializer::deserializeObject);

  //----------------
  // Collections

  setDeserializerMethod(data::mapping::type::__class::AbstractVector::CLASS_ID, &Deserializer::deserializeArray<oatpp::AbstractVector>);
  setDeserializerMethod(data::mapping::type::__class::AbstractList::CLASS_ID, &Deserializer::deserializeArray<oatpp::AbstractList>);
  setDeserializerMethod(data::mapping::type::__class::AbstractUnorderedSet::CLASS_ID, &Deserializer::deserializeArray<oatpp::AbstractUnorderedSet>);

  setDeserializerMethod(data::mapping::type::__class::AbstractPairList::CLASS_ID, &Deserializer::deserializeKeyValue<oatpp::AbstractFields>);
  setDeserializerMethod(data::mapping::type::__class::AbstractUnorderedMap::CLASS_ID, &Deserializer::deserializeKeyValue<oatpp::AbstractUnorderedFields>);


  //----------------
  // Other

  setDeserializerMethod(oatpp::mongo::bson::__class::InlineDocument::CLASS_ID, &Deserializer::deserializeInlineDocs);
  setDeserializerMethod(oatpp::mongo::bson::__class::InlineArray::CLASS_ID, &Deserializer::deserializeInlineDocs);

  setDeserializerMethod(oatpp::mongo::bson::__class::ObjectId::CLASS_ID, &Deserializer::deserializeObjectId);

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

const Type* Deserializer::guessType(v_char8 bsonTypeCode) {

  switch(bsonTypeCode) {

    case TypeCode::DOCUMENT_ROOT:       return Fields<Any>::Class::getType();
    case TypeCode::DOUBLE:              return Float64::Class::getType();
    case TypeCode::STRING:              return String::Class::getType();
    case TypeCode::DOCUMENT_EMBEDDED:   return Fields<Any>::Class::getType();
    case TypeCode::DOCUMENT_ARRAY:      return List<Any>::Class::getType();
    case TypeCode::BINARY:              return nullptr;
    case TypeCode::UNDEFINED:           return nullptr;
    case TypeCode::OBJECT_ID:           return ObjectId::Class::getType();
    case TypeCode::BOOLEAN:             return Boolean::Class::getType();
    case TypeCode::DATE_TIME:           return nullptr;
    case TypeCode::NULL_VALUE:          return nullptr;
    case TypeCode::REGEXP:              return nullptr;
    case TypeCode::BD_POINTER:          return nullptr;
    case TypeCode::JAVASCRIPT_CODE:     return nullptr;
    case TypeCode::SYMBOL:              return nullptr;
    case TypeCode::JAVASCRIPT_CODE_WS:  return nullptr;
    case TypeCode::INT_32:              return Int32::Class::getType();
    case TypeCode::TIMESTAMP:           return UInt64::Class::getType();
    case TypeCode::INT_64:              return Int64::Class::getType();
    case TypeCode::DECIMAL_128:         return nullptr;

    case TypeCode::MIN_KEY:             return nullptr;
    case TypeCode::MAX_KEY:             return nullptr;

    default:
      return nullptr;
  }

}

oatpp::Void Deserializer::deserializeBoolean(Deserializer* deserializer,
                                             parser::Caret& caret,
                                             const Type* const type,
                                             v_char8 bsonTypeCode)
{

  (void) deserializer;
  (void) type;

  switch(bsonTypeCode) {

    case TypeCode::NULL_VALUE:
      return oatpp::Void(Boolean::Class::getType());

    case TypeCode::BOOLEAN:
      if(caret.canContinueAtChar(0, 1)) {
        return oatpp::Void(std::make_shared<bool>(false), Boolean::Class::getType());
      } else if(caret.canContinueAtChar(1, 1)) {
        return oatpp::Void(std::make_shared<bool>(true), Boolean::Class::getType());
      }
      caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeBoolean()]: Error. Invalid boolean value.");
      return oatpp::Void(Boolean::Class::getType());

    default:
      caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeBoolean()]: Error. Type-code doesn't match boolean.");
      return oatpp::Void(Boolean::Class::getType());

  }

}

oatpp::Void Deserializer::deserializeString(Deserializer* deserializer,
                                            parser::Caret& caret,
                                            const Type* const type,
                                            v_char8 bsonTypeCode)
{

  (void) deserializer;
  (void) type;

  switch(bsonTypeCode) {

    case TypeCode::NULL_VALUE:
      return oatpp::Void(String::Class::getType());

    case TypeCode::STRING: {
      v_int32 size = Utils::readInt32(caret);
      if (size + caret.getPosition() > caret.getDataSize() || size < 1) {
        caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeString()]: Error. Invalid string size.");
        return nullptr;
      }
      auto label = caret.putLabel();
      caret.inc(size);
      return oatpp::Void(base::StrBuffer::createShared(label.getData(), label.getSize() - 1), String::Class::getType());

    }

    default:
      caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeString()]: Error. Type-code doesn't match string.");
      return oatpp::Void(Boolean::Class::getType());

  }

}

oatpp::Void Deserializer::deserializeInlineDocs(Deserializer* deserializer,
                                                parser::Caret& caret,
                                                const Type* const type,
                                                v_char8 bsonTypeCode)
{

  switch(bsonTypeCode) {

    case TypeCode::NULL_VALUE:
      return oatpp::Void(type);

    case TypeCode::DOCUMENT_EMBEDDED:
    case TypeCode::DOCUMENT_ARRAY:
    {

      auto label = caret.putLabel();

      v_int32 docSize = Utils::readInt32(caret);
      if (docSize - 4 + caret.getPosition() > caret.getDataSize() || docSize < 5) {
        caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeInlineDocs()]: Error. Invalid document size.");
        return nullptr;
      }

      caret.inc(docSize - 4);
      label.end();

      if(bsonTypeCode == DOCUMENT_ARRAY) {
        return oatpp::Void(base::StrBuffer::createShared(label.getData(), label.getSize()), InlineArray::Class::getType());
      }

      return oatpp::Void(base::StrBuffer::createShared(label.getData(), label.getSize()), InlineDocument::Class::getType());

    }

    default:
      caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeList()]: Error. Invalid type code.");
      return nullptr;
  }

}

oatpp::Void Deserializer::deserializeObjectId(Deserializer* deserializer,
                                              parser::Caret& caret,
                                              const Type* const type,
                                              v_char8 bsonTypeCode)
{

  switch(bsonTypeCode) {

    case TypeCode::NULL_VALUE:
      return oatpp::Void(type);

    case TypeCode::OBJECT_ID:
    {

      if(caret.getPosition() + 12 > caret.getDataSize()) {
        caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeObjectId()]: Error. Invalid parsing state.");
        return nullptr;
      }

      auto label = caret.putLabel();
      caret.inc(12);

      return oatpp::Void(std::make_shared<type::ObjectId>(label.getData()), ObjectId::Class::getType());

    }

    default:
      caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeObjectId()]: Error. Invalid type code.");
      return nullptr;
  }

}

oatpp::Void Deserializer::deserializeAny(Deserializer* deserializer,
                                         parser::Caret& caret,
                                         const Type* const type,
                                         v_char8 bsonTypeCode)
{

  if(bsonTypeCode != TypeCode::NULL_VALUE) {

    const Type *const fieldType = guessType(bsonTypeCode);
    if (fieldType != nullptr) {
      auto fieldValue = deserializer->deserialize(caret, fieldType, bsonTypeCode);
      auto anyHandle = std::make_shared<data::mapping::type::AnyHandle>(fieldValue.getPtr(), fieldValue.valueType);
      return oatpp::Void(anyHandle, Any::Class::getType());
    }

  }

  return oatpp::Void(Any::Class::getType());

}

oatpp::Void Deserializer::deserializeEnum(Deserializer* deserializer,
                                          parser::Caret& caret,
                                          const Type* const type,
                                          v_char8 bsonTypeCode)
{

  auto polymorphicDispatcher = static_cast<const data::mapping::type::__class::AbstractEnum::AbstractPolymorphicDispatcher*>(
    type->polymorphicDispatcher
  );

  data::mapping::type::EnumInterpreterError e = data::mapping::type::EnumInterpreterError::OK;
  const auto& value = deserializer->deserialize(caret, polymorphicDispatcher->getInterpretationType(), bsonTypeCode);
  if(caret.hasError()) {
    return nullptr;
  }
  const auto& result = polymorphicDispatcher->fromInterpretation(value, e);

  if(e == data::mapping::type::EnumInterpreterError::OK) {
    return result;
  }

  switch(e) {
    case data::mapping::type::EnumInterpreterError::CONSTRAINT_NOT_NULL:
      caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeEnum()]: Error. Enum constraint violated - 'NotNull'.");
      break;
    default:
      caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeEnum()]: Error. Can't deserialize Enum.");
  }

  return nullptr;

}

oatpp::Void Deserializer::deserializeObject(Deserializer* deserializer,
                                            parser::Caret& caret,
                                            const Type* const type,
                                            v_char8 bsonTypeCode)
{

  switch(bsonTypeCode) {

    case TypeCode::NULL_VALUE:
      return oatpp::Void(type);

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
      const auto& fieldsMap = type->propertiesGetter()->getMap();

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

oatpp::Void Deserializer::deserialize(parser::Caret& caret, const Type* const type, v_char8 bsonTypeCode) {
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
