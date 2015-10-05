
#ifndef DUSK_GRAPHICS_NATIVE_H
#define DUSK_GRAPHICS_NATIVE_H

#include "dart_api.h"
#include "dusk/vm/native.h"

namespace dusk {
namespace graphics {

#define GRAPHICS_NATIVE_LIST(V)   \
  /* Drawing */                   \
  V(Graphics_Arc, 7)              \
  V(Graphics_Ellipse, 6)					\
  V(Graphics_Circle, 5)						\
  V(Graphics_Clear, 0)						\
  V(Graphics_Draw, 0)             \
  V(Graphics_Line, 4)             \
  V(Graphics_Point, 2)						\
  V(Graphics_Polygon, 2)					\
  V(Graphics_Polyline, 1)					\
  V(Graphics_Present, 0)					\
  V(Graphics_Print, 10)						\
  V(Graphics_Printf, 12)					\
  V(Graphics_Rectangle, 8)				\
  /* Coordinate */                \
  V(Graphics_Origin, 0)           \
  V(Graphics_Pop, 0)              \
  V(Graphics_Push, 0)             \
  V(Graphics_Rotate, 1)           \
  V(Graphics_Scale, 2)            \
  V(Graphics_Shear, 2)            \
  V(Graphics_Translate, 2)        \

GRAPHICS_NATIVE_LIST(NATIVE_DECL)

static struct NativeEntry {
  const char* name_;
  Dart_NativeFunction function_;
  int argument_count_;
} GraphicsNativeEntries[] = {
  GRAPHICS_NATIVE_LIST(NATIVE_ENTRY)
};

NATIVE_LOOKUP(GraphicsNativeEntries)

NATIVE_SYMBOL(GraphicsNativeEntries)

}  // namespace graphics
}  // namespace dusk

#endif  // DUSK_GRAPHICS_NATIVE_H
