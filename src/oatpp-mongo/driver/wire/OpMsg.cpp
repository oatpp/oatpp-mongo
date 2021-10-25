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

#include "OpMsg.hpp"

#include "oatpp-mongo/bson/Utils.hpp"

#include "oatpp/core/data/stream/BufferStream.hpp"

namespace oatpp { namespace mongo { namespace driver { namespace wire {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Message

void OpMsg::writeToStream(data::stream::ConsistentOutputStream* stream) {

  v_int32 flags = 0;

  if(checksumPresent) flags |= FLAG_CHECKSUM_PRESENT;
  if(moreToCome) flags |= FLAG_MORE_TO_COME;
  if(exhaustAllowed) flags |= FLAG_EXHAUST_ALLOWED;

  bson::Utils::writeInt32(stream, flags);

  for(auto& section : sections) {
    section->writeToStream(stream);
  }

}

bool OpMsg::readFromCaret(parser::Caret& caret) {

  sections.clear();

  v_int32 flags = bson::Utils::readInt32(caret);

  checksumPresent = (flags & FLAG_CHECKSUM_PRESENT) > 0;
  moreToCome = (flags & FLAG_MORE_TO_COME) > 0;
  exhaustAllowed = (flags & FLAG_EXHAUST_ALLOWED) > 0;

  while(caret.canContinue()) {

    v_uint8 sectionType = (v_uint8) *caret.getCurrData();
    std::shared_ptr<Section> section;

    switch(sectionType) {
      case Section::TYPE_BODY:
        section = std::make_shared<BodySection>();
        break;
      case Section::TYPE_DOCUMENT_SEQUENCE:
        section = std::make_shared<DocumentSequenceSection>(nullptr);
        break;
      default:
        caret.setError("[oatpp::mongo::driver::wire::Message::readFromCaret()]: Error. Invalid section type!");
        return false;
    }

    section->readFromCaret(caret);
    sections.push_back(section);

  }

  return !caret.hasError();

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BodySection

void BodySection::writeToStream(data::stream::ConsistentOutputStream* stream) {
  stream->writeCharSimple(TYPE_BODY);
  *stream << document;
}

bool BodySection::readFromCaret(parser::Caret& caret) {
  if(caret.canContinueAtChar(TYPE_BODY, 1)) {
    auto label = caret.putLabel();
    v_int64 docSize = bson::Utils::readInt32(caret);
    if(docSize < 5 || docSize - 4 + (v_int64) caret.getPosition() > (v_int64) caret.getDataSize()) {
      caret.setError("[oatpp::mongo::driver::wire::BodySection::readFromCaret()]: Error. Invalid Document size.");
      return false;
    }
    caret.inc((v_buff_size) docSize - 4);
    document = label.toString();
    return true;
  }
  caret.setError("[oatpp::mongo::driver::wire::BodySection::readFromCaret()]: Error. Invalid section type.");
  return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DocumentSequenceSection

void DocumentSequenceSection::writeToStream(data::stream::ConsistentOutputStream* stream) {

  stream->writeCharSimple(TYPE_DOCUMENT_SEQUENCE);

  v_int32 size = 4 + identifier->size() + 1;
  for(auto& doc : documents) {
    size += doc->size();
  }
  bson::Utils::writeInt32(stream, size);

  *stream << identifier;
  stream->writeCharSimple(0);

  for(auto& doc : documents) {
    *stream << doc;
  }

}

bool DocumentSequenceSection::readFromCaret(parser::Caret& caret) {

  if(caret.canContinueAtChar(TYPE_DOCUMENT_SEQUENCE, 1)) {

    v_int64 overallSize = bson::Utils::readInt32(caret);
    v_int64 progress = 4;

    if(overallSize < 5 || overallSize - 4 + (v_int64) caret.getPosition() > (v_int64) caret.getDataSize()) {
      caret.setError("[oatpp::mongo::driver::wire::DocumentSequenceSection::readFromCaret()]: Error. Invalid Sequence size.");
      return false;
    }

    identifier = bson::Utils::readCString(caret);
    if(caret.hasError()) {
      caret.setError("[oatpp::mongo::driver::wire::DocumentSequenceSection::readFromCaret()]: Error. Invalid Sequence identifier.");
      return false;
    }

    progress += identifier->size() + 1;

    while(caret.canContinue() && progress < overallSize) {

      auto label = caret.putLabel();
      v_int64 docSize = bson::Utils::readInt32(caret);
      if (docSize < 5 || docSize - 4 + (v_int64) caret.getPosition() > (v_int64) caret.getDataSize()) {
        caret.setError("[oatpp::mongo::driver::wire::DocumentSequenceSection::readFromCaret()]: Error. Invalid Document size.");
        return false;
      }
      caret.inc((v_buff_size) docSize - 4);
      oatpp::String document = label.toString();
      documents.push_back(document);

      progress += document->size();

    }

    return !caret.hasError();

  }

  caret.setError("[oatpp::mongo::driver::wire::DocumentSequenceSection::readFromCaret()]: Error. Invalid section type.");
  return false;

}

}}}}
