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


#include "DtoToMongo.hpp"

#include "oatpp/parser/json/Utils.hpp"
#include <string>
#include <bsoncxx/types.hpp>

namespace oatpp { namespace parser { namespace mongocxx { namespace mapping {

  
void DtoToMongo::writeValue(bsoncxx::builder::stream::document::key_context* stream, const AbstractObjectWrapper& polymorph, const std::shared_ptr<Config>& config) {

  if(!polymorph) {
    return;
  }


  const char* typeName = polymorph.valueType->name;
  
  if(typeName == oatpp::data::mapping::type::__class::AbstractListMap::CLASS_NAME) {
    const AbstractFieldsMap::ObjectWrapper& map = oatpp::data::mapping::type::static_wrapper_cast<AbstractFieldsMap>(polymorph);
    auto curr = map->getFirstEntry();
    while(curr != nullptr){
      auto value = curr->getValue();
      if(value || config->includeNullFields) {
        const auto& key = curr->getKey();
        writeSimpleKV(stream, std::string((const char *) key->getData(), key->getSize()), curr->getValue(), config);
      }
      curr = curr->getNext();
    }
  } else if(typeName == oatpp::data::mapping::type::__class::AbstractObject::CLASS_NAME) {
    const PolymorphicWrapper<Object>& polymorphmoprh =  oatpp::data::mapping::type::static_wrapper_cast<Object>(polymorph);
    auto fields = polymorph.valueType->properties->getList();
    Object* object = polymorphmoprh.get();
    for (auto const& field : fields) {
      auto value = field->get(object);
      if(value || config->includeNullFields) {
        writeSimpleKV(stream, std::string(field->name), value, config);
      }
    }
  } else {
    std::string err = "[oatpp::kafka::protocol::mapping::DtoToMongo::writeField]: Datatype ";
    err.append(typeName);
    err.append(" can not appear in document");
    throw std::runtime_error(err);
  }
  
}

void DtoToMongo::writeObjValue(bsoncxx::builder::stream::document::key_context* stream, const AbstractObjectWrapper& polymorph, const std::shared_ptr<Config>& config) {

  if(!polymorph) {
    return;
  }

  const char* typeName = polymorph.valueType->name;

  if(typeName == oatpp::data::mapping::type::__class::AbstractListMap::CLASS_NAME) {

    const AbstractFieldsMap::ObjectWrapper& map = oatpp::data::mapping::type::static_wrapper_cast<AbstractFieldsMap>(polymorph);
    auto curr = map->getFirstEntry();
    while(curr != nullptr){
      auto value = curr->getValue();
      if(value || config->includeNullFields) {
        writeSimpleKV(stream,
                      std::string((const char *) curr->getKey()->getData(), curr->getKey()->getSize()),
                      value,
                      config);
      }
      curr = curr->getNext();
    }

  } else if(typeName == oatpp::data::mapping::type::__class::AbstractObject::CLASS_NAME) {

    const PolymorphicWrapper<Object>& polymorphmoprh =  oatpp::data::mapping::type::static_wrapper_cast<Object>(polymorph);
    auto fields = polymorph.valueType->properties->getList();
    Object* object = polymorphmoprh.get();
    for (auto const& field : fields) {
      auto value = field->get(object);
      if(value || config->includeNullFields) {
        writeSimpleKV(stream, std::string(field->name), value, config);
      }
    }

  } else {
    std::string err = "[oatpp::kafka::protocol::mapping::DtoToMongo::writeField]: Datatype ";
    err.append(typeName);
    err.append(" is not an object value");
    throw std::runtime_error(err);
  }
}

void DtoToMongo::writeArrValue(bsoncxx::builder::stream::array::array_context* stream, const AbstractObjectWrapper& polymorph, const std::shared_ptr<Config>& config) {

  if(!polymorph) {
    return;
  }

  const char* typeName = polymorph.valueType->name;

  if(typeName == oatpp::data::mapping::type::__class::AbstractList::CLASS_NAME) {

    const PolymorphicWrapper<AbstractList>& polymorphmoprh =  oatpp::data::mapping::type::static_wrapper_cast<AbstractList>(polymorph);
    auto curr = polymorphmoprh->getFirstNode();
    while(curr != nullptr){
      auto value = curr->getData();
      if(value || config->includeNullFields) {
        writeSimpleV(stream, curr->getData(), config);
      }
      curr = curr->getNext();
    }

  } else {
    std::string err = "[oatpp::kafka::protocol::mapping::DtoToMongo::writeField]: Datatype ";
    err.append(typeName);
    err.append(" is not an list value");
    throw std::runtime_error(err);
  }
}


void DtoToMongo::writeSimpleKV(bsoncxx::builder::stream::document::key_context *stream,
                               const std::string &key,
                               const DtoToMongo::AbstractObjectWrapper &polymorph,
                               const std::shared_ptr<DtoToMongo::Config> &config) {

  if(!polymorph) {
    if(config->includeNullFields) {
      /**stream = */*stream << key << bsoncxx::types::b_null{};
      return;
    } else {
      return;
    }
  }

  const char* typeName = polymorph.valueType->name;


  // POD
  if(typeName == oatpp::data::mapping::type::__class::String::CLASS_NAME) {
    auto str = oatpp::data::mapping::type::static_wrapper_cast<oatpp::base::StrBuffer>(polymorph);
    /**stream = */*stream << key << std::string((const char*)str->getData(), str->getSize());
  } else if(typeName == oatpp::data::mapping::type::__class::Int8::CLASS_NAME) {
    /**stream = */*stream << key << bsoncxx::types::b_int32{oatpp::data::mapping::type::static_wrapper_cast<Int8::ObjectType>(polymorph)->getValue()};
  } else if(typeName == oatpp::data::mapping::type::__class::Int16::CLASS_NAME) {
    /**stream = */*stream << key << bsoncxx::types::b_int32{oatpp::data::mapping::type::static_wrapper_cast<Int16::ObjectType>(polymorph)->getValue()};
  } else if(typeName == oatpp::data::mapping::type::__class::Int32::CLASS_NAME) {
    /**stream = */*stream << key << bsoncxx::types::b_int32{oatpp::data::mapping::type::static_wrapper_cast<Int32::ObjectType>(polymorph)->getValue()};
  } else if(typeName == oatpp::data::mapping::type::__class::Int64::CLASS_NAME) {
    /**stream = */*stream << key << bsoncxx::types::b_int64{oatpp::data::mapping::type::static_wrapper_cast<Int64::ObjectType>(polymorph)->getValue()};
  } else if(typeName == oatpp::data::mapping::type::__class::Float32::CLASS_NAME) {
    /**stream = */*stream << key << bsoncxx::types::b_double{oatpp::data::mapping::type::static_wrapper_cast<Float32::ObjectType>(polymorph)->getValue()};
  } else if(typeName == oatpp::data::mapping::type::__class::Float64::CLASS_NAME) {
    /**stream = */*stream << key << bsoncxx::types::b_double{oatpp::data::mapping::type::static_wrapper_cast<Float64::ObjectType>(polymorph)->getValue()};
  } else if(typeName == oatpp::data::mapping::type::__class::Boolean::CLASS_NAME) {
    /**stream = */*stream << key << bsoncxx::types::b_bool{oatpp::data::mapping::type::static_wrapper_cast<Boolean::ObjectType>(polymorph)->getValue()};
  }

  else if(typeName == oatpp::data::mapping::type::__class::AbstractListMap::CLASS_NAME) {

    bsoncxx::builder::stream::document doc;
    writeObjValue(&doc, polymorph, config);
    /**stream = */*stream << key << doc;

  } else if(typeName == oatpp::data::mapping::type::__class::AbstractObject::CLASS_NAME) {

    bsoncxx::builder::stream::document doc;
    writeObjValue(&doc, polymorph, config);
    /**stream = */*stream << key << doc;

  } else if(typeName == oatpp::data::mapping::type::__class::AbstractList::CLASS_NAME) {

    bsoncxx::builder::stream::array arr;
    writeArrValue(&arr, polymorph, config);
    /**stream = */*stream << key << arr;

  } else {
    std::string err = "[oatpp::kafka::protocol::mapping::DtoToMongo::writeField]: Datatype ";
    err.append(typeName);
    err.append(" is not an simple value");
    throw std::runtime_error(err);
  }

}
void DtoToMongo::writeSimpleV(bsoncxx::builder::stream::array::array_context *stream,
                              const DtoToMongo::AbstractObjectWrapper &polymorph,
                              const std::shared_ptr<DtoToMongo::Config> &config) {

  const char* typeName = polymorph.valueType->name;

  if(!polymorph) {
    if(config->includeNullFields) {
      /**stream = */*stream << bsoncxx::types::b_null{};
    } else {
      return;
    }
  }

  // POD
  if(typeName == oatpp::data::mapping::type::__class::String::CLASS_NAME) {
    auto str = oatpp::data::mapping::type::static_wrapper_cast<oatpp::base::StrBuffer>(polymorph);
    /**stream = */*stream << std::string((const char*)str->getData(), str->getSize());
  } else if(typeName == oatpp::data::mapping::type::__class::Int8::CLASS_NAME) {
    /**stream = */*stream << bsoncxx::types::b_int32{oatpp::data::mapping::type::static_wrapper_cast<Int8::ObjectType>(polymorph)->getValue()};
  } else if(typeName == oatpp::data::mapping::type::__class::Int16::CLASS_NAME) {
    /**stream = */*stream << bsoncxx::types::b_int32{oatpp::data::mapping::type::static_wrapper_cast<Int16::ObjectType>(polymorph)->getValue()};
  } else if(typeName == oatpp::data::mapping::type::__class::Int32::CLASS_NAME) {
    /**stream = */*stream << bsoncxx::types::b_int32{oatpp::data::mapping::type::static_wrapper_cast<Int32::ObjectType>(polymorph)->getValue()};
  } else if(typeName == oatpp::data::mapping::type::__class::Int64::CLASS_NAME) {
    /**stream = */*stream << bsoncxx::types::b_int64{oatpp::data::mapping::type::static_wrapper_cast<Int64::ObjectType>(polymorph)->getValue()};
  } else if(typeName == oatpp::data::mapping::type::__class::Float32::CLASS_NAME) {
    /**stream = */*stream << bsoncxx::types::b_double{oatpp::data::mapping::type::static_wrapper_cast<Float32::ObjectType>(polymorph)->getValue()};
  } else if(typeName == oatpp::data::mapping::type::__class::Float64::CLASS_NAME) {
    /**stream = */*stream << bsoncxx::types::b_double{oatpp::data::mapping::type::static_wrapper_cast<Float64::ObjectType>(polymorph)->getValue()};
  } else if(typeName == oatpp::data::mapping::type::__class::Boolean::CLASS_NAME) {
    /**stream = */*stream << bsoncxx::types::b_bool{oatpp::data::mapping::type::static_wrapper_cast<Boolean::ObjectType>(polymorph)->getValue()};
  }

  else if(typeName == oatpp::data::mapping::type::__class::AbstractListMap::CLASS_NAME) {

    bsoncxx::builder::stream::document doc;
    writeObjValue(&doc, polymorph, config);
    /**stream = */*stream << doc;

  } else if(typeName == oatpp::data::mapping::type::__class::AbstractObject::CLASS_NAME) {

    bsoncxx::builder::stream::document doc;
    writeObjValue(&doc, polymorph, config);
    /**stream = */*stream << doc;

  } else if(typeName == oatpp::data::mapping::type::__class::AbstractList::CLASS_NAME) {

    bsoncxx::builder::stream::array arr;
    writeArrValue(&arr, polymorph, config);
    /**stream = */*stream << arr;

  } else {
    std::string err = "[oatpp::kafka::protocol::mapping::DtoToMongo::writeField]: Datatype ";
    err.append(typeName);
    err.append(" is not an simple value");
    throw std::runtime_error(err);
  }

}

}}}}
