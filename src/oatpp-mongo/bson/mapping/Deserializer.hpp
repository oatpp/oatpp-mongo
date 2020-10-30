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

  template<class Collection>
  static oatpp::Void deserializeArray(Deserializer* deserializer,
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
          caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeList()]: Error. Invalid document size.");
          return nullptr;
        }

        parser::Caret innerCaret(caret.getCurrData(), docSize - 4);

        auto polymorphicDispatcher = static_cast<const typename Collection::Class::PolymorphicDispatcher*>(type->polymorphicDispatcher);
        auto listWrapper = polymorphicDispatcher->createObject();
        const auto& list = listWrapper.template staticCast<Collection>();

        const Type* itemType = *type->params.begin();
        v_int32 expectedIndex = 0;
        while(innerCaret.canContinue() && innerCaret.getPosition() < innerCaret.getDataSize() - 1) {

          v_char8 valueTypeCode;
          auto key = Utils::readKey(innerCaret, valueTypeCode);
          if(innerCaret.hasError()){
            caret.inc(innerCaret.getPosition());
            caret.setError(innerCaret.getErrorMessage(), innerCaret.getErrorCode());
            return nullptr;
          }

          bool success;
          v_int32 keyIntValue = utils::conversion::strToInt32(key, success);
          if(!success || keyIntValue != expectedIndex) {
            caret.inc(innerCaret.getPosition());
            caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeList()]: Error. Array invalid index value. Looks like it's not an array.");
            return nullptr;
          }

          auto item = deserializer->deserialize(innerCaret, itemType, valueTypeCode);
          if(innerCaret.hasError()){
            caret.inc(innerCaret.getPosition());
            caret.setError(innerCaret.getErrorMessage(), innerCaret.getErrorCode());
            return nullptr;
          }

          polymorphicDispatcher->addPolymorphicItem(listWrapper, item);
          ++ expectedIndex;

        }

        if(!innerCaret.canContinueAtChar(0, 1)){
          caret.inc(innerCaret.getPosition());
          if(innerCaret.hasError()) {
            caret.setError(innerCaret.getErrorMessage(), innerCaret.getErrorCode());
          } else {
            caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeList()]: Error. '\\0' - expected");
          }
          return nullptr;
        }

        if(innerCaret.getPosition() != innerCaret.getDataSize()) {
          caret.inc(innerCaret.getPosition());
          caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeList()]: Error. Document parsing failed.");
        }

        caret.inc(innerCaret.getPosition());
        return oatpp::Void(list.getPtr(), list.valueType);

      }

      default:
        caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeList()]: Error. Invalid type code.");
        return nullptr;
    }

  }

  template<class Collection>
  static oatpp::Void deserializeKeyValue(Deserializer* deserializer,
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
          caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeFieldsMap()]: Error. Invalid document size.");
          return nullptr;
        }

        parser::Caret innerCaret(caret.getCurrData(), docSize - 4);

        auto polymorphicDispatcher = static_cast<const typename Collection::Class::PolymorphicDispatcher*>(type->polymorphicDispatcher);
        auto mapWrapper = polymorphicDispatcher->createObject();
        const auto& map = mapWrapper.template staticCast<Collection>();

        auto it = type->params.begin();
        const Type* keyType = *it ++;
        if(keyType->classId.id != oatpp::data::mapping::type::__class::String::CLASS_ID.id){
          throw std::runtime_error("[oatpp::mongo::bson::mapping::Deserializer::deserializeFieldsMap()]: Invalid bson map key. Key should be String");
        }

        const Type* valueType = *it;
        while(innerCaret.canContinue() && innerCaret.getPosition() < innerCaret.getDataSize() - 1) {

          v_char8 valueTypeCode;
          auto key = Utils::readKey(innerCaret, valueTypeCode);
          if(innerCaret.hasError()){
            caret.inc(innerCaret.getPosition());
            caret.setError(innerCaret.getErrorMessage(), innerCaret.getErrorCode());
            return nullptr;
          }

          polymorphicDispatcher->addPolymorphicItem(mapWrapper, key, deserializer->deserialize(innerCaret, valueType, valueTypeCode));

        }

        if(!innerCaret.canContinueAtChar(0, 1)){
          caret.inc(innerCaret.getPosition());
          if(innerCaret.hasError()) {
            caret.setError(innerCaret.getErrorMessage(), innerCaret.getErrorCode());
          } else {
            caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeFieldsMap()]: Error. '\\0' - expected");
          }
          return nullptr;
        }

        if(innerCaret.getPosition() != innerCaret.getDataSize()) {
          caret.inc(innerCaret.getPosition());
          caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeFieldsMap()]: Error. Document parsing failed.");
        }

        caret.inc(innerCaret.getPosition());
        return oatpp::Void(map.getPtr(), map.valueType);

      }

      default:
        caret.setError("[oatpp::mongo::bson::mapping::Deserializer::deserializeFieldsMap()]: Error. Invalid type code.");
        return nullptr;
    }

  }

  static oatpp::Void deserializeBoolean(Deserializer* deserializer, parser::Caret& caret, const Type* const type, v_char8 bsonTypeCode);
  static oatpp::Void deserializeDateTime(Deserializer* deserializer, parser::Caret& caret, const Type* const type, v_char8 bsonTypeCode);
  static oatpp::Void deserializeString(Deserializer* deserializer, parser::Caret& caret, const Type* const type, v_char8 bsonTypeCode);

  static oatpp::Void deserializeInlineDocs(Deserializer* deserializer, parser::Caret& caret, const Type* const type, v_char8 bsonTypeCode);

  static oatpp::Void deserializeObjectId(Deserializer* deserializer, parser::Caret& caret, const Type* const type, v_char8 bsonTypeCode);

  static oatpp::Void deserializeAny(Deserializer* deserializer, parser::Caret& caret, const Type* const type, v_char8 bsonTypeCode);
  static oatpp::Void deserializeEnum(Deserializer* deserializer, parser::Caret& caret, const Type* const type, v_char8 bsonTypeCode);
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
