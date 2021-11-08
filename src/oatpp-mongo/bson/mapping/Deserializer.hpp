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

#ifndef oatpp_mongo_bson_mapping_Deserializer_hpp
#define oatpp_mongo_bson_mapping_Deserializer_hpp

#include "oatpp-mongo/bson/Utils.hpp"

#include "oatpp/core/parser/Caret.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"
#include "oatpp/core/Types.hpp"

namespace oatpp { namespace mongo { namespace bson { namespace mapping {

/**
 * BSON Deserializer.
 * Deserialize oatpp DTO object from bson. See [Data Transfer Object(DTO) component](https://oatpp.io/docs/components/dto/).
 */
class Deserializer {
public:
  typedef oatpp::BaseObject::Property Property;
  typedef oatpp::BaseObject::Properties Properties;
public:

  /**
   * Deserializer config.
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
     * Create shared Config.
     * @return - `std::shared_ptr` to Config.
     */
    static std::shared_ptr<Config> createShared(){
      return std::make_shared<Config>();
    }

    /**
     * Do not fail if unknown field is found in bson.
     * "unknown field" is the one which is not present in DTO object class.
     */
    bool allowUnknownFields = true;

    /**
     * Enable type interpretations.
     */
    std::vector<std::string> enableInterpretations = {};

  };

public:
  typedef oatpp::Void (*DeserializerMethod)(Deserializer*, parser::Caret&, const Type* const, v_char8 bsonTypeCode);
private:
  struct PolymorphData {
    oatpp::BaseObject::Property* field;
    oatpp::String unparsedData;
    v_char8 valueType;
  };
private:
  static void skipCString(parser::Caret& caret);
  static void skipSizedElement(parser::Caret& caret, v_int32 additionalBytes = 0);
  static void skipElement(parser::Caret& caret, v_char8 bsonTypeCode);
  static const Type* guessType(v_char8 bsonTypeCode);
private:

  template<class T>
  static oatpp::Void deserializePrimitive(Deserializer* deserializer, parser::Caret& caret, const Type* const type, v_char8 bsonTypeCode){

    (void) deserializer;
    (void) type;

    if(bsonTypeCode == TypeCode::NULL_VALUE) {
      return oatpp::Void(T::Class::getType());
    }

    typename T::ObjectType value;
    Utils::readPrimitive(caret, value, bsonTypeCode);
    return T(value);

  }

  static oatpp::Void deserializeBoolean(Deserializer* deserializer, parser::Caret& caret, const Type* const type, v_char8 bsonTypeCode);
  static oatpp::Void deserializeDateTime(Deserializer* deserializer, parser::Caret& caret, const Type* const type, v_char8 bsonTypeCode);
  static oatpp::Void deserializeString(Deserializer* deserializer, parser::Caret& caret, const Type* const type, v_char8 bsonTypeCode);

  static oatpp::Void deserializeInlineDocs(Deserializer* deserializer, parser::Caret& caret, const Type* const type, v_char8 bsonTypeCode);

  static oatpp::Void deserializeObjectId(Deserializer* deserializer, parser::Caret& caret, const Type* const type, v_char8 bsonTypeCode);

  static oatpp::Void deserializeAny(Deserializer* deserializer, parser::Caret& caret, const Type* const type, v_char8 bsonTypeCode);
  static oatpp::Void deserializeEnum(Deserializer* deserializer, parser::Caret& caret, const Type* const type, v_char8 bsonTypeCode);

  static oatpp::Void deserializeCollection(Deserializer* deserializer, parser::Caret& caret, const Type* const type, v_char8 bsonTypeCode);
  static oatpp::Void deserializeMap(Deserializer* deserializer, parser::Caret& caret, const Type* const type, v_char8 bsonTypeCode);
  static oatpp::Void deserializeObject(Deserializer* deserializer, parser::Caret& caret, const Type* const type, v_char8 bsonTypeCode);

private:
  std::shared_ptr<Config> m_config;
  std::vector<DeserializerMethod> m_methods;
public:

  /**
   * Constructor.
   * @param config
   */
  Deserializer(const std::shared_ptr<Config>& config = std::make_shared<Config>());

  /**
   * Set deserializer method for type.
   * @param classId - &id:oatpp::data::mapping::type::ClassId;.
   * @param method - `typedef oatpp::Void (*DeserializerMethod)(Deserializer*, parser::Caret&, const Type* const)`.
   */
  void setDeserializerMethod(const data::mapping::type::ClassId& classId, DeserializerMethod method);

  /**
   * Deserialize text.
   * @param caret - &id:oatpp::parser::Caret;.
   * @param type - &id:oatpp::data::mapping::type::Type;
   * @return - `oatpp::Void` over deserialized object.
   */
  oatpp::Void deserialize(parser::Caret& caret, const Type* const type, v_char8 bsonTypeCode);

  /**
   * Get deserializer config.
   * @return
   */
  const std::shared_ptr<Config>& getConfig();

};

}}}}

#endif /* oatpp_mongo_bson_mapping_Deserializer_hpp */
