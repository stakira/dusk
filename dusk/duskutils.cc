
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "duskutils.h"
#include "bin\dartutils.h"
#include "platform\assert.h"
#include "vmhost\native.h"
#include "window\window_native.h"

namespace dusk {
namespace utils {

#define UTILS_NATIVE_LIST(V)                                                  \
  V(Utils_Version, 0)											                                    \

const char* dusk_url = "dusk:dart";

const char* utils_source = "library dusk.dart;\nString Version() native \"DuskNative_Utils_Version\";";

const char* Version() { return "Dusk2d 0.0.1 alpha"; }

void DuskNative_Utils_Version(Dart_NativeArguments arguments) {
  Dart_Handle result = Dart_NewStringFromCString(Version());
  Dart_SetReturnValue(arguments, result);
}

static struct NativeEntry {
  const char* name_;
  Dart_NativeFunction function_;
  int argument_count_;
} UtilsEntries[] = {
  UTILS_NATIVE_LIST(NATIVE_ENTRY)
};

NATIVE_LOOKUP(UtilsEntries)

NATIVE_SYMBOL(UtilsEntries)

static bool IsDuskURL(const char* url_name) {
  return (strcmp(url_name, dusk_url) == 0);
}

void SetNativeResolver() {
    Dart_Handle url = Dart_NewStringFromCString(dusk_url);
    Dart_Handle library = Dart_LookupLibrary(url);
    ASSERT(!Dart_IsError(library));
    // Setup the native resolver for built in library functions.
    DART_CHECK_VALID(
      Dart_SetNativeResolver(library, NativeLookup, NativeSymbol));
}

// move it somewhere
Dart_Handle LibraryTagHandler(Dart_LibraryTag tag,
  Dart_Handle library,
  Dart_Handle url) {
  if (!Dart_IsLibrary(library)) {
    return Dart_NewApiError("not a library");
  }
  if (!Dart_IsString(url)) {
    return Dart_NewApiError("url is not a string");
  }
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
  printf("dusk::utils::LibraryTagHandler tag=%d url_string=%s library_url_string=%s\n",
    tag, url_string, library_url_string);

  bool is_dusk_library = IsDuskURL(url_string);

  if (is_dusk_library) {
    printf("dusk::utils::LibraryTagHandler before load library\n");
    Dart_Handle result = Dart_LoadLibrary(
      url, Dart_NewStringFromCString(utils_source), 0, 0);
    if (Dart_IsError(result)) {
      printf("dusk::utils::LibraryTagHandler '%s'", Dart_GetError(result));
    }
    printf("dusk::utils::LibraryTagHandler after load library\n");
    SetNativeResolver();
    printf("dusk::utils::LibraryTagHandler after SetNativeResolver\n");
    //DART_CHECK_VALID(
    //	Dart_SetNativeResolver(result, NativeLookup, NativeSymbol));
    return url;
  }
  else {
    printf("dart::bin::DartUtils::LibraryTagHandler url_string=%s\n", url_string);
    return dart::bin::DartUtils::LibraryTagHandler(tag, library, url);
  }
}

}
}