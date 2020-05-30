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

#ifndef oatpp_mongo_driver_wire_Message_hpp
#define oatpp_mongo_driver_wire_Message_hpp

#include "oatpp/core/data/stream/Stream.hpp"
#include "oatpp/core/parser/Caret.hpp"
#include "oatpp/core/Types.hpp"

namespace oatpp { namespace mongo { namespace driver { namespace wire {

struct MessageHeader {
public:

  v_int32 messageLength;
  v_int32 requestId;
  v_int32 responseTo;
  v_int32 opCode;

public:

  MessageHeader() = default;
  MessageHeader(v_int32 length, v_int32 msgOpCode);

  void writeToStream(data::stream::ConsistentOutputStream* stream) const;
  bool readFromCaret(parser::Caret& caret);

};

/**
 * MongoDB wire message.
 */
struct Message {

  Message() = default;
  Message(v_int32 length, v_int32 opCode, const oatpp::String& msgData);

  MessageHeader header;
  oatpp::String data;
};

}}}}


#endif //oatpp_mongo_driver_wire_Message_hpp
