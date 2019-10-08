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
 * mongocxx ObjectMapper. Mappes oatpp DTO objects to/from mongocxx documents.
 * See [Data Transfer Object(DTO) component](https://oatpp.io/docs/components/dto/). <br>
 * Extends &id:oatpp::base::Countable;.
 */
class ObjectMapper : public oatpp::base::Countable {

public:
  /**
   * Constructor.
   * @param pSerializerConfig - &id:oatpp::parser::mongocxx::mapping::DtoToMongo::Config;.
   * @param pDeserializerConfig - &id:oatpp::parser::mongocxx::mapping::MongoToDto::Config;.
   */
  ObjectMapper(const std::shared_ptr<DtoToMongo::Config>& pSerializerConfig = DtoToMongo::Config::createShared(),
               const std::shared_ptr<MongoToDto::Config>& pDeserializerConfig = MongoToDto::Config::createShared());
public:

  /**
   * Create shared ObjectMapper.
   * @param pSerializerConfig - &id:oatpp::parser::mongocxx::mapping::DtoToMongo::Config;.
   * @param pDeserializerConfig - &id:oatpp::parser::mongocxx::mapping::MongoToDto::Config;.
   * @return - `std::shared_ptr` to ObjectMapper.
   */
  static std::shared_ptr<ObjectMapper>
  createShared(const std::shared_ptr<DtoToMongo::Config>& serializerConfig = DtoToMongo::Config::createShared(),
               const std::shared_ptr<MongoToDto::Config>& deserializerConfig = MongoToDto::Config::createShared());


  /**
   * Maps the DTO/AbstractObjectWrapper to a mongocxx document
   * @param variant - &id:oatpp::data::mapping::type::AbstractObjectWrapper; the object to map.
   * @return - mongocxx document
   */
  bsoncxx::document::value writeAsDocument(const oatpp::data::mapping::type::AbstractObjectWrapper& variant) const;


  /**
   * Maps the given mongocxx document to a DTO of type &:Class;.
   * throws `std::runtime_error` on unknown fields (if configured) or when the type in &l:document; does not match the type expected by the DTO.
   * @tparam Class - The DTO-type
   * @param document - mongocxx document
   * @return - DTO
   */
  template<class Class>
  typename Class::ObjectWrapper readFromDocument(const bsoncxx::document::view &document) const {
    return oatpp::data::mapping::type::static_wrapper_cast<typename Class::ObjectWrapper::ObjectType>(read(document, Class::ObjectWrapper::Class::getType()));
  }

 private:
  oatpp::data::mapping::type::AbstractObjectWrapper read(const bsoncxx::document::view &document,
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
