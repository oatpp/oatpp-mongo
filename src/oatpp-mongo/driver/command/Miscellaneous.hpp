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

#ifndef oatpp_mongo_driver_command_Miscellaneous_hpp
#define oatpp_mongo_driver_command_Miscellaneous_hpp

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

namespace oatpp { namespace mongo { namespace driver { namespace command {

#include OATPP_CODEGEN_BEGIN(DTO)

class WriteConcern : public oatpp::Object {

  DTO_INIT(WriteConcern, Object)

  DTO_FIELD(String, w, "w");
  DTO_FIELD(Boolean, j, "j");
  DTO_FIELD(Int64, wtimeout, "wtimeout");

};

#include OATPP_CODEGEN_END(DTO)

}}}}

#endif // oatpp_mongo_driver_command_Miscellaneous_hpp
