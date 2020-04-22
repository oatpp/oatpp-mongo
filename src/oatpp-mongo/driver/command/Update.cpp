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

#include "Update.hpp"

#include "oatpp-mongo/driver/wire/Header.hpp"

#include "oatpp/core/data/stream/BufferStream.hpp"

namespace oatpp { namespace mongo { namespace driver { namespace command {

Update::Update(const oatpp::String &databaseName,
               const oatpp::String &collectionName,
               const WriteConcern::ObjectWrapper &writeConcern)
  : m_updateDto(UpdateDto::createShared())
  , m_documents(std::make_shared<wire::DocumentSequenceSection>("updates"))
{
  m_updateDto->databaseName = databaseName;
  m_updateDto->collectionName = collectionName;
  m_updateDto->writeConcern = writeConcern;
}

void Update::addDocument(const oatpp::String &document) {
  m_documents->documents.push_back(document);
}

void Update::writeToStream(data::stream::ConsistentOutputStream *stream,
                           bson::mapping::ObjectMapper* commandObjectMapper,
                           v_int32 requestId)
{

  wire::Message msg;

  auto bodySection = std::make_shared<wire::BodySection>();
  bodySection->document = commandObjectMapper->writeToString(m_updateDto);

  msg.sections.push_back(bodySection);
  msg.sections.push_back(m_documents);

  data::stream::BufferOutputStream payloadStream;
  msg.writeToStream(&payloadStream);

  wire::Header header{ 16 + (v_int32) payloadStream.getCurrentPosition(), requestId, 0, wire::Message::OP_CODE};

  header.writeToStream(stream);
  payloadStream.flushToStream(stream);

}

}}}}