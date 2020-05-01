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

#ifndef oatpp_mongo_driver_command_Find_hpp
#define oatpp_mongo_driver_command_Find_hpp

#include "./Miscellaneous.hpp"
#include "./Command.hpp"

#include "oatpp-mongo/driver/wire/OpMsg.hpp"
#include "oatpp-mongo/bson/mapping/ObjectMapper.hpp"

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

namespace oatpp { namespace mongo { namespace driver { namespace command {

#include OATPP_CODEGEN_BEGIN(DTO)

class Find : public Command {
private:

  class FindDto : public oatpp::Object {

    DTO_INIT(FindDto, Object)

    DTO_FIELD(String, collectionName, "find");
    DTO_FIELD(String, databaseName, "$db");

  };

private:
  FindDto::ObjectWrapper m_findDto;
public:

  Find(const oatpp::String& databaseName, const oatpp::String& collectionName);

  wire::Message toMessage(ObjectMapper* commandObjectMapper) override;

};

#include OATPP_CODEGEN_END(DTO)

}}}}

#endif // oatpp_mongo_driver_command_Find_hpp
