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

#include "Utils.hpp"

#include <iostream>

namespace oatpp { namespace mongo { namespace bson {

Utils::BO_TYPE Utils::INT_BO = detectIntBO();
Utils::BO_TYPE Utils::FLOAT_BO = detectFloatBO();

Utils::BO_TYPE Utils::detectIntBO() {
  BO_TYPE result = BO_TYPE::UNKNOWN;
  BO_CHECK check {.i64 = 255};
  if(check.bytes[0] == 255) {
    result = BO_TYPE::LITTLE;
  } else if(check.bytes[7] == 255) {
    result = BO_TYPE::NETWORK;
  }
  return result;
}

Utils::BO_TYPE Utils::detectFloatBO() {
  BO_TYPE result = BO_TYPE::UNKNOWN;
  BO_CHECK check {.f64 = 2.0};
  if(check.bytes[0] > 0) {
    result = BO_TYPE::NETWORK;
  } else if(check.bytes[7] > 0) {
    result = BO_TYPE::LITTLE;
  }
  return result;
}

void Utils::writeKey(data::stream::ConsistentOutputStream *stream, TypeCode typeCode, const data::share::StringKeyLabel &key) {
  if (key) {
    stream->writeCharSimple(typeCode);
    stream->writeSimple(key.getData(), key.getSize());
    stream->writeCharSimple(0);
  }
}

oatpp::String Utils::readKey(parser::Caret& caret, v_char8& typeCode) {
  typeCode = *caret.getCurrData();
  caret.inc();
  auto label = caret.putLabel();
  if(caret.findChar(0)) {
    label.end();
    caret.inc();
    return label.toString();
  }

  caret.setError("[oatpp::mongo::bson::Utils::readKey()]: Error. Unterminated cstring.");
  return nullptr;
}

void Utils::writeInt32(data::stream::ConsistentOutputStream *stream, v_int32 value, BO_TYPE valueBO) {

  switch(valueBO) {

    case BO_TYPE::LITTLE:
      stream->writeSimple(&value, 4);
      break;

    default: {
      v_uint8 data[4];
      data[0] = (v_uint8) (0xFF & value);
      data[1] = (v_uint8) (0xFF & (value >> 8));
      data[2] = (v_uint8) (0xFF & (value >> 16));
      data[3] = (v_uint8) (0xFF & (value >> 24));
      stream->writeSimple(data, 4);
    }

  }

}

v_int32 Utils::readInt32(parser::Caret& caret, BO_TYPE valueBO) {

  if(caret.getDataSize() - caret.getPosition() < 4) {
    caret.setError("[oatpp::mongo::bson::Utils::readInt32()]: Error. Invalid Int32 value.");
    return 0;
  }

  v_int32 result;

  switch(valueBO) {

    case BO_TYPE::LITTLE:
      result = *((p_int32) caret.getCurrData());
      break;

    default: {
      p_char8 data = caret.getCurrData();
      result = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
    }

  }

  caret.inc(4);
  return result;

}

void Utils::writeInt64(data::stream::ConsistentOutputStream *stream, v_int64 value, BO_TYPE valueBO) {
  switch(valueBO) {

    case BO_TYPE::LITTLE:
      stream->writeSimple(&value, 8);
      break;

    default: {
      v_uint8 data[8];
      data[0] = (v_uint8) (0xFF & value);
      data[1] = (v_uint8) (0xFF & (value >> 8));
      data[2] = (v_uint8) (0xFF & (value >> 16));
      data[3] = (v_uint8) (0xFF & (value >> 24));

      data[4] = (v_uint8) (0xFF & (value >> 32));
      data[5] = (v_uint8) (0xFF & (value >> 40));
      data[6] = (v_uint8) (0xFF & (value >> 48));
      data[7] = (v_uint8) (0xFF & (value >> 56));
      stream->writeSimple(data, 8);
    }

  }
}

void Utils::writeUInt64(data::stream::ConsistentOutputStream *stream, v_uint64 value, BO_TYPE valueBO) {
  switch(valueBO) {

    case BO_TYPE::LITTLE:
      stream->writeSimple(&value, 8);
      break;

    default: {
      v_uint8 data[8];
      data[0] = (v_uint8) (0xFF & value);
      data[1] = (v_uint8) (0xFF & (value >> 8));
      data[2] = (v_uint8) (0xFF & (value >> 16));
      data[3] = (v_uint8) (0xFF & (value >> 24));

      data[4] = (v_uint8) (0xFF & (value >> 32));
      data[5] = (v_uint8) (0xFF & (value >> 40));
      data[6] = (v_uint8) (0xFF & (value >> 48));
      data[7] = (v_uint8) (0xFF & (value >> 56));
      stream->writeSimple(data, 8);
    }

  }
}

void Utils::writeFloat64(data::stream::ConsistentOutputStream *stream, v_float64 value, BO_TYPE valueBO) {

  switch(valueBO) {

    case BO_TYPE::LITTLE:
      stream->writeSimple(&value, 8);
      break;

    case BO_TYPE::NETWORK: {
      p_char8 v = (p_char8) &value;
      v_char8 vi[8] = {v[7], v[6], v[5], v[4], v[3], v[2], v[1], v[0]};
      stream->writeSimple(vi, 8);
      break;
    }

    default: {
      throw std::runtime_error("[oatpp::mongo::bson::Utils::writeFloat64()]: Error. Can't serialize float64. Value has unknown byte-order.");
    }

  }

}

void Utils::writePrimitive(data::stream::ConsistentOutputStream *stream, const data::share::StringKeyLabel &key, v_int8 value) {
  writeKey(stream, TypeCode::INT_32, key);
  writeInt32(stream, value);
}

void Utils::writePrimitive(data::stream::ConsistentOutputStream *stream, const data::share::StringKeyLabel &key, v_uint8 value) {
  writeKey(stream, TypeCode::INT_32, key);
  writeInt32(stream, value);
}

void Utils::writePrimitive(data::stream::ConsistentOutputStream *stream, const data::share::StringKeyLabel &key, v_int16 value) {
  writeKey(stream, TypeCode::INT_32, key);
  writeInt32(stream, value);
}

void Utils::writePrimitive(data::stream::ConsistentOutputStream *stream, const data::share::StringKeyLabel &key, v_uint16 value) {
  writeKey(stream, TypeCode::INT_32, key);
  writeInt32(stream, value);
}

void Utils::writePrimitive(data::stream::ConsistentOutputStream *stream, const data::share::StringKeyLabel &key, v_int32 value) {
  writeKey(stream, TypeCode::INT_32, key);
  writeInt32(stream, value);
}

void Utils::writePrimitive(data::stream::ConsistentOutputStream *stream, const data::share::StringKeyLabel &key, v_uint32 value) {
  writeKey(stream, TypeCode::INT_64, key);
  writeInt64(stream, value);
}

void Utils::writePrimitive(data::stream::ConsistentOutputStream *stream, const data::share::StringKeyLabel &key, v_int64 value) {
  writeKey(stream, TypeCode::INT_64, key);
  writeInt64(stream, value);
}

void Utils::writePrimitive(data::stream::ConsistentOutputStream *stream, const data::share::StringKeyLabel &key, v_uint64 value) {
  writeKey(stream, TypeCode::TIMESTAMP, key);
  writeUInt64(stream, value);
}

void Utils::writePrimitive(data::stream::ConsistentOutputStream *stream, const data::share::StringKeyLabel &key, v_float32 value) {
  writeKey(stream, TypeCode::DOUBLE, key);
  writeFloat64(stream, value);
}

void Utils::writePrimitive(data::stream::ConsistentOutputStream *stream, const data::share::StringKeyLabel &key, v_float64 value) {
  writeKey(stream, TypeCode::DOUBLE, key);
  writeFloat64(stream, value);
}

void Utils::writePrimitive(data::stream::ConsistentOutputStream *stream, const data::share::StringKeyLabel &key, bool value) {
  writeKey(stream, TypeCode::BOOLEAN, key);
  if(value) {
    stream->writeCharSimple(1);
  } else {
    stream->writeCharSimple(0);
  }
}

}}}
