
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

#define GfxInst() (love::Module::getInstance<love::graphics::opengl::Graphics>(love::Module::M_GRAPHICS))
#define FontInst() (love::Module::getInstance<love::font::Font>(love::Module::M_FONT))
#define FsysInst() (love::Module::getInstance<love::filesystem::Filesystem>(love::Module::M_FILESYSTEM))

// Load a truetype font and returns a rasterizer.
// Returns nullptr if failed.
love::font::Rasterizer* LoadNewTrueTypeRasterizer(
  std::string filename, int size, intptr_t* datasize) {
  using ::love::graphics::opengl::Font;
  using ::love::filesystem::File;
  using ::love::font::TrueTypeRasterizer;
  using ::love::font::Rasterizer;
    
  // open font file
  File *file = nullptr;
  file = FsysInst()->newFile(filename.c_str());

  if (!file->open(File::MODE_READ)) {
    file->release();
    return nullptr; // fail to open file
  }

  // load font data
  love::Data* d = nullptr;
  try {
    d = file->read();
  }
  catch (love::Exception e) {
    printf("file read failed %s\n", e.what());
    file->release();
    return nullptr;
  }

  file->release();
  
  Rasterizer* rasterizer = FontInst()->newTrueTypeRasterizer(
    d, size, TrueTypeRasterizer::HINTING_NORMAL);
  printf("rasterizer created\n");

  *datasize = d->getSize();
  d->release();

  return rasterizer;
}

/**
* Font
*/

NATIVE_DEF(Graphics_NewFont, 3) {
  using ::love::graphics::opengl::Font;

  // todo : support only font filename and default font at the moment
  int size = vm::ConvertArg<int>(args, 1);
  Dart_Handle dartObject = Dart_GetNativeArgument(args, 2);
  printf("font size %d\n", size);

  intptr_t datasize = 0;

  love::font::Rasterizer* rasterizer = nullptr;
  if (Dart_IsNull(Dart_GetNativeArgument(args, 0))) {
    // use default font
    rasterizer = FontInst()->newTrueTypeRasterizer(
      size, love::font::TrueTypeRasterizer::HINTING_NORMAL);
  }
  else {
    std::string filename = vm::ConvertArgString(args, 0);
    rasterizer = LoadNewTrueTypeRasterizer(filename, size, &datasize);
    if (rasterizer == nullptr) {
      vm::PopError("Cannot load font file: %s", filename.c_str());
    }
  }

  Font* font = GfxInst()->newFont(
    rasterizer, GfxInst()->getDefaultFilter());

  love::Proxy* u = new love::Proxy();

  font->retain();
  u->object = font;
  u->type = love::Type::GRAPHICS_FONT_ID;

  Dart_SetReturnValue(args,
    Dart_NewInteger(reinterpret_cast<intptr_t>(u)));

  vm::RegisterGC(dartObject, u, datasize);
  printf("gc registered\n");
  font->release();
}

NATIVE_DEF(Graphics_SetFont, 1) {
  using ::love::graphics::opengl::Font;
  Font* font = vm::ConvertArgLoveType<Font>(
    args, 0, love::Type::GRAPHICS_FONT_ID);
  GfxInst()->setFont(font);
}

NATIVE_DEF(Graphics_GetFont, 0) {
  using ::love::graphics::opengl::Font;
  Font* font = GfxInst()->getFont();
  // todo
  // the original dart object may have being garbage
  // colleted, we just create a new dart object
  // or just memorize it in dart library?
}

}  // namespace graphics
}  // namespace dusk
