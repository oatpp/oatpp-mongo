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

#include "Find.hpp"

#include "oatpp-mongo/driver/wire/Message.hpp"

#include "oatpp/core/data/stream/BufferStream.hpp"

namespace oatpp { namespace mongo { namespace driver { namespace command {

Find::Find(const oatpp::String &databaseName, const oatpp::String &collectionName)
  : m_findDto(FindDto::createShared())
{
  m_findDto->databaseName = databaseName;
  m_findDto->collectionName = collectionName;
}

wire::Message Find::toMessage(ObjectMapper* commandObjectMapper) {

  wire::OpMsg msg;

  auto bodySection = std::make_shared<wire::BodySection>();
  bodySection->document = commandObjectMapper->writeToString(m_findDto);

  msg.sections.push_back(bodySection);

  data::stream::BufferOutputStream payloadStream;
  msg.writeToStream(&payloadStream);

  auto data = payloadStream.toString();

  return wire::Message(16 + (v_int32) data->getSize(), wire::OpMsg::OP_CODE, data);

}

}}}}
