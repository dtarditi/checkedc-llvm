//===- TpiStreamBuilder.h - PDB Tpi Stream Creation -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_DEBUGINFO_PDB_RAW_PDBTPISTREAMBUILDER_H
#define LLVM_DEBUGINFO_PDB_RAW_PDBTPISTREAMBUILDER_H

#include "llvm/ADT/Optional.h"
#include "llvm/DebugInfo/CodeView/TypeRecord.h"
#include "llvm/DebugInfo/MSF/ByteStream.h"
#include "llvm/DebugInfo/MSF/SequencedItemStream.h"
#include "llvm/DebugInfo/PDB/Raw/RawConstants.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Error.h"

#include <vector>

namespace llvm {
namespace codeview {
class TypeRecord;
}
namespace msf {
struct MSFLayout;
class ReadableStreamRef;
class WritableStream;

template <> struct SequencedItemTraits<llvm::codeview::CVType> {
  static size_t length(const codeview::CVType &Item) {
    return Item.RawData.size();
  }
  static ArrayRef<uint8_t> bytes(const codeview::CVType &Item) {
    return Item.RawData;
  }
};
}
namespace pdb {
class PDBFile;
class TpiStream;
struct TpiStreamHeader;

class TpiStreamBuilder {
public:
  explicit TpiStreamBuilder(BumpPtrAllocator &Allocator);
  ~TpiStreamBuilder();

  TpiStreamBuilder(const TpiStreamBuilder &) = delete;
  TpiStreamBuilder &operator=(const TpiStreamBuilder &) = delete;

  void setVersionHeader(PdbRaw_TpiVer Version);
  void addTypeRecord(const codeview::CVType &Record);

  Expected<std::unique_ptr<TpiStream>> build(PDBFile &File,
                                             const msf::WritableStream &Buffer);

  Error commit(const msf::MSFLayout &Layout, const msf::WritableStream &Buffer);

  uint32_t calculateSerializedLength() const;

private:
  Error finalize();

  BumpPtrAllocator &Allocator;

  Optional<PdbRaw_TpiVer> VerHeader;
  std::vector<codeview::CVType> TypeRecords;
  msf::SequencedItemStream<codeview::CVType> TypeRecordStream;

  const TpiStreamHeader *Header;
};
}
}

#endif
