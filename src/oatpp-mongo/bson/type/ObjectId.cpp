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

#include "ObjectId.hpp"

#include "oatpp/core/utils/Random.hpp"
#include <chrono>

namespace oatpp { namespace mongo { namespace bson { namespace type {

const std::string ObjectId::PROCESS_UNIQUE = seedProcessUnique();
std::atomic<v_uint64> ObjectId::COUNTER(seedCounter());

std::string ObjectId::seedProcessUnique() {
  v_char8 buff[5];
  utils::random::Random::randomBytes(buff, 5);
  return std::string((const char*)buff, 5);
}

v_uint64 ObjectId::seedCounter() {
  v_uint64 result;
  utils::random::Random::randomBytes((p_char8)&result, 8);
  return result;
}

ObjectId::ObjectId() {

  v_uint32 counter = (++ COUNTER) % 16777216;
  v_uint32 seconds = (v_uint32 )std::chrono::duration_cast<std::chrono::seconds>
    (std::chrono::system_clock::now().time_since_epoch()).count();

  m_data[0] = (v_char8) (0xFF & (seconds >> 24));
  m_data[1] = (v_char8) (0xFF & (seconds >> 16));
  m_data[2] = (v_char8) (0xFF & (seconds >> 8));
  m_data[3] = (v_char8) (0xFF &  seconds);

  for(v_buff_size i = 0; i < 5; i ++) {
    m_data[4 + i] = PROCESS_UNIQUE[i];
  }

  m_data[ 9] = (v_char8) (0xFF & (counter >> 16));
  m_data[10] = (v_char8) (0xFF & (counter >> 8));
  m_data[11] = (v_char8) (0xFF &  counter);

}

ObjectId::ObjectId(v_char8 data[DATA_SIZE]) {
  for(v_buff_size i = 0; i < DATA_SIZE; i ++) {
    m_data[i] = data[i];
  }
}

const p_char8 ObjectId::getData() const {
  return (const p_char8)&m_data;
}

v_buff_size ObjectId::getSize() const {
  return DATA_SIZE;
}

v_uint32 ObjectId::getTimestamp() const {
  return (m_data[0] << 24) | (m_data[1] << 16) | (m_data[2] << 8) | m_data[3];
}

oatpp::String ObjectId::toString() const {
  static const v_char8 alphabet[16] {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  oatpp::String result(DATA_SIZE * 2);
  p_char8 rdata = (p_char8) result->data();
  for(v_buff_size i = 0; i < DATA_SIZE; i ++) {
    auto a = m_data[i];
    v_char8 b1 = 0x0F & (a >> 4);
    v_char8 b2 = 0x0F & (a);
    rdata[i * 2    ] = alphabet[b1];
    rdata[i * 2 + 1] = alphabet[b2];
  }
  return result;
}

bool ObjectId::operator==(const ObjectId &other) const {

  v_uint64 m1 = *((p_uint64) m_data);
  v_uint64 o1 = *((p_uint64) other.m_data);

  v_uint32 m2 = *((p_uint32) &m_data[8]);
  v_uint32 o2 = *((p_uint32) &other.m_data[8]);

  return m1 == o1 && m2 == o2;
}

bool ObjectId::operator!=(const ObjectId &other) const {
  return !operator==(other);
}

}}}}
