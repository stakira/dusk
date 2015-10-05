
#include "nativeutils.h"

namespace dusk {
namespace vm {

Dart_Handle PopError(const char* error) {
  Dart_Handle ret = Dart_PropagateError(Dart_NewApiError(error));
  // should be unreachable
  return ret;
}

}  // namespace vm
}  // namespace dusk
