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

#ifndef oatpp_mongo_driver_wire_Connection_hpp
#define oatpp_mongo_driver_wire_Connection_hpp

#include "./Message.hpp"
#include "oatpp/core/provider/Provider.hpp"
#include "oatpp/core/data/stream/Stream.hpp"

namespace oatpp { namespace mongo { namespace driver { namespace wire {

/**
 * MongoDB connection.
 */
class Connection {
private:
  provider::ResourceHandle<data::stream::IOStream> m_connection;
public:

  Connection(const provider::ResourceHandle<data::stream::IOStream>& connection);

  v_io_size write(const Message& message);
  v_io_size read(Message& message);


};

}}}}

#endif // oatpp_mongo_driver_wire_Connection_hpp
