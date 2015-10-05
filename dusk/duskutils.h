
#ifndef DUSK_UTIL_H
#define DUSK_UTIL_H

#pragma once

#include "include/dart_api.h"

namespace dusk {
  namespace utils {

    Dart_Handle LibraryTagHandler(Dart_LibraryTag tag,
      Dart_Handle library,
      Dart_Handle url);

  }
}

#endif // !DUSK_UTIL_H
