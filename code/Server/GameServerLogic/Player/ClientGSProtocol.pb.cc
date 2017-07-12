// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: ClientGSProtocol.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "ClientGSProtocol.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace ClientGSPack {
class C2S_CHAT_MESSAGEDefaultTypeInternal : public ::google::protobuf::internal::ExplicitlyConstructed<C2S_CHAT_MESSAGE> {
} _C2S_CHAT_MESSAGE_default_instance_;

namespace protobuf_ClientGSProtocol_2eproto {


namespace {

::google::protobuf::Metadata file_level_metadata[1];

}  // namespace

PROTOBUF_CONSTEXPR_VAR ::google::protobuf::internal::ParseTableField
    const TableStruct::entries[] = {
  {0, 0, 0, ::google::protobuf::internal::kInvalidMask, 0, 0},
};

PROTOBUF_CONSTEXPR_VAR ::google::protobuf::internal::AuxillaryParseTableField
    const TableStruct::aux[] = {
  ::google::protobuf::internal::AuxillaryParseTableField(),
};
PROTOBUF_CONSTEXPR_VAR ::google::protobuf::internal::ParseTable const
    TableStruct::schema[] = {
  { NULL, NULL, 0, -1, -1, false },
};

const ::google::protobuf::uint32 TableStruct::offsets[] = {
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(C2S_CHAT_MESSAGE, _has_bits_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(C2S_CHAT_MESSAGE, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(C2S_CHAT_MESSAGE, strmsg_),
  0,
};

static const ::google::protobuf::internal::MigrationSchema schemas[] = {
  { 0, 6, sizeof(C2S_CHAT_MESSAGE)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&_C2S_CHAT_MESSAGE_default_instance_),
};

namespace {

void protobuf_AssignDescriptors() {
  AddDescriptors();
  ::google::protobuf::MessageFactory* factory = NULL;
  AssignDescriptors(
      "ClientGSProtocol.proto", schemas, file_default_instances, TableStruct::offsets, factory,
      file_level_metadata, NULL, NULL);
}

void protobuf_AssignDescriptorsOnce() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &protobuf_AssignDescriptors);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::internal::RegisterAllTypes(file_level_metadata, 1);
}

}  // namespace

void TableStruct::Shutdown() {
  _C2S_CHAT_MESSAGE_default_instance_.Shutdown();
  delete file_level_metadata[0].reflection;
}

void TableStruct::InitDefaultsImpl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::internal::InitProtobufDefaults();
  _C2S_CHAT_MESSAGE_default_instance_.DefaultConstruct();
}

void InitDefaults() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &TableStruct::InitDefaultsImpl);
}
void AddDescriptorsImpl() {
  InitDefaults();
  static const char descriptor[] = {
      "\n\026ClientGSProtocol.proto\022\014ClientGSPack\"\""
      "\n\020C2S_CHAT_MESSAGE\022\016\n\006strMsg\030\001 \002(\014"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 74);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "ClientGSProtocol.proto", &protobuf_RegisterTypes);
  ::google::protobuf::internal::OnShutdown(&TableStruct::Shutdown);
}

void AddDescriptors() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &AddDescriptorsImpl);
}
// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer {
  StaticDescriptorInitializer() {
    AddDescriptors();
  }
} static_descriptor_initializer;

}  // namespace protobuf_ClientGSProtocol_2eproto


// ===================================================================

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int C2S_CHAT_MESSAGE::kStrMsgFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

C2S_CHAT_MESSAGE::C2S_CHAT_MESSAGE()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  if (GOOGLE_PREDICT_TRUE(this != internal_default_instance())) {
    protobuf_ClientGSProtocol_2eproto::InitDefaults();
  }
  SharedCtor();
  // @@protoc_insertion_point(constructor:ClientGSPack.C2S_CHAT_MESSAGE)
}
C2S_CHAT_MESSAGE::C2S_CHAT_MESSAGE(const C2S_CHAT_MESSAGE& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      _has_bits_(from._has_bits_),
      _cached_size_(0) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  strmsg_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.has_strmsg()) {
    strmsg_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.strmsg_);
  }
  // @@protoc_insertion_point(copy_constructor:ClientGSPack.C2S_CHAT_MESSAGE)
}

void C2S_CHAT_MESSAGE::SharedCtor() {
  _cached_size_ = 0;
  strmsg_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}

C2S_CHAT_MESSAGE::~C2S_CHAT_MESSAGE() {
  // @@protoc_insertion_point(destructor:ClientGSPack.C2S_CHAT_MESSAGE)
  SharedDtor();
}

void C2S_CHAT_MESSAGE::SharedDtor() {
  strmsg_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}

void C2S_CHAT_MESSAGE::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* C2S_CHAT_MESSAGE::descriptor() {
  protobuf_ClientGSProtocol_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_ClientGSProtocol_2eproto::file_level_metadata[kIndexInFileMessages].descriptor;
}

const C2S_CHAT_MESSAGE& C2S_CHAT_MESSAGE::default_instance() {
  protobuf_ClientGSProtocol_2eproto::InitDefaults();
  return *internal_default_instance();
}

C2S_CHAT_MESSAGE* C2S_CHAT_MESSAGE::New(::google::protobuf::Arena* arena) const {
  C2S_CHAT_MESSAGE* n = new C2S_CHAT_MESSAGE;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void C2S_CHAT_MESSAGE::Clear() {
// @@protoc_insertion_point(message_clear_start:ClientGSPack.C2S_CHAT_MESSAGE)
  if (has_strmsg()) {
    GOOGLE_DCHECK(!strmsg_.IsDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited()));
    (*strmsg_.UnsafeRawStringPointer())->clear();
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear();
}

bool C2S_CHAT_MESSAGE::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:ClientGSPack.C2S_CHAT_MESSAGE)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required bytes strMsg = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(10u)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_strmsg()));
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:ClientGSPack.C2S_CHAT_MESSAGE)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:ClientGSPack.C2S_CHAT_MESSAGE)
  return false;
