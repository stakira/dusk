
// dusk
#include "nativeutils.h"

// std
#include <cstdarg>

namespace dusk {
namespace vm {

Dart_Handle PopError(const char* msg, ...) {
  va_list argList;
  char buffer[256] = { 0 };
  va_start(argList, msg);
  vsprintf(buffer, msg, argList);
  va_end(argList);
  Dart_Handle ret = Dart_PropagateError(Dart_NewApiError(buffer));
  // should be unreachable
  return ret;
}

}  // namespace vm
}  // namespace dusk
