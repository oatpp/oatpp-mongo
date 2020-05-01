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

oatpp::String Utils::readCString(parser::Caret& caret) {
  auto label = caret.putLabel();
  if(caret.findChar(0)) {
    label.end();
    caret.inc();
    return label.toString();
  }
  caret.setError("[oatpp::mongo::bson::Utils::readCString()]: Error. Unterminated cstring.");
  return nullptr;
}

void Utils::writeKey(ConsistentOutputStream *stream, TypeCode typeCode, const StringKeyLabel &key) {
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

void Utils::writeInt32(ConsistentOutputStream *stream, v_int32 value, BO_TYPE valueBO) {

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

void Utils::writeInt64(ConsistentOutputStream *stream, v_int64 value, BO_TYPE valueBO) {
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

v_int64 Utils::readInt64(parser::Caret& caret, BO_TYPE valueBO) {

  if(caret.getDataSize() - caret.getPosition() < 8) {
    caret.setError("[oatpp::mongo::bson::Utils::readInt64()]: Error. Invalid Int64 value.");
    return 0;
  }

  v_int64 result;

  switch(valueBO) {

    case BO_TYPE::LITTLE:
      result = *((p_int64) caret.getCurrData());
      break;

    default: {
      p_char8 data = caret.getCurrData();
      result = ((v_int64) data[0]      ) | ((v_int64) data[1] <<  8) | ((v_int64) data[2] << 16) | ((v_int64) data[3] << 24) |
               ((v_int64) data[4] << 32) | ((v_int64) data[5] << 40) | ((v_int64) data[6] << 48) | ((v_int64) data[7] << 56);
    }

  }

  caret.inc(8);
  return result;

}

void Utils::writeUInt64(ConsistentOutputStream *stream, v_uint64 value, BO_TYPE valueBO) {
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

v_uint64 Utils::readUInt64(parser::Caret& caret, BO_TYPE valueBO) {

  if(caret.getDataSize() - caret.getPosition() < 8) {
    caret.setError("[oatpp::mongo::bson::Utils::readUInt64()]: Error. Invalid UInt64 value.");
    return 0;
  }

  v_uint64 result;

  switch(valueBO) {

    case BO_TYPE::LITTLE:
      result = *((p_uint64) caret.getCurrData());
      break;

    default: {
      p_char8 data = caret.getCurrData();
      result = ((v_uint64) data[0]      ) | ((v_uint64) data[1] <<  8) | ((v_uint64) data[2] << 16) | ((v_uint64) data[3] << 24) |
               ((v_uint64) data[4] << 32) | ((v_uint64) data[5] << 40) | ((v_uint64) data[6] << 48) | ((v_uint64) data[7] << 56);
    }

  }

  caret.inc(8);
  return result;

}

void Utils::writeFloat64(ConsistentOutputStream *stream, v_float64 value, BO_TYPE valueBO) {

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

v_float64 Utils::readFloat64(parser::Caret& caret, BO_TYPE valueBO) {

  if(caret.getDataSize() - caret.getPosition() < 8) {
    caret.setError("[oatpp::mongo::bson::Utils::readFloat64()]: Error. Invalid Float64 value.");
    return 0;
  }

  v_float64 result;

  switch(valueBO) {

    case BO_TYPE::LITTLE:
      result = *((p_float64) caret.getCurrData());
      break;

    case BO_TYPE::NETWORK: {

      p_float64 rp = &result;
      p_float64 vp = (p_float64) caret.getCurrData();

      rp[7] = vp[0];
      rp[6] = vp[1];
      rp[5] = vp[2];
      rp[4] = vp[3];
      rp[3] = vp[4];
      rp[2] = vp[5];
      rp[1] = vp[6];
      rp[0] = vp[7];

      break;

    }

    default: {
      throw std::runtime_error("[oatpp::mongo::bson::Utils::readFloat64()]: Error. Can't deserialize float64. Host has unknown byte-order.");
    }

  }

  caret.inc(8);
  return result;

}

void Utils::writePrimitive(ConsistentOutputStream *stream, const StringKeyLabel &key, v_int8 value) {
  writeKey(stream, TypeCode::INT_32, key);
  writeInt32(stream, value);
}

void Utils::readPrimitive(parser::Caret& caret, v_int8& value, v_char8 bsonTypeCode) {
  if(bsonTypeCode == TypeCode::INT_32) {
    if(!checkLimitsAndAssign(value, readInt32(caret))){
      caret.setError("[oatpp::mongo::bson::Utils::readPrimitive()]: Error. Document value is out of the range bounds of Int8");
    }
  } else {
    caret.setError("[oatpp::mongo::bson::Utils::readPrimitive()]: Error. Can't deserialize Int8 value. BSON document value type is expected to be Int32.");
  }
}

void Utils::writePrimitive(ConsistentOutputStream *stream, const StringKeyLabel &key, v_uint8 value) {
  writeKey(stream, TypeCode::INT_32, key);
  writeInt32(stream, value);
}

void Utils::readPrimitive(parser::Caret& caret, v_uint8& value, v_char8 bsonTypeCode) {
  if(bsonTypeCode == TypeCode::INT_32) {
    if(!checkLimitsAndAssign(value, readInt32(caret))){
      caret.setError("[oatpp::mongo::bson::Utils::readPrimitive()]: Error. Document value is out of the range bounds of UInt8");
    }
  } else {
    caret.setError("[oatpp::mongo::bson::Utils::readPrimitive()]: Error. Can't deserialize UInt8 value. BSON document value type is expected to be Int32.");
  }
}

void Utils::writePrimitive(ConsistentOutputStream *stream, const StringKeyLabel &key, v_int16 value) {
  writeKey(stream, TypeCode::INT_32, key);
  writeInt32(stream, value);
}

void Utils::readPrimitive(parser::Caret& caret, v_int16& value, v_char8 bsonTypeCode) {
  if(bsonTypeCode == TypeCode::INT_32) {
    if(!checkLimitsAndAssign(value, readInt32(caret))){
      caret.setError("[oatpp::mongo::bson::Utils::readPrimitive()]: Error. Document value is out of the range bounds of Int16");
    }
  } else {
    caret.setError("[oatpp::mongo::bson::Utils::readPrimitive()]: Error. Can't deserialize Int16 value. BSON document value type is expected to be Int32.");
  }
}

void Utils::writePrimitive(ConsistentOutputStream *stream, const StringKeyLabel &key, v_uint16 value) {
  writeKey(stream, TypeCode::INT_32, key);
  writeInt32(stream, value);
}

void Utils::readPrimitive(parser::Caret& caret, v_uint16& value, v_char8 bsonTypeCode) {
  if(bsonTypeCode == TypeCode::INT_32) {
    if(!checkLimitsAndAssign(value, readInt32(caret))){
      caret.setError("[oatpp::mongo::bson::Utils::readPrimitive()]: Error. Document value is out of the range bounds of UInt16");
    }
  } else {
    caret.setError("[oatpp::mongo::bson::Utils::readPrimitive()]: Error. Can't deserialize UInt16 value. BSON document value type is expected to be Int32.");
  }
}

void Utils::writePrimitive(ConsistentOutputStream *stream, const StringKeyLabel &key, v_int32 value) {
  writeKey(stream, TypeCode::INT_32, key);
  writeInt32(stream, value);
}

void Utils::readPrimitive(parser::Caret& caret, v_int32& value, v_char8 bsonTypeCode) {
  if(bsonTypeCode == TypeCode::INT_32) {
    value = readInt32(caret);
  } else {
    caret.setError("[oatpp::mongo::bson::Utils::readPrimitive()]: Error. Can't deserialize Int32 value. BSON document value type is expected to be Int32.");
  }
}

void Utils::writePrimitive(ConsistentOutputStream *stream, const StringKeyLabel &key, v_uint32 value) {
  writeKey(stream, TypeCode::INT_64, key);
  writeInt64(stream, value);
}

void Utils::readPrimitive(parser::Caret& caret, v_uint32& value, v_char8 bsonTypeCode) {
  if(bsonTypeCode == TypeCode::INT_64) {
    if(!checkLimitsAndAssign(value, readInt64(caret))){
      caret.setError("[oatpp::mongo::bson::Utils::readPrimitive()]: Error. Document value is out of the range bounds of UInt32");
    }
  } else {
    caret.setError("[oatpp::mongo::bson::Utils::readPrimitive()]: Error. Can't deserialize UInt32 value. BSON document value type is expected to be Int64.");
  }
}

void Utils::writePrimitive(ConsistentOutputStream *stream, const StringKeyLabel &key, v_int64 value) {
  writeKey(stream, TypeCode::INT_64, key);
  writeInt64(stream, value);
}

void Utils::readPrimitive(parser::Caret& caret, v_int64& value, v_char8 bsonTypeCode) {
  if(bsonTypeCode == TypeCode::INT_64) {
    value = readInt64(caret);
  } else {
    caret.setError("[oatpp::mongo::bson::Utils::readPrimitive()]: Error. Can't deserialize Int64 value. BSON document value type is expected to be Int64.");
  }
}

void Utils::writePrimitive(ConsistentOutputStream *stream, const StringKeyLabel &key, v_uint64 value) {
  writeKey(stream, TypeCode::TIMESTAMP, key);
  writeUInt64(stream, value);
}

void Utils::readPrimitive(parser::Caret& caret, v_uint64& value, v_char8 bsonTypeCode) {
  if(bsonTypeCode == TypeCode::TIMESTAMP) {
    value = readUInt64(caret);
  } else {
    caret.setError("[oatpp::mongo::bson::Utils::readPrimitive()]: Error. Can't deserialize UInt64 value. BSON document value type is expected to be TIMESTAMP.");
  }
}

void Utils::writePrimitive(ConsistentOutputStream *stream, const StringKeyLabel &key, v_float32 value) {
  writeKey(stream, TypeCode::DOUBLE, key);
  writeFloat64(stream, value);
}

void Utils::readPrimitive(parser::Caret& caret, v_float32& value, v_char8 bsonTypeCode) {
  if(bsonTypeCode == TypeCode::DOUBLE) {
    if(!checkLimitsAndAssign(value, readFloat64(caret))){
      caret.setError("[oatpp::mongo::bson::Utils::readPrimitive()]: Error. Document value is out of the range bounds of Float32");
    }
  } else {
    caret.setError("[oatpp::mongo::bson::Utils::readPrimitive()]: Error. Can't deserialize Float32 value. BSON document value type is expected to be DOUBLE.");
  }
}

void Utils::writePrimitive(ConsistentOutputStream *stream, const StringKeyLabel &key, v_float64 value) {
  writeKey(stream, TypeCode::DOUBLE, key);
  writeFloat64(stream, value);
}

void Utils::readPrimitive(parser::Caret& caret, v_float64& value, v_char8 bsonTypeCode) {
  if(bsonTypeCode == TypeCode::DOUBLE) {
    value = readFloat64(caret);
  } else {
    caret.setError("[oatpp::mongo::bson::Utils::readPrimitive()]: Error. Can't deserialize Float64 value. BSON document value type is expected to be DOUBLE.");
  }
}

void Utils::writePrimitive(ConsistentOutputStream *stream, const StringKeyLabel &key, bool value) {
  writeKey(stream, TypeCode::BOOLEAN, key);
  if(value) {
    stream->writeCharSimple(1);
  } else {
    stream->writeCharSimple(0);
  }
}

}}}
