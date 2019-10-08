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


#ifndef oatpp_parser_mongocxx_mapping_Serializer_hpp
#define oatpp_parser_mongocxx_mapping_Serializer_hpp

#include "oatpp/core/data/mapping/type/ListMap.hpp"
#include "oatpp/core/data/mapping/type/List.hpp"
#include "oatpp/core/data/mapping/type/Object.hpp"
#include "oatpp/core/data/mapping/type/Primitive.hpp"
#include "oatpp/core/data/mapping/type/Type.hpp"
#include "oatpp/core/data/stream/ChunkedBuffer.hpp"

#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>

#include "oatpp/core/collection/LinkedList.hpp"
#include "oatpp/core/Types.hpp"

namespace oatpp { namespace parser { namespace mongocxx { namespace mapping {

/**
 * Json Serializer.
 * Serializes oatpp DTO object to json. See [Data Transfer Object(DTO) component](https://oatpp.io/docs/components/dto/).
 */
class DtoToMongo {
public:
  typedef oatpp::data::mapping::type::Type Type;
  typedef oatpp::data::mapping::type::Type::Property Property;
  typedef oatpp::data::mapping::type::Type::Properties Properties;
  
  typedef oatpp::data::mapping::type::Object Object;
  typedef oatpp::String String;
  
  template<class T>
  using PolymorphicWrapper = data::mapping::type::PolymorphicWrapper<T>;
  
  typedef oatpp::data::mapping::type::AbstractObjectWrapper AbstractObjectWrapper;
  typedef oatpp::data::mapping::type::List<AbstractObjectWrapper> AbstractList;
  typedef oatpp::data::mapping::type::ListMap<String, AbstractObjectWrapper> AbstractFieldsMap;
  
public:
  /**
   * Serializer config.
   */
  class Config : public oatpp::base::Countable {
  public:
    /**
     * Constructor.
     */
    Config() = default;
  public:

    /**
     * Create shared config.
     * @return - `std::shared_ptr` to Config.
     */
    static std::shared_ptr<Config> createShared(){
      return std::make_shared<Config>();
    }

    /**
     * Include fields with value == nullptr into serialized json.
     */
    bool includeNullFields = true;

  };
  
private:

  static void writeValue(bsoncxx::builder::stream::document::key_context* stream, const AbstractObjectWrapper& polymorph, const std::shared_ptr<Config>& config);
  static void writeSimpleV(bsoncxx::builder::stream::array::array_context *stream,
                           const AbstractObjectWrapper &polymorph,
                           const std::shared_ptr<Config> &config);
  static void writeObjValue(bsoncxx::builder::stream::document::key_context* stream, const AbstractObjectWrapper& polymorph, const std::shared_ptr<Config>& config);
  static void writeSimpleKV(bsoncxx::builder::stream::document::key_context *stream,
                            const std::string &key,
                            const AbstractObjectWrapper &polymorph,
                            const std::shared_ptr<Config> &config);
  static void writeArrValue(bsoncxx::builder::stream::array::array_context* stream, const AbstractObjectWrapper& polymorph, const std::shared_ptr<Config>& config);


 public:

  /**
   * Serialize DTO object to stream.
   * @param stream - stream to write serialized object to. &id:oatpp::data::stream::ConsistentOutputStream;. <br>
   * @param polymorph - DTO object to serialize.
   * @param config - &l:Serializer::Config;.
   */
  static void serialize(bsoncxx::builder::stream::document* stream,
                        const oatpp::data::mapping::type::AbstractObjectWrapper& polymorph,
                        const std::shared_ptr<Config>& config){
    auto type = polymorph.valueType;
    if(type->name == oatpp::data::mapping::type::__class::AbstractObject::CLASS_NAME) {
      writeValue(stream, oatpp::data::mapping::type::static_wrapper_cast<Object>(polymorph), config);
    } else if(type->name == oatpp::data::mapping::type::__class::AbstractListMap::CLASS_NAME) {
      writeValue(stream, oatpp::data::mapping::type::static_wrapper_cast<AbstractFieldsMap>(polymorph), config);
    } else {
      throw std::runtime_error("[oatpp::parser::json::mapping::DtoToMongo::serialize()]: Unknown parameter type");
    }
  }
  
};
  
}}}}

#endif /*oatpp_parser_mongocxx_mapping_Serializer_hpp*/
