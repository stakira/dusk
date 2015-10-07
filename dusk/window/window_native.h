
#ifndef DUSK_WINDOW_NATIVE_H
#define DUSK_WINDOW_NATIVE_H

#include "dusk/vm/native.h"

namespace dusk {
namespace window {

#define WINDOW_NATIVE_LIST(V)         \
  V(Window_FromPixels, 2)             \
  V(Window_GetDesktopDimensions, 1)   \
  V(Window_GetDisplayCount, 0)        \
  V(Window_GetDisplayName, 1)         \
  V(Window_GetFullscreen, 0)          \
  V(Window_GetFullscreenModes, 1)     \
  V(Window_GetIcon, 0)                \
  V(Window_GetMode, 0)                \
  V(Window_GetPixelScale, 0)          \
  V(Window_GetPosition, 0)            \
  V(Window_GetTitle, 0)               \
  V(Window_HasFocus, 0)               \
  V(Window_HasMouseFocus, 0)          \
  V(Window_IsCreated, 0)              \
  V(Window_IsVisible, 0)              \
  V(Window_Minimize, 0)               \
  V(Window_Maximize, 0)               \
  V(Window_SetFullscreen, 2)          \
  V(Window_SetIcon, 0)                \
  V(Window_SetMode, 3)                \
  V(Window_SetPosition, 3)            \
  V(Window_SetTitle, 1)               \
  V(Window_ShowMessageBox, 0)         \
  V(Window_ToPixels, 2)               \

WINDOW_NATIVE_LIST(NATIVE_DECL)

void Register();

}  // namespace window
}  // namespace dusk

#endif // !DUSK_WINDOW_NATIVE_H
