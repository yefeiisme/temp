// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: ClientGSProtocol.proto

#ifndef PROTOBUF_ClientGSProtocol_2eproto__INCLUDED
#define PROTOBUF_ClientGSProtocol_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3003000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3003000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
namespace WS_CS_NET_PACK {
class CS2WS_LOGIN_RESULT;
class CS2WS_LOGIN_RESULTDefaultTypeInternal;
extern CS2WS_LOGIN_RESULTDefaultTypeInternal _CS2WS_LOGIN_RESULT_default_instance_;
class WS2CS_LOGIN;
class WS2CS_LOGINDefaultTypeInternal;
extern WS2CS_LOGINDefaultTypeInternal _WS2CS_LOGIN_default_instance_;
}  // namespace WS_CS_NET_PACK

namespace WS_CS_NET_PACK {

namespace protobuf_ClientGSProtocol_2eproto {
// Internal implementation detail -- do not call these.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[];
  static const ::google::protobuf::uint32 offsets[];
  static void InitDefaultsImpl();
  static void Shutdown();
};
void AddDescriptors();
void InitDefaults();
}  // namespace protobuf_ClientGSProtocol_2eproto

// ===================================================================

class WS2CS_LOGIN : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:WS_CS_NET_PACK.WS2CS_LOGIN) */ {
 public:
  WS2CS_LOGIN();
  virtual ~WS2CS_LOGIN();

  WS2CS_LOGIN(const WS2CS_LOGIN& from);

