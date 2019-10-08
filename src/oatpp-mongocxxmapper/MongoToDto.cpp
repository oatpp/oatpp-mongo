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


#include "MongoToDto.hpp"

#include <bsoncxx/types/value.hpp>

#include "oatpp/parser/json/Utils.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"

namespace oatpp { namespace parser { namespace mongocxx { namespace mapping {

MongoToDto::AbstractObjectWrapper MongoToDto::readStringValue(const bsoncxx::types::value& caret){

  if (caret.type() == bsoncxx::type::k_utf8 /*caret.isAtText("null", true)*/) {
    auto utf8 = caret.get_utf8();
    auto buffer = oatpp::base::StrBuffer::createShared(utf8.value.size());
    memcpy(buffer->getData(), utf8.value.data(), utf8.value.size());
    return AbstractObjectWrapper(buffer, String::Class::getType());
  } else if(caret.type() == bsoncxx::type::k_null) {
    return AbstractObjectWrapper(String::Class::getType());
  }
  std::string err = "Expected string-value, found ";
  err.append(bsoncxx::to_string(caret.type()));
  throw std::runtime_error(err);
}

MongoToDto::AbstractObjectWrapper MongoToDto::readInt8Value(const bsoncxx::types::value& caret){
  if(caret.type() == bsoncxx::type::k_int32 /*caret.isAtText("null", true)*/){
    v_int8 i8 = caret.get_int32().value & 0xff;
    return AbstractObjectWrapper(Int8::ObjectType::createAbstract(i8), Int8::ObjectWrapper::Class::getType());
  } else if (caret.type() == bsoncxx::type::k_null) {
    return AbstractObjectWrapper(Int8::ObjectWrapper::Class::getType());
  }
  std::string err = "Expected int8-value, found ";
  err.append(bsoncxx::to_string(caret.type()));
  throw std::runtime_error(err);
}

MongoToDto::AbstractObjectWrapper MongoToDto::readInt16Value(const bsoncxx::types::value& caret){
  if(caret.type() == bsoncxx::type::k_int32 /*caret.isAtText("null", true)*/){
    v_int16 i16 = caret.get_int32().value & 0xffff;
    return AbstractObjectWrapper(Int16::ObjectType::createAbstract(i16), Int16::ObjectWrapper::Class::getType());
  } else if (caret.type() == bsoncxx::type::k_null) {
    return AbstractObjectWrapper(Int16::ObjectWrapper::Class::getType());
  }
  std::string err = "Expected int16-value, found ";
  err.append(bsoncxx::to_string(caret.type()));
  throw std::runtime_error(err);
}

MongoToDto::AbstractObjectWrapper MongoToDto::readInt32Value(const bsoncxx::types::value& caret){
  if(caret.type() == bsoncxx::type::k_int32 /*caret.isAtText("null", true)*/){
    return AbstractObjectWrapper(Int32::ObjectType::createAbstract((const v_int32)caret.get_int32()), Int32::ObjectWrapper::Class::getType());
  } else if (caret.type() == bsoncxx::type::k_null) {
    return AbstractObjectWrapper(Int32::ObjectWrapper::Class::getType());
  }
  std::string err = "Expected int32-value, found ";
  err.append(bsoncxx::to_string(caret.type()));
  throw std::runtime_error(err);
}

MongoToDto::AbstractObjectWrapper MongoToDto::readInt64Value(const bsoncxx::types::value& caret){
  if(caret.type() == bsoncxx::type::k_int64 /*caret.isAtText("null", true)*/){
    return AbstractObjectWrapper(Int64::ObjectType::createAbstract((const v_int64)caret.get_int64()), Int64::ObjectWrapper::Class::getType());
  } else if (caret.type() == bsoncxx::type::k_null) {
    return AbstractObjectWrapper(Int64::ObjectWrapper::Class::getType());
  }
  std::string err = "Expected int64-value, found ";
  err.append(bsoncxx::to_string(caret.type()));
  throw std::runtime_error(err);
}

MongoToDto::AbstractObjectWrapper MongoToDto::readFloat32Value(const bsoncxx::types::value& caret){
  if(caret.type() == bsoncxx::type::k_double /*caret.isAtText("null", true)*/){
    v_float32 f32 = caret.get_double();
    return AbstractObjectWrapper(Float32::ObjectType::createAbstract(f32), Float32::ObjectWrapper::Class::getType());
  } else if (caret.type() == bsoncxx::type::k_null) {
    return AbstractObjectWrapper(Float32::ObjectWrapper::Class::getType());
  }
  std::string err = "Expected float-value, found ";
  err.append(bsoncxx::to_string(caret.type()));
  throw std::runtime_error(err);
}

MongoToDto::AbstractObjectWrapper MongoToDto::readFloat64Value(const bsoncxx::types::value& caret){
  if(caret.type() == bsoncxx::type::k_double /*caret.isAtText("null", true)*/){
    return AbstractObjectWrapper(Float64::ObjectType::createAbstract(caret.get_double()), Float64::ObjectWrapper::Class::getType());
  } else if (caret.type() == bsoncxx::type::k_null) {
    return AbstractObjectWrapper(Float64::ObjectWrapper::Class::getType());
  }
  std::string err = "Expected double-value, found ";
  err.append(bsoncxx::to_string(caret.type()));
  throw std::runtime_error(err);
}

MongoToDto::AbstractObjectWrapper MongoToDto::readBooleanValue(const bsoncxx::types::value& caret){
  if(caret.type() == bsoncxx::type::k_bool /*caret.isAtText("null", true)*/){
    return AbstractObjectWrapper(Boolean::ObjectType::createAbstract(caret.get_bool()), Boolean::ObjectWrapper::Class::getType());
  } else if (caret.type() == bsoncxx::type::k_null) {
    return AbstractObjectWrapper(Boolean::ObjectWrapper::Class::getType());
  }
  std::string err = "Expected boolean-value, found ";
  err.append(bsoncxx::to_string(caret.type()));
  throw std::runtime_error(err);
}
  
MongoToDto::AbstractObjectWrapper MongoToDto::readObjectValue(const Type* const type,
                                                        const bsoncxx::types::value& caret,
                                                        const std::shared_ptr<Config>& config){
  if(caret.type() == bsoncxx::type::k_document) {
    return readObject(type, caret, config);
  } else if (caret.type() == bsoncxx::type::k_null) {
    return AbstractObjectWrapper::empty();
  }
  std::string err = "Expected object-value, found ";
  err.append(bsoncxx::to_string(caret.type()));
  throw std::runtime_error(err);
}
  
MongoToDto::AbstractObjectWrapper MongoToDto::readListValue(const Type* const type,
                                                      const bsoncxx::types::value& caret,
                                                      const std::shared_ptr<Config>& config){
  if(caret.type() == bsoncxx::type::k_array) {
    return readList(type, caret, config);
  } else if (caret.type() == bsoncxx::type::k_null) {
    return AbstractObjectWrapper::empty();
  }
  std::string err = "Expected array-value, found ";
  err.append(bsoncxx::to_string(caret.type()));
  throw std::runtime_error(err);
}
  
MongoToDto::AbstractObjectWrapper MongoToDto::readListMapValue(const Type* const type,
                                                                   const bsoncxx::types::value& caret,
                                                                   const std::shared_ptr<Config>& config){
  if(caret.type() == bsoncxx::type::k_document) {
    return readListMap(type, caret, config);
  } else if (caret.type() == bsoncxx::type::k_null) {
    return AbstractObjectWrapper::empty();
  }
  std::string err = "Expected object-value, found ";
  err.append(bsoncxx::to_string(caret.type()));
  throw std::runtime_error(err);
}
  
MongoToDto::AbstractObjectWrapper MongoToDto::readValue(const Type* const type,
                                                  const bsoncxx::types::value& caret,
                                                  const std::shared_ptr<Config>& config){
  
  auto typeName = type->name;
  if(typeName == oatpp::data::mapping::type::__class::String::CLASS_NAME){
    return readStringValue(caret);
  } else if(typeName == oatpp::data::mapping::type::__class::Int8::CLASS_NAME){
    return readInt8Value(caret);
  } else if(typeName == oatpp::data::mapping::type::__class::Int16::CLASS_NAME){
    return readInt16Value(caret);
  } else if(typeName == oatpp::data::mapping::type::__class::Int32::CLASS_NAME){
    return readInt32Value(caret);
  } else if(typeName == oatpp::data::mapping::type::__class::Int64::CLASS_NAME){
    return readInt64Value(caret);
  } else if(typeName == oatpp::data::mapping::type::__class::Float32::CLASS_NAME){
    return readFloat32Value(caret);
  } else if(typeName == oatpp::data::mapping::type::__class::Float64::CLASS_NAME){
    return readFloat64Value(caret);
  } else if(typeName == oatpp::data::mapping::type::__class::Boolean::CLASS_NAME){
    return readBooleanValue(caret);
  } else if(typeName == oatpp::data::mapping::type::__class::AbstractObject::CLASS_NAME){
    return readObjectValue(type, caret, config);
  } else if(typeName == oatpp::data::mapping::type::__class::AbstractList::CLASS_NAME){
    return readListValue(type, caret, config);
  } else if(typeName == oatpp::data::mapping::type::__class::AbstractListMap::CLASS_NAME){
    return readListMapValue(type, caret, config);
  }
  
  return AbstractObjectWrapper::empty();
  
}

MongoToDto::AbstractObjectWrapper MongoToDto::readList(const Type* type,
                                                 const bsoncxx::types::value& caret,
                                                 const std::shared_ptr<Config>& config){

  if(caret.type() == bsoncxx::type::k_array) {

    auto listWrapper = type->creator();
    oatpp::data::mapping::type::PolymorphicWrapper<AbstractList>
        list(std::static_pointer_cast<AbstractList>(listWrapper.getPtr()), listWrapper.valueType);

    Type *itemType = *type->params.begin();

    auto array = caret.get_array().value;
    for (const auto & curr : array) {
      auto item = readValue(itemType, curr.get_value(), config);
      list->addPolymorphicItem(item);
    }

    return list;

  } else if(caret.type() == bsoncxx::type::k_null) {
    return AbstractObjectWrapper(AbstractList::ObjectWrapper::Class::getType());
  }
  std::string err = "Expected list, found ";
  err.append(bsoncxx::to_string(caret.type()));
  throw std::runtime_error(err);

}
  
MongoToDto::AbstractObjectWrapper MongoToDto::readListMap(const Type* type,
                                                              const bsoncxx::types::value& caret,
                                                              const std::shared_ptr<Config>& config){
  if(caret.type() == bsoncxx::type::k_document) {
    auto view = caret.get_document();
    return readListMap(type, view, config);
  } else if(caret.type() == bsoncxx::type::k_null) {
    return AbstractObjectWrapper(type);
  }
  std::string err = "Expected list-map, found ";
  err.append(bsoncxx::to_string(caret.type()));
  throw std::runtime_error(err);
}
  
MongoToDto::AbstractObjectWrapper MongoToDto::readObject(const Type* type,
                                                   const bsoncxx::types::value& caret,
                                                   const std::shared_ptr<Config>& config){
  if(caret.type() == bsoncxx::type::k_document) {
    auto view = caret.get_document();
    return readObject(type, view, config);
  } else if(caret.type() == bsoncxx::type::k_null) {
    return AbstractObjectWrapper(type);
  }
  std::string err = "Expected object, found ";
  err.append(bsoncxx::to_string(caret.type()));
  throw std::runtime_error(err);
}


MongoToDto::AbstractObjectWrapper MongoToDto::readListMap(const MongoToDto::Type *const type,
                                                        const bsoncxx::document::view &caret,
                                                        const std::shared_ptr<MongoToDto::Config> &config) {

  auto mapWrapper = type->creator();
  oatpp::data::mapping::type::PolymorphicWrapper<AbstractListMap>
      map(std::static_pointer_cast<AbstractListMap>(mapWrapper.getPtr()), mapWrapper.valueType);

  auto it = type->params.begin();
  Type* keyType = *it ++;
  if(keyType->name != oatpp::data::mapping::type::__class::String::CLASS_NAME){
    throw std::runtime_error("[oatpp::parser::json::mapping::MongoToDto::readListMap()]: Invalid json map key. Key should be String");
  }
  Type* valueType = *it;

  for(auto curr = caret.begin(); curr != caret.end(); ++curr) {
    oatpp::String key(curr->key().data(), curr->keylen(), true);
    map->putPolymorphicItem(key, readValue(valueType, caret[curr->key()].get_value(), config));
  }

  return AbstractObjectWrapper(map.getPtr(), map.valueType);
}

MongoToDto::AbstractObjectWrapper MongoToDto::readObject(const MongoToDto::Type *const type,
                                                       const bsoncxx::document::view &caret,
                                                       const std::shared_ptr<MongoToDto::Config> &config) {

  auto object = type->creator();
  const auto& fieldsMap = type->properties->getMap();

  for(auto curr = caret.begin(); curr != caret.end(); ++curr) {
    std::string bsonkey = curr->key().to_string();
    auto fieldIterator = fieldsMap.find(bsonkey);
    if(fieldIterator != fieldsMap.end()){

      auto field = fieldIterator->second;
      field->set(object.get(), readValue(field->type, caret[curr->key()].get_value(), config));

    } else if (!config->allowUnknownFields) {

      std::string err = "Unknown field ";
      err.append(bsonkey);
      throw std::runtime_error(err);

    }
  }

  return object;
}

}}}}
