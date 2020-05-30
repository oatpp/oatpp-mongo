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

#ifndef oatpp_mongo_bson_type_ObjectId_hpp
#define oatpp_mongo_bson_type_ObjectId_hpp

#include "oatpp/core/Types.hpp"
#include <atomic>

namespace oatpp { namespace mongo { namespace bson { namespace type {

/**
 * BSON ObjectId implementation.
 */
class ObjectId : public oatpp::base::Countable {
private:
  static const std::string PROCESS_UNIQUE;
  static std::atomic<v_uint64> COUNTER;
  static std::string seedProcessUnique();
  static v_uint64 seedCounter();
public:
  /**
   * Size of ObjectId data.
   */
  static constexpr v_buff_size DATA_SIZE = 12;
private:
  v_char8 m_data[DATA_SIZE];
public:

  /**
   * Constructor. Creates new ObjectId.
   */
  ObjectId();

  /**
   * Constructor. Creates ObjectId from byte array.
   * @param m_data
   */
  ObjectId(v_char8 m_data[DATA_SIZE]);

  /**
   * Get raw data of ObjectId.
   * @return
   */
  const p_char8 getData() const;

  /**
   * Get size of ObjectId data.
   * @return - &l:ObjectId::DATA_SIZE;.
   */
  v_buff_size getSize() const;

  /**
   * Get ObjectId timestamp.
   * @return
   */
  v_uint32 getTimestamp() const;

  /**
   * To hex string.
   * @return
   */
  oatpp::String toString() const;

  bool operator==(const ObjectId &other) const;
  bool operator!=(const ObjectId &other) const;

};

}}}}

#endif // oatpp_mongo_bson_type_ObjectId_hpp
