
// dusk
#include "window_native.h"
#include "window_source.h"
#include "dusk/vm/nativeutils.h"
#include "dusk/vm/libregistry.h"

// std
#include <string>
#include <sstream>

// dart
#include "dart_api.h"

// love
#include "common/Module.h"
#include "modules/window/sdl/Window.h"

namespace dusk {
namespace window {

NativeEntry WindowNativeEntries[] = {
  WINDOW_NATIVE_LIST(NATIVE_ENTRY)
};

void Register() {
  vm::LibraryRegistry::RegisterLibrary("window", source);
  REG_ENTRIES(WindowNativeEntries)
}

#define instance() (love::Module::getInstance<love::window::sdl::Window>(love::Module::M_WINDOW))

NATIVE_DEF(Window_FromPixels, 2) {
  double px = vm::ConvertArg<double>(args, 0);
  double py = vm::ConvertArg<double>(args, 1);
  
  double wx = 0.0, wy = 0.0;
  instance()->fromPixels(px, py, wx, wy);

  Dart_Handle ret = Dart_NewList(2);
  Dart_ListSetAt(ret, 0, Dart_NewDouble(wx));
  Dart_ListSetAt(ret, 1, Dart_NewDouble(wy));
  Dart_SetReturnValue(args, ret);
}

NATIVE_DEF(Window_GetDesktopDimensions, 1) {
  int display = vm::ConvertArg<int>(args, 0);
  int w = 0, h = 0;
  instance()->getDesktopDimensions(display, w, h);

  Dart_Handle ret = Dart_NewList(2);
  Dart_ListSetAt(ret, 0, Dart_NewInteger(w));
  Dart_ListSetAt(ret, 1, Dart_NewInteger(h));
  Dart_SetReturnValue(args, ret);
}

NATIVE_DEF(Window_GetDisplayCount, 0) {
  int64_t count = static_cast<int64_t>(instance()->getDisplayCount());
  Dart_SetReturnValue(args, Dart_NewInteger(count));
}

NATIVE_DEF(Window_GetDisplayName, 1) {
  int display = vm::ConvertArg<int>(args, 0);
  const char* name = instance()->getDisplayName(display);

  Dart_Handle ret = Dart_NewStringFromCString(name);
  Dart_SetReturnValue(args, ret);
}

NATIVE_DEF(Window_GetFullscreen, 0) {
  int w, h;
  love::window::WindowSettings settings;
  instance()->getWindow(w, h, settings);

  const char *typestr;
  if (!love::window::Window::getConstant(settings.fstype, typestr)) {
    vm::PopError("Unknown fullscreen type.");
  }

  Dart_Handle ret = Dart_NewList(2);
  Dart_ListSetAt(ret, 0, Dart_NewBoolean(settings.fullscreen));
  Dart_ListSetAt(ret, 1, Dart_NewStringFromCString(typestr));
  Dart_SetReturnValue(args, ret);
}

NATIVE_DEF(Window_GetFullscreenModes, 1) {
  int display = vm::ConvertArg<int>(args, 0);
  const char* name = instance()->getDisplayName(display);

  std::vector<love::window::Window::WindowSize> modes =
    instance()->getFullscreenSizes(display);

  Dart_Handle list = Dart_NewList(static_cast<intptr_t>(modes.size()));

  for (size_t i = 0; i < modes.size(); i++) {
    Dart_Handle item = Dart_NewList(2);
    Dart_ListSetAt(item, 0, Dart_NewInteger(modes[i].width));
    Dart_ListSetAt(item, 1, Dart_NewInteger(modes[i].height));
    Dart_ListSetAt(list, i, item);
  }

  Dart_SetReturnValue(args, list);
}

NATIVE_DEF(Window_GetIcon, 0) {
  // todo
}

NATIVE_DEF(Window_GetMode, 0) {
  using ::love::window::Window;

  int w, h;
  love::window::WindowSettings settings;
  instance()->getWindow(w, h, settings);
  
  // there is no way to manipulate map using dart embedding
  // api, so the workaround is to use json.
  
  const char *fstypestr = "desktop";
  Window::getConstant(settings.fstype, fstypestr);

  std::stringstream ss;
  ss << std::boolalpha;
  ss << "{ \"width\" : " << w << ", \"height\" : " << h << ", \"flags\" : ";
  ss << "{ \"fullscreen\" : " << settings.fullscreen << ",";
  ss << " \"fullscreentype\" : \"" << fstypestr << "\",";
  ss << " \"vsync\" : " << settings.vsync << ",";
  ss << " \"fsaa\" : " << settings.msaa << ",";
  ss << " \"resizable\" : " << settings.resizable << ",";
  ss << " \"borderless\" : " << settings.borderless << ",";
  ss << " \"centered\" : " << settings.centered << ",";
  ss << " \"display\" : " << settings.display << ",";
  ss << " \"minwidth\" : " << settings.minwidth << ",";
  ss << " \"minheight\" : " << settings.minheight << ",";
  ss << " \"highdpi\" : " << settings.highdpi << ",";
  ss << " \"refreshrate\" : " << settings.refreshrate << ",";
  ss << " \"x\" : " << settings.x << ",";
  ss << " \"y\" : " << settings.y << " } }";

  Dart_SetReturnValue(args, Dart_NewStringFromCString(ss.str().c_str()));
}

NATIVE_DEF(Window_GetPixelScale, 0) {
  Dart_SetReturnValue(args, Dart_NewDouble(instance()->getPixelScale()));
}

NATIVE_DEF(Window_GetPosition, 0) {
  int x = 0;
  int y = 0;
  int display = 0;
  instance()->getPosition(x, y, display);
  
  Dart_Handle list = Dart_NewList(3);
  Dart_ListSetAt(list, 0, Dart_NewInteger(x));
  Dart_ListSetAt(list, 1, Dart_NewInteger(y));
  Dart_ListSetAt(list, 2, Dart_NewInteger(display));

  Dart_SetReturnValue(args, list);
}

NATIVE_DEF(Window_GetTitle, 0) {
  Dart_Handle str = Dart_NewStringFromCString(
    instance()->getWindowTitle().c_str());
  Dart_SetReturnValue(args, str);
}

NATIVE_DEF(Window_HasFocus, 0) {
  bool result = instance()->hasFocus();
  Dart_SetReturnValue(args, Dart_NewBoolean(result));
}

NATIVE_DEF(Window_HasMouseFocus, 0) {
  bool result = instance()->hasMouseFocus();
  Dart_SetReturnValue(args, Dart_NewBoolean(result));
}

NATIVE_DEF(Window_IsCreated, 0) {
  bool result = instance()->isOpen();
  Dart_SetReturnValue(args, Dart_NewBoolean(result));
}

NATIVE_DEF(Window_IsVisible, 0) {
  bool result = instance()->isVisible();
  Dart_SetReturnValue(args, Dart_NewBoolean(result));
}

NATIVE_DEF(Window_Minimize, 0) {
  instance()->minimize();
}

NATIVE_DEF(Window_Maximize, 0) {
  instance()->maximize();
}

NATIVE_DEF(Window_SetFullscreen, 2) {
  using ::love::window::Window;

  bool fullscreen = vm::ConvertArg<bool>(args, 0);
  bool exclusive = vm::ConvertArg<bool>(args, 1);

  Window::FullscreenType fstype = Window::FULLSCREEN_MAX_ENUM;

  const char *typestr = exclusive ? "exclusive" : "desktop";
  if (typestr && !Window::getConstant(typestr, fstype)) {
    vm::PopError("Invalid fullscreen type");
  }
  
  bool success = false;
  if (fstype == Window::FULLSCREEN_MAX_ENUM) {
    success = instance()->setFullscreen(fullscreen);
  }
  else {
    success = instance()->setFullscreen(fullscreen, fstype);
  }

  Dart_SetReturnValue(args, Dart_NewBoolean(success));
}

NATIVE_DEF(Window_SetIcon, 0) {
  // todo
}

NATIVE_DEF(Window_SetMode, 3) {
  using ::love::window::Window;
  using ::love::window::WindowSettings;
  
  int w = vm::ConvertArg<int>(args, 0);
  int h = vm::ConvertArg<int>(args, 1);
  bool success = false;
  Dart_Handle flags = Dart_GetNativeArgument(args, 2);
  if (Dart_IsNull(flags)) {
    success = instance()->setWindow(w, h, nullptr);
  }
  else if (Dart_IsMap(flags)) {
    WindowSettings settings;
    settings.fullscreen =
      vm::MapConvertAt<bool>(flags, "fullscreen", settings.fullscreen);
    settings.fstype = static_cast<Window::FullscreenType>(
      vm::MapConvertAt<int>(flags, "fstype",
        static_cast<int>(settings.fstype)));
    settings.vsync =
      vm::MapConvertAt<bool>(flags, "vsync", settings.vsync);
    settings.msaa =
      vm::MapConvertAt<int>(flags, "fsaa", settings.msaa);
    settings.resizable =
      vm::MapConvertAt<bool>(flags, "resizable", settings.resizable);
    settings.borderless =
      vm::MapConvertAt<bool>(flags, "borderless", settings.borderless);
    settings.centered =
      vm::MapConvertAt<bool>(flags, "centered", settings.centered);
    settings.display =
      vm::MapConvertAt<bool>(flags, "display", settings.display);
    settings.minwidth =
      vm::MapConvertAt<int>(flags, "minwidth", settings.minwidth);
    settings.minheight =
      vm::MapConvertAt<int>(flags, "minheight", settings.minheight);
    settings.highdpi =
      vm::MapConvertAt<bool>(flags, "highdpi", settings.highdpi);
    settings.x =
      vm::MapConvertAt<int>(flags, "x", settings.x);
    settings.y =
      vm::MapConvertAt<int>(flags, "y", settings.y);
    settings.useposition =
      vm::MapContainsKey(flags, "x") && vm::MapContainsKey(flags, "y");
    success = instance()->setWindow(w, h, &settings);
  }
  Dart_SetReturnValue(args, Dart_NewBoolean(success));
}

NATIVE_DEF(Window_SetPosition, 3) {
  int x = vm::ConvertArg<int>(args, 0);
  int y = vm::ConvertArg<int>(args, 1);
  int display = vm::ConvertArg<int>(args, 2);
  instance()->setPosition(x, y, display);
}

NATIVE_DEF(Window_SetTitle, 1) {
  std::string str = vm::ConvertArgString(args, 0);
  instance()->setWindowTitle(str);
}

NATIVE_DEF(Window_ShowMessageBox, 0) {
  // todo
}

NATIVE_DEF(Window_ToPixels, 2) {
  double wx = vm::ConvertArg<double>(args, 0);
  double wy = vm::ConvertArg<double>(args, 1);
  double px = 0.0, py = 0.0;
  instance()->toPixels(wx, wy, px, py);

  Dart_Handle ret = Dart_NewList(2);
  Dart_ListSetAt(ret, 0, Dart_NewDouble(px));
  Dart_ListSetAt(ret, 1, Dart_NewDouble(py));
  Dart_SetReturnValue(args, ret);
}

}  // namespace window
}  // namespace dusk
