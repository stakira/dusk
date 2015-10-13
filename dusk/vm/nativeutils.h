
#ifndef DUSK_VM_NATIVEUTILS_H
#define DUSK_VM_NATIVEUTILS_H

// std
#include <string>
#include <vector>
#include <sstream>

// dart
#include "dart_api.h"
#include "bin/dartutils.h"

// love
#include "common/Object.h"
#include "common/runtime.h"

namespace dusk {
namespace vm {

Dart_Handle PopError(const char* msg, ...);

// Convert Dart handle to native type
template<typename T>
inline T Convert(Dart_Handle handle);

// Convert Dart native argument to native type
template<typename T>
inline T ConvertArg(
  Dart_NativeArguments args, int index);

inline std::string ConvertArgString(
  Dart_NativeArguments args, int index);

// Convert List type Dart native argument to a vector
template<typename T>
inline std::vector<T> ConvertArgList(
  Dart_NativeArguments args, int index);

template<>
inline int64_t Convert(Dart_Handle handle) {
  if (!Dart_IsInteger(handle)) {
    PopError("expecting a integer");
    return 0; // not reachable
  }
  // todo : check fits
  else {
    int64_t value;
    Dart_IntegerToInt64(handle, &value);
    return value;
  }
}

template<>
inline uint64_t Convert(Dart_Handle handle) {
  if (!Dart_IsInteger(handle)) {
    PopError("expecting a integer");
    return 0; // not reachable
  }
  // todo : check fits
  else {
    uint64_t value;
    Dart_IntegerToUint64(handle, &value);
    return value;
  }
}

template<>
inline int Convert(Dart_Handle handle) {
  if (!Dart_IsInteger(handle)) {
    PopError("expecting a integer");
    return 0; // not reachable
  }
  // todo : check fits
  else {
    uint64_t value;
    Dart_IntegerToUint64(handle, &value);
    return static_cast<int>(value);
  }
}

template<>
inline double Convert(Dart_Handle handle) {
  if (!Dart_IsNumber(handle)) {
    PopError("expecting a number");
    return 0.f; // not reachable
  }
  else {
    if (Dart_IsInteger(handle)) {
      int64_t value;
      Dart_IntegerToInt64(handle, &value);
      return static_cast<double>(value);
    }
    else if (Dart_IsDouble(handle)) {
      double value;
      Dart_DoubleValue(handle, &value);
      return value;
    }
    else {
      PopError("Invalid number");
      return 0.0;
    }
  }
}

template<>
inline float Convert(Dart_Handle handle) {
  return static_cast<float>(Convert<double>(handle));
}

template<>
inline bool Convert(Dart_Handle handle) {
  if (!Dart_IsBoolean(handle)) {
    PopError("expecting a boolean");
    return false; // not reachable
  }
  else {
    bool value;
    Dart_BooleanValue(handle, &value);
    return value;
  }
}

template<typename T>
inline T ConvertArg(
  Dart_NativeArguments args, int index) {
  Dart_Handle ret = Dart_GetNativeArgument(args, index);
  if (Dart_IsError(ret)) {
    Dart_PropagateError(ret);
  }
  return Convert<T>(ret);
}

template<typename T>
inline std::vector<T> ConvertArgList(
  Dart_NativeArguments args, int index) {

  Dart_Handle list = Dart_GetNativeArgument(args, index);
  if (Dart_IsError(list)) {
    Dart_PropagateError(list);
  }

  if (!Dart_IsList(list)) {
    PopError("expecting a list");
  }

  intptr_t len;
  Dart_ListLength(list, &len);
  std::vector<T> vec(len);

  for (intptr_t i = 0; i < len; ++i) {
    vec[i] = Convert<T>(Dart_ListGetAt(list, i));
  }

  return vec;
}

// disable warning caused by uninitialized peer pointer
#pragma warning(push)
#pragma warning(disable:4700)
inline std::string ConvertArgString(
  Dart_NativeArguments args, int index) {
  void** peer;
  Dart_Handle ret = Dart_GetNativeStringArgument(args, 0, peer);
  if (Dart_IsError(ret)) {
    Dart_PropagateError(ret);
  }

  const char* cstr;
  ret = Dart_StringToCString(ret, &cstr);
  if (Dart_IsError(ret)) {
    Dart_PropagateError(ret);
  }

  std::stringstream ss;
  ss << cstr;
  return ss.str();
}
#pragma warning(pop)

Dart_WeakPersistentHandle RegisterGC(
  Dart_Handle dartObject,
  love::Proxy* proxy,
  intptr_t size);

bool ArgIsLoveType(Dart_NativeArguments args, int index, love::Type type);

template<typename T>
T* ConvertArgLoveType(Dart_NativeArguments args, int index, love::Type type) {
  // get pointer
  Dart_Handle handle = Dart_GetNativeArgument(args, index);
  intptr_t ptr = dart::bin::DartUtils::GetIntptrValue(handle);
  // convert type
  love::Proxy *u = reinterpret_cast<love::Proxy*>(ptr);
  if (!love::typeFlags[u->type][type]) {
    const char* name = "Invalid";
    love::getType(type, name);
    vm::PopError("Could not convert to type %s.", name);
  }
  return reinterpret_cast<T*>(u->object);
}

template<typename T>
inline T MapConvertAt(
  Dart_Handle map, const char* key, T fallback) {
  Dart_Handle key_ = Dart_NewStringFromCString(key);
  // check key
  Dart_Handle ret = Dart_MapContainsKey(map, key_);
  if (Dart_IsError(ret)) {
    Dart_PropagateError(ret);
  }
  if (!Convert<bool>(ret)) {
    return fallback;
  }
  // retrive value
  ret = Dart_MapGetAt(map, key_);
  if (Dart_IsError(ret)) {
    Dart_PropagateError(ret);
  }
  return Convert<T>(ret);
}

inline bool MapContainsKey(Dart_Handle map, const char* key) {
  return Convert<bool>(
    Dart_MapContainsKey(map, Dart_NewStringFromCString(key)));
}


}  // namespace vm
}  // namespace dusk

#endif // DUSK_VM_NATIVEUTILS_H
