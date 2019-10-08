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


#ifndef oatpp_parser_mongocxx_mapping_ObjectMapper_hpp
#define oatpp_parser_mongocxx_mapping_ObjectMapper_hpp

#include "DtoToMongo.hpp"
#include "MongoToDto.hpp"

#include "oatpp/core/data/mapping/ObjectMapper.hpp"

#include <bsoncxx/builder/basic/document.hpp>

namespace oatpp { namespace parser { namespace mongocxx { namespace mapping {

/**
 * Json ObjectMapper. Serialized/Deserializes oatpp DTO objects to/from JSON.
 * See [Data Transfer Object(DTO) component](https://oatpp.io/docs/components/dto/). <br>
 * Extends &id:oatpp::base::Countable;, &id:oatpp::data::mapping::ObjectMapper;.
 */
class ObjectMapper : public oatpp::base::Countable {

public:
  /**
   * Constructor.
   * @param pSerializerConfig - &id:oatpp::parser::json::mapping::Serializer::Config;.
   * @param pDeserializerConfig - &id:oatpp::parser::json::mapping::Deserializer::Config;.
   */
  ObjectMapper(const std::shared_ptr<DtoToMongo::Config>& pSerializerConfig = DtoToMongo::Config::createShared(),
               const std::shared_ptr<MongoToDto::Config>& pDeserializerConfig = MongoToDto::Config::createShared());
public:

  /**
   * Create shared ObjectMapper.
   * @param serializerConfig - &id:oatpp::parser::json::mapping::Serializer::Config;.
   * @param deserializerConfig - &id:oatpp::parser::json::mapping::Deserializer::Config;.
   * @return - `std::shared_ptr` to ObjectMapper.
   */
  static std::shared_ptr<ObjectMapper>
  createShared(const std::shared_ptr<DtoToMongo::Config>& serializerConfig = DtoToMongo::Config::createShared(),
               const std::shared_ptr<MongoToDto::Config>& deserializerConfig = MongoToDto::Config::createShared());


  bsoncxx::document::value writeAsDocument(const oatpp::data::mapping::type::AbstractObjectWrapper& variant) const;


  template<class Class>
  typename Class::ObjectWrapper readFromDocument(bsoncxx::document::value &document) const {
    auto type = Class::ObjectWrapper::Class::getType();
    return oatpp::data::mapping::type::static_wrapper_cast<typename Class::ObjectWrapper::ObjectType>(read(document, type));
  }

 private:
  oatpp::data::mapping::type::AbstractObjectWrapper read(bsoncxx::document::value &document,
                                            const oatpp::data::mapping::type::Type* const type) const;

 public:
  /**
   * Serializer config.
   */
  std::shared_ptr<DtoToMongo::Config> serializerConfig;

  /**
   * Deserializer config.
   */
  std::shared_ptr<MongoToDto::Config> deserializerConfig;
  
};
  
}}}}

#endif /* oatpp_parser_mongocxx_mapping_ObjectMapper_hpp */