#undef DO_
}

void C2S_CHAT_MESSAGE::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:ClientGSPack.C2S_CHAT_MESSAGE)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  // required bytes strMsg = 1;
  if (cached_has_bits & 0x00000001u) {
    ::google::protobuf::internal::WireFormatLite::WriteBytesMaybeAliased(
      1, this->strmsg(), output);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:ClientGSPack.C2S_CHAT_MESSAGE)
}

::google::protobuf::uint8* C2S_CHAT_MESSAGE::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:ClientGSPack.C2S_CHAT_MESSAGE)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  // required bytes strMsg = 1;
  if (cached_has_bits & 0x00000001u) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        1, this->strmsg(), target);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:ClientGSPack.C2S_CHAT_MESSAGE)
  return target;
}

size_t C2S_CHAT_MESSAGE::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:ClientGSPack.C2S_CHAT_MESSAGE)
  size_t total_size = 0;

  if (_internal_metadata_.have_unknown_fields()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  // required bytes strMsg = 1;
  if (has_strmsg()) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::BytesSize(
        this->strmsg());
  }
  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = cached_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void C2S_CHAT_MESSAGE::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:ClientGSPack.C2S_CHAT_MESSAGE)
  GOOGLE_DCHECK_NE(&from, this);
  const C2S_CHAT_MESSAGE* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const C2S_CHAT_MESSAGE>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:ClientGSPack.C2S_CHAT_MESSAGE)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:ClientGSPack.C2S_CHAT_MESSAGE)
    MergeFrom(*source);
  }
}

void C2S_CHAT_MESSAGE::MergeFrom(const C2S_CHAT_MESSAGE& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:ClientGSPack.C2S_CHAT_MESSAGE)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.has_strmsg()) {
    set_has_strmsg();
    strmsg_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.strmsg_);
  }
}

void C2S_CHAT_MESSAGE::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:ClientGSPack.C2S_CHAT_MESSAGE)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void C2S_CHAT_MESSAGE::CopyFrom(const C2S_CHAT_MESSAGE& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:ClientGSPack.C2S_CHAT_MESSAGE)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool C2S_CHAT_MESSAGE::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;
  return true;
}

void C2S_CHAT_MESSAGE::Swap(C2S_CHAT_MESSAGE* other) {
  if (other == this) return;
  InternalSwap(other);
}
void C2S_CHAT_MESSAGE::InternalSwap(C2S_CHAT_MESSAGE* other) {
  strmsg_.Swap(&other->strmsg_);
  std::swap(_has_bits_[0], other->_has_bits_[0]);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  std::swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata C2S_CHAT_MESSAGE::GetMetadata() const {
  protobuf_ClientGSProtocol_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_ClientGSProtocol_2eproto::file_level_metadata[kIndexInFileMessages];
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// C2S_CHAT_MESSAGE

// required bytes strMsg = 1;
bool C2S_CHAT_MESSAGE::has_strmsg() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
void C2S_CHAT_MESSAGE::set_has_strmsg() {
  _has_bits_[0] |= 0x00000001u;
}
void C2S_CHAT_MESSAGE::clear_has_strmsg() {
  _has_bits_[0] &= ~0x00000001u;
}
void C2S_CHAT_MESSAGE::clear_strmsg() {
  strmsg_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_strmsg();
}
const ::std::string& C2S_CHAT_MESSAGE::strmsg() const {
  // @@protoc_insertion_point(field_get:ClientGSPack.C2S_CHAT_MESSAGE.strMsg)
  return strmsg_.GetNoArena();
}
void C2S_CHAT_MESSAGE::set_strmsg(const ::std::string& value) {
  set_has_strmsg();
  strmsg_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:ClientGSPack.C2S_CHAT_MESSAGE.strMsg)
}
#if LANG_CXX11
void C2S_CHAT_MESSAGE::set_strmsg(::std::string&& value) {
  set_has_strmsg();
  strmsg_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:ClientGSPack.C2S_CHAT_MESSAGE.strMsg)
}
#endif
void C2S_CHAT_MESSAGE::set_strmsg(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  set_has_strmsg();
  strmsg_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:ClientGSPack.C2S_CHAT_MESSAGE.strMsg)
}
void C2S_CHAT_MESSAGE::set_strmsg(const void* value, size_t size) {
  set_has_strmsg();
  strmsg_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:ClientGSPack.C2S_CHAT_MESSAGE.strMsg)
}
::std::string* C2S_CHAT_MESSAGE::mutable_strmsg() {
  set_has_strmsg();
  // @@protoc_insertion_point(field_mutable:ClientGSPack.C2S_CHAT_MESSAGE.strMsg)
  return strmsg_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
::std::string* C2S_CHAT_MESSAGE::release_strmsg() {
  // @@protoc_insertion_point(field_release:ClientGSPack.C2S_CHAT_MESSAGE.strMsg)
  clear_has_strmsg();
  return strmsg_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
void C2S_CHAT_MESSAGE::set_allocated_strmsg(::std::string* strmsg) {
  if (strmsg != NULL) {
    set_has_strmsg();
  } else {
    clear_has_strmsg();
  }
  strmsg_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), strmsg);
  // @@protoc_insertion_point(field_set_allocated:ClientGSPack.C2S_CHAT_MESSAGE.strMsg)
}

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

}  // namespace ClientGSPack

// @@protoc_insertion_point(global_scope)