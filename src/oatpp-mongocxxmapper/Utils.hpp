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

#ifndef oatpp_mongo_bson_Utils_hpp
#define oatpp_mongo_bson_Utils_hpp

#include "oatpp/core/data/share/MemoryLabel.hpp"
#include "oatpp/core/data/stream/Stream.hpp"

namespace oatpp { namespace mongo { namespace bson {

class Utils {
public:

  enum BO_TYPE : v_int32 {
    UNKNOWN = 0,
    LITTLE = 1,
    NETWORK = 2
  };

  union BO_CHECK {
    v_uint64 i64;
    v_float64 f64;
    v_char8 bytes[8];
  };

  static BO_TYPE detectIntBO();
  static BO_TYPE detectFloatBO();

public:

  static BO_TYPE INT_BO;
  static BO_TYPE FLOAT_BO;

public:

  static void writeKey(data::stream::ConsistentOutputStream *stream, v_char8 dataType, const data::share::StringKeyLabel &key);

  static void writeInt32(data::stream::ConsistentOutputStream *stream, v_int32 value, BO_TYPE valueBO = INT_BO);

  static void writeInt64(data::stream::ConsistentOutputStream *stream, v_int64 value, BO_TYPE valueBO = INT_BO);

  static void writeUInt64(data::stream::ConsistentOutputStream *stream, v_uint64 value, BO_TYPE valueBO = INT_BO);

  static void writeFloat64(data::stream::ConsistentOutputStream *stream, v_float64 value, BO_TYPE valueBO = FLOAT_BO);
  
  static void writePrimitive(data::stream::ConsistentOutputStream *stream, const data::share::StringKeyLabel &key, v_int8 value);
  
  static void writePrimitive(data::stream::ConsistentOutputStream *stream, const data::share::StringKeyLabel &key, v_uint8 value);
  
  static void writePrimitive(data::stream::ConsistentOutputStream *stream, const data::share::StringKeyLabel &key, v_int16 value);
  
  static void writePrimitive(data::stream::ConsistentOutputStream *stream, const data::share::StringKeyLabel &key, v_uint16 value);
  
  static void writePrimitive(data::stream::ConsistentOutputStream *stream, const data::share::StringKeyLabel &key, v_int32 value);
  
  static void writePrimitive(data::stream::ConsistentOutputStream *stream, const data::share::StringKeyLabel &key, v_uint32 value);
  
  static void writePrimitive(data::stream::ConsistentOutputStream *stream, const data::share::StringKeyLabel &key, v_int64 value);
  
  static void writePrimitive(data::stream::ConsistentOutputStream *stream, const data::share::StringKeyLabel &key, v_uint64 value);
  
  static void writePrimitive(data::stream::ConsistentOutputStream *stream, const data::share::StringKeyLabel &key, v_float32 value);
  
  static void writePrimitive(data::stream::ConsistentOutputStream *stream, const data::share::StringKeyLabel &key, v_float64 value);
  
  static void writePrimitive(data::stream::ConsistentOutputStream *stream, const data::share::StringKeyLabel &key, bool value);


};

}}}

#endif //oatpp_mongo_bson_Utils_hpp