  inline WS2CS_LOGIN& operator=(const WS2CS_LOGIN& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const WS2CS_LOGIN& default_instance();

  static inline const WS2CS_LOGIN* internal_default_instance() {
    return reinterpret_cast<const WS2CS_LOGIN*>(
               &_WS2CS_LOGIN_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    0;

  void Swap(WS2CS_LOGIN* other);

  // implements Message ----------------------------------------------

  inline WS2CS_LOGIN* New() const PROTOBUF_FINAL { return New(NULL); }

  WS2CS_LOGIN* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const WS2CS_LOGIN& from);
  void MergeFrom(const WS2CS_LOGIN& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(WS2CS_LOGIN* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required bytes account = 1;
  bool has_account() const;
  void clear_account();
  static const int kAccountFieldNumber = 1;
  const ::std::string& account() const;
  void set_account(const ::std::string& value);
  #if LANG_CXX11
  void set_account(::std::string&& value);
  #endif
  void set_account(const char* value);
  void set_account(const void* value, size_t size);
  ::std::string* mutable_account();
  ::std::string* release_account();
  void set_allocated_account(::std::string* account);

  // required bytes password = 2;
  bool has_password() const;
  void clear_password();
  static const int kPasswordFieldNumber = 2;
  const ::std::string& password() const;
  void set_password(const ::std::string& value);
  #if LANG_CXX11
  void set_password(::std::string&& value);
  #endif
  void set_password(const char* value);
  void set_password(const void* value, size_t size);
  ::std::string* mutable_password();
  ::std::string* release_password();
  void set_allocated_password(::std::string* password);

  // @@protoc_insertion_point(class_scope:WS_CS_NET_PACK.WS2CS_LOGIN)
 private:
  void set_has_account();
  void clear_has_account();
  void set_has_password();
  void clear_has_password();

  // helper for ByteSizeLong()
  size_t RequiredFieldsByteSizeFallback() const;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::HasBits<1> _has_bits_;
  mutable int _cached_size_;
  ::google::protobuf::internal::ArenaStringPtr account_;
  ::google::protobuf::internal::ArenaStringPtr password_;
  friend struct protobuf_ClientGSProtocol_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class CS2WS_LOGIN_RESULT : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:WS_CS_NET_PACK.CS2WS_LOGIN_RESULT) */ {
 public:
  CS2WS_LOGIN_RESULT();
  virtual ~CS2WS_LOGIN_RESULT();

  CS2WS_LOGIN_RESULT(const CS2WS_LOGIN_RESULT& from);

  inline CS2WS_LOGIN_RESULT& operator=(const CS2WS_LOGIN_RESULT& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const CS2WS_LOGIN_RESULT& default_instance();

  static inline const CS2WS_LOGIN_RESULT* internal_default_instance() {
    return reinterpret_cast<const CS2WS_LOGIN_RESULT*>(
               &_CS2WS_LOGIN_RESULT_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    1;

  void Swap(CS2WS_LOGIN_RESULT* other);

  // implements Message ----------------------------------------------

  inline CS2WS_LOGIN_RESULT* New() const PROTOBUF_FINAL { return New(NULL); }

  CS2WS_LOGIN_RESULT* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const CS2WS_LOGIN_RESULT& from);
  void MergeFrom(const CS2WS_LOGIN_RESULT& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(CS2WS_LOGIN_RESULT* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required uint32 result = 1;
  bool has_result() const;
  void clear_result();
  static const int kResultFieldNumber = 1;
  ::google::protobuf::uint32 result() const;
  void set_result(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:WS_CS_NET_PACK.CS2WS_LOGIN_RESULT)
 private:
  void set_has_result();
  void clear_has_result();

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::HasBits<1> _has_bits_;
  mutable int _cached_size_;
  ::google::protobuf::uint32 result_;
  friend struct protobuf_ClientGSProtocol_2eproto::TableStruct;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// WS2CS_LOGIN

// required bytes account = 1;
inline bool WS2CS_LOGIN::has_account() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void WS2CS_LOGIN::set_has_account() {
  _has_bits_[0] |= 0x00000001u;
}
inline void WS2CS_LOGIN::clear_has_account() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void WS2CS_LOGIN::clear_account() {
  account_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_account();
}
inline const ::std::string& WS2CS_LOGIN::account() const {
  // @@protoc_insertion_point(field_get:WS_CS_NET_PACK.WS2CS_LOGIN.account)
  return account_.GetNoArena();
}
inline void WS2CS_LOGIN::set_account(const ::std::string& value) {
  set_has_account();
  account_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:WS_CS_NET_PACK.WS2CS_LOGIN.account)
}
#if LANG_CXX11
inline void WS2CS_LOGIN::set_account(::std::string&& value) {
  set_has_account();
  account_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:WS_CS_NET_PACK.WS2CS_LOGIN.account)
}
#endif
inline void WS2CS_LOGIN::set_account(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  set_has_account();
  account_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:WS_CS_NET_PACK.WS2CS_LOGIN.account)
}
inline void WS2CS_LOGIN::set_account(const void* value, size_t size) {
  set_has_account();
  account_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:WS_CS_NET_PACK.WS2CS_LOGIN.account)
}
inline ::std::string* WS2CS_LOGIN::mutable_account() {
  set_has_account();
  // @@protoc_insertion_point(field_mutable:WS_CS_NET_PACK.WS2CS_LOGIN.account)
  return account_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* WS2CS_LOGIN::release_account() {
  // @@protoc_insertion_point(field_release:WS_CS_NET_PACK.WS2CS_LOGIN.account)
  clear_has_account();
  return account_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void WS2CS_LOGIN::set_allocated_account(::std::string* account) {
  if (account != NULL) {
    set_has_account();
  } else {
    clear_has_account();
  }
  account_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), account);
  // @@protoc_insertion_point(field_set_allocated:WS_CS_NET_PACK.WS2CS_LOGIN.account)
}

// required bytes password = 2;
inline bool WS2CS_LOGIN::has_password() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void WS2CS_LOGIN::set_has_password() {
  _has_bits_[0] |= 0x00000002u;
}
inline void WS2CS_LOGIN::clear_has_password() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void WS2CS_LOGIN::clear_password() {
  password_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_password();
}
inline const ::std::string& WS2CS_LOGIN::password() const {
  // @@protoc_insertion_point(field_get:WS_CS_NET_PACK.WS2CS_LOGIN.password)
  return password_.GetNoArena();
}
inline void WS2CS_LOGIN::set_password(const ::std::string& value) {
  set_has_password();
  password_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:WS_CS_NET_PACK.WS2CS_LOGIN.password)
}
#if LANG_CXX11
inline void WS2CS_LOGIN::set_password(::std::string&& value) {
  set_has_password();
  password_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:WS_CS_NET_PACK.WS2CS_LOGIN.password)
}
#endif
inline void WS2CS_LOGIN::set_password(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  set_has_password();
  password_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:WS_CS_NET_PACK.WS2CS_LOGIN.password)
}
inline void WS2CS_LOGIN::set_password(const void* value, size_t size) {
  set_has_password();
  password_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:WS_CS_NET_PACK.WS2CS_LOGIN.password)
}
inline ::std::string* WS2CS_LOGIN::mutable_password() {
  set_has_password();
  // @@protoc_insertion_point(field_mutable:WS_CS_NET_PACK.WS2CS_LOGIN.password)
  return password_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* WS2CS_LOGIN::release_password() {
  // @@protoc_insertion_point(field_release:WS_CS_NET_PACK.WS2CS_LOGIN.password)
  clear_has_password();
  return password_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void WS2CS_LOGIN::set_allocated_password(::std::string* password) {
  if (password != NULL) {
    set_has_password();
  } else {
    clear_has_password();
  }
  password_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), password);
  // @@protoc_insertion_point(field_set_allocated:WS_CS_NET_PACK.WS2CS_LOGIN.password)
}

// -------------------------------------------------------------------

// CS2WS_LOGIN_RESULT

// required uint32 result = 1;
inline bool CS2WS_LOGIN_RESULT::has_result() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void CS2WS_LOGIN_RESULT::set_has_result() {
  _has_bits_[0] |= 0x00000001u;
}
inline void CS2WS_LOGIN_RESULT::clear_has_result() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void CS2WS_LOGIN_RESULT::clear_result() {
  result_ = 0u;
  clear_has_result();
}
inline ::google::protobuf::uint32 CS2WS_LOGIN_RESULT::result() const {
  // @@protoc_insertion_point(field_get:WS_CS_NET_PACK.CS2WS_LOGIN_RESULT.result)
  return result_;
}
inline void CS2WS_LOGIN_RESULT::set_result(::google::protobuf::uint32 value) {
  set_has_result();
  result_ = value;
  // @@protoc_insertion_point(field_set:WS_CS_NET_PACK.CS2WS_LOGIN_RESULT.result)
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)


}  // namespace WS_CS_NET_PACK

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_ClientGSProtocol_2eproto__INCLUDED
