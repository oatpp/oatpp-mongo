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

#include "Connection.hpp"

#include "oatpp-mongo/bson/Utils.hpp"
#include "oatpp/core/data/stream/BufferStream.hpp"

namespace oatpp { namespace mongo { namespace driver { namespace wire {

Connection::Connection(const std::shared_ptr<data::stream::IOStream>& connection)
  : m_connection(connection)
{}

v_io_size Connection::write(const Message& message) {

  if(message.header.messageLength != 16 + message.data->getSize()) {
    throw std::runtime_error("[oatpp::mongo::driver::wire::Connection::write()]: Error. Invalid message header.");
  }

  data::stream::BufferOutputStream stream(16, 16);
  message.header.writeToStream(&stream);

  auto res1 = m_connection->writeExactSizeDataSimple(stream.getData(), stream.getCurrentPosition());

  if(res1 < stream.getCurrentPosition()) {
    return res1;
  }

  auto res2 = m_connection->writeExactSizeDataSimple(message.data->getData(), message.data->getSize());
  if(res2 < 0) {
    return res2;
  }

  return res1 + res2;

}

v_io_size Connection::read(Message& message) {

  const v_buff_size headerSize = 16;
  v_char8 headerDataBuffer[headerSize];
  auto res1 = m_connection->readExactSizeDataSimple(headerDataBuffer, headerSize);
  if(res1 != headerSize) {
    return res1;
  }

  parser::Caret caret(headerDataBuffer, headerSize);
  message.header.readFromCaret(caret);

  oatpp::String dataBuffer(message.header.messageLength - headerSize);
  auto res2 = m_connection->readExactSizeDataSimple(dataBuffer->getData(), dataBuffer->getSize());

  if(res2 < 0) {
    return res2;
  }

  message.data = dataBuffer;

  return res1 + res2;

}

}}}}

