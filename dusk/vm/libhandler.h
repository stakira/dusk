
#ifndef DUSK_VM_LIBHANDLER_H
#define DUSK_VM_LIBHANDLER_H

#include "include/dart_api.h"

namespace dusk {
namespace vm {

Dart_Handle LibraryTagHandler(Dart_LibraryTag tag,
  Dart_Handle library,
  Dart_Handle url);

}  // namespace vm
}  // namespace dusk

#endif // DUSK_VM_LIBHANDLER_H
