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


#include "ObjectMapper.hpp"

namespace oatpp { namespace parser { namespace mongocxx { namespace mapping {

ObjectMapper::ObjectMapper(const std::shared_ptr<DtoToMongo::Config>& pSerializerConfig,
                           const std::shared_ptr<MongoToDto::Config>& pDeserializerConfig)
  : serializerConfig(pSerializerConfig)
  , deserializerConfig(pDeserializerConfig)
{}

std::shared_ptr<ObjectMapper> ObjectMapper::createShared(const std::shared_ptr<DtoToMongo::Config>& serializerConfig,
                                                         const std::shared_ptr<MongoToDto::Config>& deserializerConfig){
  return std::make_shared<ObjectMapper>(serializerConfig, deserializerConfig);
}


oatpp::data::mapping::type::AbstractObjectWrapper ObjectMapper::read(bsoncxx::document::view &document,
                                                                     const oatpp::data::mapping::type::Type* const type) const {
  return MongoToDto::deserialize(document, deserializerConfig, type);
}

bsoncxx::document::value ObjectMapper::writeAsDocument(const oatpp::data::mapping::type::AbstractObjectWrapper &variant) const {
  bsoncxx::builder::stream::document doc;
  DtoToMongo::serialize(&doc, variant, serializerConfig);
  return doc << bsoncxx::builder::stream::finalize;
}

}}}}