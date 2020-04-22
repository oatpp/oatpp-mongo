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

#include "oatpp/core/data/mapping/type/ListMap.hpp"
#include "oatpp/core/data/mapping/type/List.hpp"
#include "oatpp/core/data/mapping/type/Object.hpp"
#include "oatpp/core/data/mapping/type/Primitive.hpp"
#include "oatpp/core/data/mapping/type/Type.hpp"

#include "oatpp/core/data/stream/Stream.hpp"
#include "oatpp/core/collection/LinkedList.hpp"
#include "oatpp/core/Types.hpp"

#include <vector>

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
                                   const data::mapping::type::AbstractObjectWrapper&);
private:

  template<class T>
  static void serializePrimitive(Serializer* serializer,
                                 data::stream::ConsistentOutputStream* stream,
                                 const data::share::StringKeyLabel& key,
                                 const data::mapping::type::AbstractObjectWrapper& polymorph){
    (void) serializer;

    if(!key) {
      throw std::runtime_error("[oatpp::mongo::bson::mapping::Serializer::serializePrimitive()]: Error. The key can't be null.");
    }

    if(polymorph) {
      auto str = static_cast<oatpp::base::StrBuffer *>(polymorph.get());
      bson::Utils::writePrimitive(stream, key, static_cast<typename T::ObjectType*>(polymorph.get())->getValue());
    } else {
      bson::Utils::writeKey(stream, TypeCode::NULL_VALUE, key);
    }
  }

  static void serializeString(Serializer* serializer,
                              data::stream::ConsistentOutputStream* stream,
                              const data::share::StringKeyLabel& key,
                              const data::mapping::type::AbstractObjectWrapper& polymorph);

  static void serializeInlineDocs(Serializer* serializer,
                                  data::stream::ConsistentOutputStream* stream,
                                  const data::share::StringKeyLabel& key,
                                  TypeCode typeCode,
                                  const data::mapping::type::AbstractObjectWrapper& polymorph);

  static void serializeInlineDocument(Serializer* serializer,
                                      data::stream::ConsistentOutputStream* stream,
                                      const data::share::StringKeyLabel& key,
                                      const data::mapping::type::AbstractObjectWrapper& polymorph);

  static void serializeInlineArray(Serializer* serializer,
                                   data::stream::ConsistentOutputStream* stream,
                                   const data::share::StringKeyLabel& key,
                                   const data::mapping::type::AbstractObjectWrapper& polymorph);

  static void serializeList(Serializer* serializer,
                            data::stream::ConsistentOutputStream* stream,
                            const data::share::StringKeyLabel& key,
                            const data::mapping::type::AbstractObjectWrapper& polymorph);

  static void serializeFieldsMap(Serializer* serializer,
                                 data::stream::ConsistentOutputStream* stream,
                                 const data::share::StringKeyLabel& key,
                                 const data::mapping::type::AbstractObjectWrapper& polymorph);

  static void serializeObject(Serializer* serializer,
                              data::stream::ConsistentOutputStream* stream,
                              const data::share::StringKeyLabel& key,
                              const data::mapping::type::AbstractObjectWrapper& polymorph);

  void serialize(data::stream::ConsistentOutputStream* stream,
                 const data::share::StringKeyLabel& key,
                 const data::mapping::type::AbstractObjectWrapper& polymorph);

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
   * @param method - `typedef void (*SerializerMethod)(Serializer*, data::stream::ConsistentOutputStream*, const data::mapping::type::AbstractObjectWrapper&)`.
   */
  void setSerializerMethod(const data::mapping::type::ClassId& classId, SerializerMethod method);

  /**
   * Serialize object to stream.
   * @param stream - &id:oatpp::data::stream::ConsistentOutputStream;.
   * @param polymorph - DTO as &id:oatpp::data::mapping::type::AbstractObjectWrapper;.
   */
  void serializeToStream(data::stream::ConsistentOutputStream* stream, const data::mapping::type::AbstractObjectWrapper& polymorph);

  /**
   * Get serializer config.
   * @return
   */
  const std::shared_ptr<Config>& getConfig();

};

}}}}

#endif /* oatpp_mongo_bson_mapping_Serializer_hpp */
