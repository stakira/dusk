
#ifndef DUSK_FILESYSTEM_NATIVE_H
#define DUSK_FILESYSTEM_NATIVE_H

#include "dusk/vm/native.h"

namespace dusk {
namespace filesystem {

// todo : read / write file
#define FILESYSTEM_NATIVE_LIST(V)         \
  V(Filesystem_OpenFile, 2)               \
  V(Filesystem_CloseFile, 1)              \

FILESYSTEM_NATIVE_LIST(NATIVE_DECL)

void Register();

}  // namespace filesystem
}  // namespace dusk

#endif // DUSK_FILESYSTEM_NATIVE_H
