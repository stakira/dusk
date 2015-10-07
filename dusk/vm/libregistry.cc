
// dusk
#include "libregistry.h"

// dart
#include "bin/dartutils.h"

namespace dusk {
namespace vm {

const char* dusk_scheme = "dusk";
const int dusk_scheme_len = 4;

std::unordered_map<std::pair<std::string, int>,
  Dart_NativeFunction, pairhash> LibraryRegistry::funcs_;

std::unordered_map<std::string,
  std::pair<const char*, const char* >> LibraryRegistry::libs_;

static bool IsDuskScheme(const char* url) {
  return strncmp(dusk_scheme, url, dusk_scheme_len) == 0;
}

Dart_Handle LibraryRegistry::LibraryTagHandler(
  Dart_LibraryTag tag,
  Dart_Handle library,
  Dart_Handle url) {
  // check
  if (!Dart_IsLibrary(library)) {
    return Dart_NewApiError("not a library");
  }
  if (!Dart_IsString(url)) {
    return Dart_NewApiError("url is not a string");
  }

  // get urls
  const char* url_string = NULL;
  Dart_Handle result = Dart_StringToCString(url, &url_string);
  if (Dart_IsError(result)) {
    return result;
  }
  Dart_Handle library_url = Dart_LibraryUrl(library);
  const char* library_url_string = NULL;
  result = Dart_StringToCString(library_url, &library_url_string);
  if (Dart_IsError(result)) {
    return result;
  }

  bool is_dusk = IsDuskScheme(url_string);
  
  if (is_dusk) {
    // handle dusk scheme
    // find dusk library

    const char* source = GetLibrarySource(url_string);
    if (source == nullptr) {
      return Dart_NewApiError("Unknown dusk scheme library.");
    }

    // load source
    Dart_Handle result = Dart_LoadLibrary(url,
      Dart_NewStringFromCString(source), 0, 0);
    if (Dart_IsError(result)) {
      printf("Cannot load source for %s: '%s'\n",
        url_string, Dart_GetError(result));
      return result;
    }

    // set native resolver
    Dart_SetNativeResolver(result,
      NativeEntryResolver, NativeEntrySymbol);

    return url;
  }
  else {
    // not dusk scheme
    return dart::bin::DartUtils::LibraryTagHandler(tag, library, url);
  }
}

void LibraryRegistry::RegisterLibrary(
  const char* name,
  const char* source) {
  std::string url = dusk_scheme;
  url += ":";
  url += name;
  libs_[url] = std::make_pair(name, source);
}

const char* LibraryRegistry::GetLibrarySource(std::string url) {
  if (libs_.find(url) == libs_.end()) {
    return nullptr;
  }
  else {
    return libs_[url].second;
  }
}

bool LibraryRegistry::RegisterNativeFunction(
  const char* name, int numArgs,
  Dart_NativeFunction fn) {
  auto key = std::make_pair(name, numArgs);
  if (funcs_.find(key) != funcs_.end()) {
    return false;
  }
  else {
    printf("Register %s %d\n", key.first, key.second);
    funcs_[key] = fn;
    return true;
  }
}

Dart_NativeFunction LibraryRegistry::NativeEntryResolver(
  Dart_Handle name,
  int num_of_arguments,
  bool* auto_setup_scope) {
  const char* namestr;
  Dart_StringToCString(name, &namestr);
  *auto_setup_scope = true;
  return NativeFunctionLookup(std::string(namestr), num_of_arguments);
}

const uint8_t* LibraryRegistry::NativeEntrySymbol(
  Dart_NativeFunction nf) {
  return reinterpret_cast<const uint8_t*>(
    NativeFunctionNameLookup(nf));
}

Dart_NativeFunction LibraryRegistry::NativeFunctionLookup(
  std::string name,
  int numArgs) {
  printf("Lookup %s %d\n", name.c_str(), numArgs);
  auto key = std::make_pair(name, numArgs);
  if (funcs_.find(key) == funcs_.end()) {
    return nullptr;
  }
  else {
    return funcs_[key];
  }
}

const char* LibraryRegistry::NativeFunctionNameLookup(
  Dart_NativeFunction fn) {
  for (auto it = funcs_.begin(); it != funcs_.end(); ++it) {
    if (it->second == fn) {
      // should be safe since function names are not further modified
      return it->first.first.c_str();
    }
  }
  return 0;
}

}  // namespace vm
}  // namespace dusk