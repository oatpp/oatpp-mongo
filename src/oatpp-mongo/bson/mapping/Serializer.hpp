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

#ifndef oatpp_mongo_bson_mapping_Serializer_hpp
#define oatpp_mongo_bson_mapping_Serializer_hpp

#include "oatpp-mongo/bson/Utils.hpp"
#include "oatpp-mongo/bson/Types.hpp"

#include "oatpp/core/data/share/MemoryLabel.hpp"
#include "oatpp/core/data/stream/BufferStream.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"
#include "oatpp/core/Types.hpp"

namespace oatpp { namespace mongo { namespace bson { namespace mapping {

/**
 * Json Serializer.
 * Serializes oatpp DTO object to bson. See [Data Transfer Object(DTO) component](https://oatpp.io/docs/components/dto/).
 */
class Serializer {
public:
  typedef oatpp::data::mapping::type::Type Type;
  typedef oatpp::data::mapping::type::Type::Property Property;
  typedef oatpp::data::mapping::type::Type::Properties Properties;
public:
  /**
   * Serializer config.
   */
  class Config : public oatpp::base::Countable {
  public:
    /**
     * Constructor.
     */
    Config()
    {}
  public:

    /**
     * Create shared config.
     * @return - `std::shared_ptr` to Config.
     */
    static std::shared_ptr<Config> createShared(){
      return std::make_shared<Config>();
    }

    /**
     * Include fields with value == nullptr into serialized bson.
     */
    bool includeNullFields = true;

    /**
     * If `true` - insert string `"<unknown-type>"` in bson field value in case unknown field found.
     * Fail if `false`.
     * Known types for this serializer are:<br>
     * (String, Int8, Int16, Int32, Int64, Float32, Float64, Boolean, DTOs, List, Fields).
     */
    bool throwOnUnknownTypes = true;

  };
public:
  typedef void (*SerializerMethod)(Serializer*,
                                   data::stream::ConsistentOutputStream*,
                                   const data::share::StringKeyLabel& key,
                                   const oatpp::Void&);
private:

  template<class T>
  static void serializePrimitive(Serializer* serializer,
                                 data::stream::ConsistentOutputStream* stream,
                                 const data::share::StringKeyLabel& key,
                                 const oatpp::Void& polymorph)
  {
    (void) serializer;

    if(!key) {
      throw std::runtime_error("[oatpp::mongo::bson::mapping::Serializer::serializePrimitive()]: Error. The key can't be null.");
    }

    if(polymorph) {
      auto str = static_cast<oatpp::base::StrBuffer *>(polymorph.get());
      bson::Utils::writePrimitive(stream, key, * static_cast<typename T::ObjectType*>(polymorph.get()));
    } else {
      bson::Utils::writeKey(stream, TypeCode::NULL_VALUE, key);
    }
  }

  template<class Collection>
  static void serializeArray(Serializer* serializer,
                             data::stream::ConsistentOutputStream* stream,
                             const data::share::StringKeyLabel& key,
                             const oatpp::Void& polymorph)
  {

    if(polymorph) {

      bson::Utils::writeKey(stream, TypeCode::DOCUMENT_ARRAY, key);

      data::stream::BufferOutputStream innerStream;

      const auto& list = polymorph.staticCast<Collection>();
      v_int32 index = 0;

      for(auto& value : *list) {
        if (value || serializer->getConfig()->includeNullFields) {
          serializer->serialize(&innerStream, utils::conversion::int32ToStr(index), value);
          index ++;
        }
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

  template<class Collection>
  static void serializeKeyValue(Serializer* serializer,
                                data::stream::ConsistentOutputStream* stream,
                                const data::share::StringKeyLabel& key,
                                const oatpp::Void& polymorph)
  {

    if(polymorph) {

      bson::Utils::writeKey(stream, TypeCode::DOCUMENT_EMBEDDED, key);

      data::stream::BufferOutputStream innerStream;

      const auto& map = polymorph.staticCast<Collection>();

      for(auto& pair : *map) {
        const auto& value = pair.second;
        if(value || serializer->getConfig()->includeNullFields) {
          const auto& key = pair.first;
          serializer->serialize(&innerStream, key, value);
        }
      }

      bson::Utils::writeInt32(stream, innerStream.getCurrentPosition() + 5);
      stream->writeSimple(innerStream.getData(), innerStream.getCurrentPosition());
      stream->writeCharSimple(0);

    } else if(key) {
      bson::Utils::writeKey(stream, TypeCode::NULL_VALUE, key);
    } else {
      throw std::runtime_error("[oatpp::mongo::bson::mapping::Serializer::serializeKeyValue()]: Error. null object with null key.");
    }

  }

  static void serializeString(Serializer* serializer,
                              data::stream::ConsistentOutputStream* stream,
                              const data::share::StringKeyLabel& key,
                              const oatpp::Void& polymorph);

  static void serializeInlineDocs(Serializer* serializer,
                                  data::stream::ConsistentOutputStream* stream,
                                  const data::share::StringKeyLabel& key,
                                  TypeCode typeCode,
                                  const oatpp::Void& polymorph);

  static void serializeInlineDocument(Serializer* serializer,
                                      data::stream::ConsistentOutputStream* stream,
                                      const data::share::StringKeyLabel& key,
                                      const oatpp::Void& polymorph);

  static void serializeInlineArray(Serializer* serializer,
                                   data::stream::ConsistentOutputStream* stream,
                                   const data::share::StringKeyLabel& key,
                                   const oatpp::Void& polymorph);

  static void serializeObjectId(Serializer* serializer,
                                data::stream::ConsistentOutputStream* stream,
                                const data::share::StringKeyLabel& key,
                                const oatpp::Void& polymorph);

  static void serializeAny(Serializer* serializer,
                           data::stream::ConsistentOutputStream* stream,
                           const data::share::StringKeyLabel& key,
                           const oatpp::Void& polymorph);

  static void serializeEnum(Serializer* serializer,
                            data::stream::ConsistentOutputStream* stream,
                            const data::share::StringKeyLabel& key,
                            const oatpp::Void& polymorph);

  static void serializeObject(Serializer* serializer,
                              data::stream::ConsistentOutputStream* stream,
                              const data::share::StringKeyLabel& key,
                              const oatpp::Void& polymorph);

  void serialize(data::stream::ConsistentOutputStream* stream,
                 const data::share::StringKeyLabel& key,
                 const oatpp::Void& polymorph);

private:
  std::shared_ptr<Config> m_config;
  std::vector<SerializerMethod> m_methods;
public:

  /**
   * Constructor.
   * @param config - serializer config.
   */
  Serializer(const std::shared_ptr<Config>& config = std::make_shared<Config>());

  /**
   * Set serializer method for type.
   * @param classId - &id:oatpp::data::mapping::type::ClassId;.
   * @param method - `typedef void (*SerializerMethod)(Serializer*, data::stream::ConsistentOutputStream*, const oatpp::Void&)`.
   */
  void setSerializerMethod(const data::mapping::type::ClassId& classId, SerializerMethod method);

  /**
   * Serialize object to stream.
   * @param stream - &id:oatpp::data::stream::ConsistentOutputStream;.
   * @param polymorph - DTO as &id:oatpp::Void;.
   */
  void serializeToStream(data::stream::ConsistentOutputStream* stream, const oatpp::Void& polymorph);

  /**
   * Get serializer config.
   * @return
   */
  const std::shared_ptr<Config>& getConfig();

};

}}}}

#endif /* oatpp_mongo_bson_mapping_Serializer_hpp */
