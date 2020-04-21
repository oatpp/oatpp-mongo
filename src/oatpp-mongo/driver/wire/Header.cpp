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

#include "Header.hpp"

#include "oatpp-mongo/bson/Utils.hpp"

namespace oatpp { namespace mongo { namespace driver { namespace wire {

void Header::writeToStream(data::stream::ConsistentOutputStream* stream) {
  bson::Utils::writeInt32(stream, messageLength);
  bson::Utils::writeInt32(stream, requestId);
  bson::Utils::writeInt32(stream, responseTo);
  bson::Utils::writeInt32(stream, opCode);
}

bool Header::readFromCaret(parser::Caret& caret) {
  messageLength = bson::Utils::readInt32(caret);
  requestId = bson::Utils::readInt32(caret);
  responseTo = bson::Utils::readInt32(caret);
  opCode = bson::Utils::readInt32(caret);
  return !caret.hasError();
}

}}}}
