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

#ifndef oatpp_mongo_bson_Types_hpp
#define oatpp_mongo_bson_Types_hpp

#include "oatpp/core/Types.hpp"

namespace oatpp { namespace mongo { namespace bson {

/**
 * Enum with bson data-type codes.
 */
enum Types : v_char8 {

  /**
   * 64-bit binary floating point.
   */
  DOUBLE = 0x01,

  /**
   * UTF-8 string.
   */
  STRING = 0x02,

  /**
   * Embedded document.
   */
  DOCUMENT_EMBEDDED = 0x03,

  /**
   * Array.
   */
  DOCUMENT_ARRAY = 0x04,

  /**
   * Binary data.
   */
  BINARY = 0x05,

  /**
   * Undefined (value) — Deprecated.
   */
  UNDEFINED = 0x06,

  /**
   * ObjectId
   */
  OBJECT_ID = 0x07,

  /**
   * Boolean value.
   */
  BOOLEAN = 0x08,

  /**
   * Int64 UTC datetime.
   */
  DATE_TIME = 0x09,

  /**
   * Null value.
   */
  NULL_VALUE = 0x0A,

  /**
   * Regular expression.
   */
  REGEXP = 0x0B,

  /**
   * DBPointer — Deprecated.
   */
  BD_POINTER = 0x0C,

  /**
   * JavaScript code.
   */
  JAVASCRIPT_CODE = 0x0D,

  /**
   * Symbol. Deprecated.
   */
  SYMBOL = 0x0E,

  /**
   * JavaScript code w/ scope.
   */
  JAVASCRIPT_CODE_WS = 0x0F,

  /**
   * 32-bit integer.
   */
  INT_32 = 0x10,

  /**
   * Timestamp (uint64).
   */
  TIMESTAMP = 0x11,

  /**
   * 64-bit integer.
   */
  INT_64 = 0x12,

  /**
   * 128-bit decimal floating point.
   */
  DECIMAL_128 = 0x13,

  /**
   * Min key.
   */
  MIN_KEY = 0xFF,

  /**
   * Max key
   */
  MAX_KEY = 0x7F

};

}}}

#endif // oatpp_mongo_bson_Types_hpp
