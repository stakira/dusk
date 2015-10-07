
#ifndef DUSK_VM_LIBHANDLER_H
#define DUSK_VM_LIBHANDLER_H

// std
#include <string>
#include <unordered_map>

// dart
#include "include/dart_api.h"

namespace dusk {
namespace vm {

struct pairhash {
public:
  template <typename T, typename U>
  std::size_t operator()(const std::pair<T, U> &x) const {
    // not efficient at all but we don't need much either
    return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
  }
};

class LibraryRegistry {
 public:
  static void RegisterLibrary(
    const char* name,
    const char* source);

  // Native function are registered regardless of its library,
  // assuming they are always prefixed by library name. Therefore
  // no two function has the same name and number of arguments.
  static bool RegisterNativeFunction(
    const char* name, int numArgs,
    Dart_NativeFunction fn);

  static Dart_Handle LibraryTagHandler(Dart_LibraryTag tag,
    Dart_Handle library,
    Dart_Handle url);

 private:
  static const char* GetLibrarySource(std::string url);

  static Dart_NativeFunction NativeEntryResolver(
    Dart_Handle name,
    int num_of_arguments,
    bool* auto_setup_scope);

  static const uint8_t* NativeEntrySymbol(Dart_NativeFunction nf);

  static Dart_NativeFunction NativeFunctionLookup(
    std::string name, int numArgs);

  static const char* NativeFunctionNameLookup(
    Dart_NativeFunction fn);

  // <<name, numArgs>, func>
  static std::unordered_map<std::pair<std::string, int>,
    Dart_NativeFunction, pairhash> funcs_;

  // <url, <name, source>>
  static std::unordered_map<std::string,
    std::pair<const char*, const char*>> libs_;
};

}  // namespace vm
}  // namespace dusk

#endif // DUSK_VM_LIBHANDLER_H
