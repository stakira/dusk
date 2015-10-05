
#include "libhandler.h"

#include <string.h>

#include "bin/dartutils.h"
#include "dusk/window/window_native.h"
#include "dusk/window/window_source.h"
#include "dusk/graphics/graphics_native.h"
#include "dusk/graphics/graphics_source.h"

namespace dusk {
namespace vm {

const char* dusk_scheme = "dusk";
const int dusk_scheme_len = 4;

#define DUSK_LIB_ENTRY(name) \
  { #name, "dusk:"#name, dusk::##name##::source, \
  dusk::##name##::NativeLookup, dusk::##name##::NativeSymbol }

static struct DuskLibEntry {
  const char* name;
  const char* url;
  const char* source;
  Dart_NativeEntryResolver lookup;
  Dart_NativeEntrySymbol symbol;
} DuskLibEntries[] = {
  DUSK_LIB_ENTRY(window),
  DUSK_LIB_ENTRY(graphics),
  { 0, 0, 0, 0, 0 }
};

static bool IsDuskScheme(const char* url) {
  return strncmp(dusk_scheme, url, dusk_scheme_len) == 0;
}

static bool IsDuskLib(DuskLibEntry lib, const char* url) {
  return strcmp(lib.url, url) == 0;
}

Dart_Handle LibraryTagHandler(Dart_LibraryTag tag,
  Dart_Handle library,
  Dart_Handle url)
{
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

  // handle dusk scheme
  bool is_dusk = IsDuskScheme(url_string);
  if (is_dusk) {
    for (int i = 0; DuskLibEntries[i].name != 0; i++) {
      if (IsDuskLib(DuskLibEntries[i], url_string)) {
        // load source
        Dart_Handle result = Dart_LoadLibrary(url,
          Dart_NewStringFromCString(DuskLibEntries[i].source), 0, 0);
        if (Dart_IsError(result)) {
          printf("Cannot load source for %s: '%s'",
            url_string, Dart_GetError(result));
          return result;
        }
        // set native resolver
        Dart_SetNativeResolver(result,
          DuskLibEntries[i].lookup, DuskLibEntries[i].symbol);
        return url;
      }
    }
    return Dart_NewApiError("Unknown dusk scheme library.");
  }
  else {
    // not dusk scheme
    return dart::bin::DartUtils::LibraryTagHandler(tag, library, url);
  }
}

}  // namespace vm
}  // namespace dusk