// Copyright 2006-2008 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// The common functionality when building with or without snapshots.

#include "src/snapshot/snapshot.h"

#include "src/api.h"
#include "src/base/platform/platform.h"
#include "src/full-codegen/full-codegen.h"
#include "src/snapshot/deserializer.h"
#include "src/snapshot/snapshot-source-sink.h"
#include "src/version.h"

namespace v8 {
namespace internal {

#ifdef DEBUG
bool Snapshot::SnapshotIsValid(v8::StartupData* snapshot_blob) {
  return Snapshot::ExtractNumContexts(snapshot_blob) > 0;
}
#endif  // DEBUG


bool Snapshot::HaveASnapshotToStartFrom(Isolate* isolate) {
  // Do not use snapshots if the isolate is used to create snapshots.
  return isolate->snapshot_blob() != NULL &&
         isolate->snapshot_blob()->data != NULL;
}


uint32_t Snapshot::SizeOfFirstPage(Isolate* isolate, AllocationSpace space) {
  DCHECK(space >= FIRST_PAGED_SPACE && space <= LAST_PAGED_SPACE);
  if (!isolate->snapshot_available()) {
    return static_cast<uint32_t>(MemoryAllocator::PageAreaSize(space));
  }
  uint32_t size;
  int offset = kFirstPageSizesOffset + (space - FIRST_PAGED_SPACE) * kInt32Size;
  memcpy(&size, isolate->snapshot_blob()->data + offset, kInt32Size);
  return size;
}


bool Snapshot::Initialize(Isolate* isolate) {
  if (!isolate->snapshot_available()) return false;
  base::ElapsedTimer timer;
  if (FLAG_profile_deserialization) timer.Start();

  const v8::StartupData* blob = isolate->snapshot_blob();
  Vector<const byte> startup_data = ExtractStartupData(blob);
  SnapshotData snapshot_data(startup_data);
  Deserializer deserializer(&snapshot_data);
  bool success = isolate->Init(&deserializer);
  if (FLAG_profile_deserialization) {
    double ms = timer.Elapsed().InMillisecondsF();
    int bytes = startup_data.length();
    PrintF("[Deserializing isolate (%d bytes) took %0.3f ms]\n", bytes, ms);
  }
  return success;
}

MaybeHandle<Context> Snapshot::NewContextFromSnapshot(
    Isolate* isolate, Handle<JSGlobalProxy> global_proxy,
    size_t context_index) {
  if (!isolate->snapshot_available()) return Handle<Context>();
  base::ElapsedTimer timer;
  if (FLAG_profile_deserialization) timer.Start();

  const v8::StartupData* blob = isolate->snapshot_blob();
  Vector<const byte> context_data =
      ExtractContextData(blob, static_cast<int>(context_index));
  SnapshotData snapshot_data(context_data);
  Deserializer deserializer(&snapshot_data);

  MaybeHandle<Object> maybe_context =
      deserializer.DeserializePartial(isolate, global_proxy);
  Handle<Object> result;
  if (!maybe_context.ToHandle(&result)) return MaybeHandle<Context>();
  CHECK(result->IsContext());
  if (FLAG_profile_deserialization) {
    double ms = timer.Elapsed().InMillisecondsF();
    int bytes = context_data.length();
    PrintF("[Deserializing context #%zu (%d bytes) took %0.3f ms]\n",
           context_index, bytes, ms);
  }
  return Handle<Context>::cast(result);
}

void UpdateMaxRequirementPerPage(
    uint32_t* requirements,
    Vector<const SerializedData::Reservation> reservations) {
  int space = 0;
  uint32_t current_requirement = 0;
  for (const auto& reservation : reservations) {
    current_requirement += reservation.chunk_size();
    if (reservation.is_last()) {
      requirements[space] = std::max(requirements[space], current_requirement);
      current_requirement = 0;
      space++;
    }
  }
  DCHECK_EQ(i::Serializer::kNumberOfSpaces, space);
}

void CalculateFirstPageSizes(const SnapshotData* startup_snapshot,
                             const List<SnapshotData*>* context_snapshots,
                             uint32_t* sizes_out) {
  if (FLAG_profile_deserialization) {
    int startup_total = 0;
    PrintF("Deserialization will reserve:\n");
    for (const auto& reservation : startup_snapshot->Reservations()) {
      startup_total += reservation.chunk_size();
    }
    PrintF("%10d bytes per isolate\n", startup_total);
    for (int i = 0; i < context_snapshots->length(); i++) {
      int context_total = 0;
      for (const auto& reservation : context_snapshots->at(i)->Reservations()) {
        context_total += reservation.chunk_size();
      }
      PrintF("%10d bytes per context #%d\n", context_total, i);
    }
  }

  uint32_t startup_requirements[i::Serializer::kNumberOfSpaces];
  uint32_t context_requirements[i::Serializer::kNumberOfSpaces];
  for (int space = 0; space < i::Serializer::kNumberOfSpaces; space++) {
    startup_requirements[space] = 0;
    context_requirements[space] = 0;
  }

  UpdateMaxRequirementPerPage(startup_requirements,
                              startup_snapshot->Reservations());
  for (const auto& context_snapshot : *context_snapshots) {
    UpdateMaxRequirementPerPage(context_requirements,
                                context_snapshot->Reservations());
  }

  for (int space = 0; space < i::Serializer::kNumberOfSpaces; space++) {
    // If the space requirement for a page is less than a page size, we consider
    // limiting the size of the first page in order to save memory on startup.
    uint32_t required = startup_requirements[space] +
                        2 * context_requirements[space] +
                        Page::kObjectStartOffset;
    // Add a small allowance to the code space for small scripts.
    if (space == CODE_SPACE) required += 32 * KB;

    if (space >= FIRST_PAGED_SPACE && space <= LAST_PAGED_SPACE) {
      uint32_t max_size =
          MemoryAllocator::PageAreaSize(static_cast<AllocationSpace>(space));
      sizes_out[space - FIRST_PAGED_SPACE] = std::min(required, max_size);
    }
  }
}

v8::StartupData Snapshot::CreateSnapshotBlob(
    const SnapshotData* startup_snapshot,
    const List<SnapshotData*>* context_snapshots) {
  int num_contexts = context_snapshots->length();
  int startup_snapshot_offset = StartupSnapshotOffset(num_contexts);
  int total_length = startup_snapshot_offset;
  total_length += startup_snapshot->RawData().length();
  for (const auto& context_snapshot : *context_snapshots) {
    total_length += context_snapshot->RawData().length();
  }

  uint32_t first_page_sizes[kNumPagedSpaces];
  CalculateFirstPageSizes(startup_snapshot, context_snapshots,
                          first_page_sizes);

  char* data = new char[total_length];
  memcpy(data + kFirstPageSizesOffset, first_page_sizes,
         kNumPagedSpaces * kInt32Size);
  memcpy(data + kNumberOfContextsOffset, &num_contexts, kInt32Size);
  int payload_offset = StartupSnapshotOffset(num_contexts);
  int payload_length = startup_snapshot->RawData().length();
  memcpy(data + payload_offset, startup_snapshot->RawData().start(),
         payload_length);
  if (FLAG_profile_deserialization) {
    PrintF("Snapshot blob consists of:\n%10d bytes for startup\n",
           payload_length);
  }
  payload_offset += payload_length;
  for (int i = 0; i < num_contexts; i++) {
    memcpy(data + ContextSnapshotOffsetOffset(i), &payload_offset, kInt32Size);
    SnapshotData* context_snapshot = context_snapshots->at(i);
    payload_length = context_snapshot->RawData().length();
    memcpy(data + payload_offset, context_snapshot->RawData().start(),
           payload_length);
    if (FLAG_profile_deserialization) {
      PrintF("%10d bytes for context #%d\n", payload_length, i);
    }
    payload_offset += payload_length;
  }

  v8::StartupData result = {data, total_length};
  return result;
}

int Snapshot::ExtractNumContexts(const v8::StartupData* data) {
  CHECK_LT(kNumberOfContextsOffset, data->raw_size);
  int num_contexts;
  memcpy(&num_contexts, data->data + kNumberOfContextsOffset, kInt32Size);
  return num_contexts;
}

Vector<const byte> Snapshot::ExtractStartupData(const v8::StartupData* data) {
  int num_contexts = ExtractNumContexts(data);
  int startup_offset = StartupSnapshotOffset(num_contexts);
  CHECK_LT(startup_offset, data->raw_size);
  int first_context_offset;
  memcpy(&first_context_offset, data->data + ContextSnapshotOffsetOffset(0),
         kInt32Size);
  CHECK_LT(first_context_offset, data->raw_size);
  int startup_length = first_context_offset - startup_offset;
  const byte* startup_data =
      reinterpret_cast<const byte*>(data->data + startup_offset);
  return Vector<const byte>(startup_data, startup_length);
}

Vector<const byte> Snapshot::ExtractContextData(const v8::StartupData* data,
                                                int index) {
  int num_contexts = ExtractNumContexts(data);
  CHECK_LT(index, num_contexts);

  int context_offset;
  memcpy(&context_offset, data->data + ContextSnapshotOffsetOffset(index),
         kInt32Size);
  int next_context_offset;
  if (index == num_contexts - 1) {
    next_context_offset = data->raw_size;
  } else {
    memcpy(&next_context_offset,
           data->data + ContextSnapshotOffsetOffset(index + 1), kInt32Size);
    CHECK_LT(next_context_offset, data->raw_size);
  }

  const byte* context_data =
      reinterpret_cast<const byte*>(data->data + context_offset);
  int context_length = next_context_offset - context_offset;
  return Vector<const byte>(context_data, context_length);
}

SnapshotData::SnapshotData(const Serializer* serializer) {
  DisallowHeapAllocation no_gc;
  List<Reservation> reservations;
  serializer->EncodeReservations(&reservations);
  const List<byte>* payload = serializer->sink()->data();

  // Calculate sizes.
  int reservation_size = reservations.length() * kInt32Size;
  int size = kHeaderSize + reservation_size + payload->length();

  // Allocate backing store and create result data.
  AllocateData(size);

  // Set header values.
  SetMagicNumber(serializer->isolate());
  SetHeaderValue(kCheckSumOffset, Version::Hash());
  SetHeaderValue(kNumReservationsOffset, reservations.length());
  SetHeaderValue(kPayloadLengthOffset, payload->length());

  // Copy reservation chunk sizes.
  CopyBytes(data_ + kHeaderSize, reinterpret_cast<byte*>(reservations.begin()),
            reservation_size);

  // Copy serialized data.
  CopyBytes(data_ + kHeaderSize + reservation_size, payload->begin(),
            static_cast<size_t>(payload->length()));
}

bool SnapshotData::IsSane() {
  return GetHeaderValue(kCheckSumOffset) == Version::Hash();
}

Vector<const SerializedData::Reservation> SnapshotData::Reservations() const {
  return Vector<const Reservation>(
      reinterpret_cast<const Reservation*>(data_ + kHeaderSize),
      GetHeaderValue(kNumReservationsOffset));
}

Vector<const byte> SnapshotData::Payload() const {
  int reservations_size = GetHeaderValue(kNumReservationsOffset) * kInt32Size;
  const byte* payload = data_ + kHeaderSize + reservations_size;
  int length = GetHeaderValue(kPayloadLengthOffset);
  DCHECK_EQ(data_ + size_, payload + length);
  return Vector<const byte>(payload, length);
}

}  // namespace internal
}  // namespace v8
