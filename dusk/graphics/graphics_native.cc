
// dusk
#include "graphics_native.h"
#include "graphics_source.h"
#include "dusk/vm/nativeutils.h"
#include "dusk/vm/libregistry.h"

// dart
#include "dart_api.h"

// love
#include "common/Module.h"
#include "modules/graphics/Graphics.h"

// fix gladfuncs.hpp
#ifdef near
#undef near
#endif

#ifdef far
#undef far
#endif

#include "modules/graphics/opengl/Graphics.h"
#include "modules/filesystem/Filesystem.h"
#include "modules/filesystem/physfs/File.h"
#include "modules/font/freetype/Font.h"

namespace dusk {
namespace graphics {

NativeEntry GraphicsNativeEntries[] = {
  GRAPHICS_NATIVE_LIST(NATIVE_ENTRY)
};

void Register() {
  vm::LibraryRegistry::RegisterLibrary("graphics", source);
  REG_ENTRIES(GraphicsNativeEntries)
}

#define instance() (love::Module::getInstance<love::graphics::opengl::Graphics>(love::Module::M_GRAPHICS))

inline love::graphics::Graphics::DrawMode GetMode(bool filled) {
  return filled ?
    love::graphics::Graphics::DrawMode::DRAW_FILL :
    love::graphics::Graphics::DrawMode::DRAW_LINE;
}

/**
 * Drawing
 */

NATIVE_DEF(Graphics_Arc, 7) {
  bool filled = vm::ConvertArg<bool>(args, 0);
  double x = vm::ConvertArg<double>(args, 1);
  double y = vm::ConvertArg<double>(args, 2);
  double radius = vm::ConvertArg<double>(args, 3);
  double angle1 = vm::ConvertArg<double>(args, 4);
  double angle2 = vm::ConvertArg<double>(args, 5);
  int64_t points = vm::ConvertArg<int64_t>(args, 6);

  instance()->arc(GetMode(filled), x, y, radius, angle1, angle2, points);
}

NATIVE_DEF(Graphics_Ellipse, 6) {
  bool filled = vm::ConvertArg<bool>(args, 0);
  double x = vm::ConvertArg<double>(args, 1);
  double y = vm::ConvertArg<double>(args, 2);
  double a = vm::ConvertArg<double>(args, 3);
  double b = vm::ConvertArg<double>(args, 4);
  int64_t points = vm::ConvertArg<int64_t>(args, 5);

  instance()->ellipse(GetMode(filled), x, y, a, b, points);
}
  
NATIVE_DEF(Graphics_Circle, 5) {
  bool filled = vm::ConvertArg<bool>(args, 0);
  double x = vm::ConvertArg<double>(args, 1);
  double y = vm::ConvertArg<double>(args, 2);
  double radius = vm::ConvertArg<double>(args, 3);
  int64_t points = vm::ConvertArg<int64_t>(args, 4);

  instance()->circle(GetMode(filled), x, y, radius, points);
}

NATIVE_DEF(Graphics_Clear, 0) {
  // todo : use background color
  love::graphics::Colorf color(0, 0, 0, 0);
  instance()->clear(color);
}

NATIVE_DEF(Graphics_Draw, 0) {

}

NATIVE_DEF(Graphics_Line, 4) {
  std::vector<float> coords(4);

  coords[0] = vm::ConvertArg<float>(args, 0);
  coords[1] = vm::ConvertArg<float>(args, 1);
  coords[2] = vm::ConvertArg<float>(args, 2);
  coords[3] = vm::ConvertArg<float>(args, 3);

  instance()->polyline(&coords[0], coords.size());
}

NATIVE_DEF(Graphics_Point, 2) {
  std::vector<float> coords(2);

  coords[0] = vm::ConvertArg<float>(args, 0);
  coords[1] = vm::ConvertArg<float>(args, 1);

  instance()->points(&coords[0], nullptr, 1);
}

NATIVE_DEF(Graphics_Polygon, 2) {
  bool filled = vm::ConvertArg<bool>(args, 0);

  std::vector<float> coords = vm::ConvertArgList<float>(args, 1);

  if (coords.size() < 6) {
    vm::PopError("Need at least three vertices to draw a polygon");
  }
  else if (coords.size() % 2 != 0) {
    vm::PopError("Number of vertex components must be a multiple of two");
  }

  // close polygon
  coords.push_back(coords[0]);
  coords.push_back(coords[1]);

  instance()->polygon(GetMode(filled), &coords[0], coords.size());
}

NATIVE_DEF(Graphics_Polyline, 1) {
  std::vector<float> coords = vm::ConvertArgList<float>(args, 0);
  
  if (coords.size() < 4) {
    vm::PopError("Need at least two vertices to draw a line");
  }
  else if (coords.size() % 2 != 0) {
    vm::PopError("Number of vertex components must be a multiple of two");
  }

  instance()->polyline(&coords[0], coords.size());
}

NATIVE_DEF(Graphics_Present, 0) {
  instance()->present();
}

NATIVE_DEF(Graphics_Print, 10) {
  std::string text = vm::ConvertArgString(args, 0);
  float x = vm::ConvertArg<float>(args, 1);
  float y = vm::ConvertArg<float>(args, 2);
  float angle = vm::ConvertArg<float>(args, 3);
  float sx = vm::ConvertArg<float>(args, 4);
  float sy = vm::ConvertArg<float>(args, 5);
  float ox = vm::ConvertArg<float>(args, 6);
  float oy = vm::ConvertArg<float>(args, 7);
  float kx = vm::ConvertArg<float>(args, 8);
  float ky = vm::ConvertArg<float>(args, 9);

  instance()->print(text, x, y, angle, sx, sy, ox, oy, kx, ky);
}

NATIVE_DEF(Graphics_Printf, 12) {
  using ::love::graphics::opengl::Font;

  std::string text = vm::ConvertArgString(args, 0);
  float x = vm::ConvertArg<float>(args, 1);
  float y = vm::ConvertArg<float>(args, 2);
  float limit = vm::ConvertArg<float>(args, 3);
  int alignment = vm::ConvertArg<int>(args, 4);
  float angle = vm::ConvertArg<float>(args, 5);
  float sx = vm::ConvertArg<float>(args, 6);
  float sy = vm::ConvertArg<float>(args, 7);
  float ox = vm::ConvertArg<float>(args, 8);
  float oy = vm::ConvertArg<float>(args, 9);
  float kx = vm::ConvertArg<float>(args, 10);
  float ky = vm::ConvertArg<float>(args, 11);

  Font::AlignMode align = static_cast<Font::AlignMode>(alignment);
  instance()->printf(text, x, y, limit, align, angle, sx, sy, ox, oy, kx, ky);
}

NATIVE_DEF(Graphics_Rectangle, 8) {
  bool filled = vm::ConvertArg<bool>(args, 0);
  double x = vm::ConvertArg<double>(args, 1);
  double y = vm::ConvertArg<double>(args, 2);
  double w = vm::ConvertArg<double>(args, 3);
  double h = vm::ConvertArg<double>(args, 4);

  // rounded rectangle
  double rx = vm::ConvertArg<double>(args, 5);
  double ry = vm::ConvertArg<double>(args, 6);

  if (rx == 0 && ry == 0) {
    instance()->rectangle(GetMode(filled), x, y, w, h);
  }
  else {
    int64_t points = vm::ConvertArg<int64_t>(args, 7);
    instance()->rectangle(GetMode(filled), x, y, w, h, rx, ry, points);
  }
}

/**
 * Coordinate system
 */

NATIVE_DEF(Graphics_Origin, 0) {
  instance()->origin();
}

NATIVE_DEF(Graphics_Pop, 0) {
  instance()->pop();
}

NATIVE_DEF(Graphics_Push, 0) {
  using ::love::graphics::opengl::Graphics;
  Graphics::StackType stype = Graphics::STACK_TRANSFORM;
  instance()->push(stype);
}

NATIVE_DEF(Graphics_Rotate, 1) {
  float angle = vm::ConvertArg<float>(args, 0);
  instance()->rotate(angle);
}

NATIVE_DEF(Graphics_Scale, 2) {
  float sx = vm::ConvertArg<float>(args, 0);
  float sy = vm::ConvertArg<float>(args, 1);
  instance()->scale(sx, sy);
}

NATIVE_DEF(Graphics_Shear, 2) {
  float kx = vm::ConvertArg<float>(args, 0);
  float ky = vm::ConvertArg<float>(args, 1);
  instance()->shear(kx, ky);
}

NATIVE_DEF(Graphics_Translate, 2) {
  float x = vm::ConvertArg<float>(args, 0);
  float y = vm::ConvertArg<float>(args, 1);
  instance()->translate(x, y);
}

/**
 * Object creation
 */

NATIVE_DEF(Graphics_NewCanvas, 4) {
  using ::love::graphics::opengl::Canvas;
  
  int w = vm::ConvertArg<int>(args, 0);
  int h = vm::ConvertArg<int>(args, 1);
  int fmt = vm::ConvertArg<int>(args, 2);
  int msaa = vm::ConvertArg<int>(args, 3);

  Canvas::Format format = static_cast<Canvas::Format>(fmt);
  Canvas* canvas = instance()->newCanvas(w, h, format, msaa);

  love::Proxy *u = new love::Proxy();
  u->object = canvas;
  u->type = love::Type::GRAPHICS_CANVAS_ID;

  canvas->release();

  Dart_SetReturnValue(args,
    Dart_NewInteger(reinterpret_cast<intptr_t>(u)));
  
  // todo : RegisterGC
}

}  // namespace graphics
}  // namespace dusk
