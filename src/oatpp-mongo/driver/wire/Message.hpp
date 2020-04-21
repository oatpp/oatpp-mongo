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

#include <list>

namespace oatpp { namespace mongo { namespace driver { namespace wire {

struct Section {
public:
  static constexpr v_uint8 TYPE_BODY = 0;
  static constexpr v_uint8 TYPE_DOCUMENT_SEQUENCE = 1;
protected:
  v_uint8 m_type;
public:

  Section(v_uint8 type)
    : m_type(type)
  {}

  v_uint8 getType() {return m_type;}

  virtual void writeToStream(data::stream::ConsistentOutputStream* stream) = 0;
  virtual bool readFromCaret(parser::Caret& caret) = 0;

};

struct Message {
public:

  static constexpr v_int32 OP_CODE = 2013;

  static constexpr v_int32 FLAG_CHECKSUM_PRESENT    = (1 << 0);
  static constexpr v_int32 FLAG_MORE_TO_COME        = (1 << 1);
  static constexpr v_int32 FLAG_EXHAUST_ALLOWED     = (1 << 16);

public:

  bool checksumPresent = false;
  bool moreToCome = false;
  bool exhaustAllowed = false;

  std::list<std::shared_ptr<Section>> sections;

public:

  void writeToStream(data::stream::ConsistentOutputStream* stream);
  bool readFromCaret(parser::Caret& caret);

};

struct BodySection : public Section {
public:

  BodySection() : Section(TYPE_BODY) {}

  oatpp::String document;

public:

  void writeToStream(data::stream::ConsistentOutputStream* stream) override;
  bool readFromCaret(parser::Caret& caret) override;

};

struct DocumentSequenceSection : public Section {
public:

  DocumentSequenceSection(const oatpp::String& sectionIdentifier) : Section(TYPE_DOCUMENT_SEQUENCE)
    , identifier(sectionIdentifier)
  {}

  oatpp::String identifier;

  std::list<oatpp::String> documents;

public:

  void writeToStream(data::stream::ConsistentOutputStream* stream) override;

  bool readFromCaret(parser::Caret& caret) override;

};

}}}}

#endif // oatpp_mongo_driver_wire_Message_hpp
