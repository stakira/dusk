
// dusk
#include "filesystem_native.h"
#include "filesystem_source.h"
#include "dusk/vm/nativeutils.h"
#include "dusk/vm/libregistry.h"

// std
#include <string>

// dart
#include "bin/dartutils.h"

// love
#include "modules/filesystem/Filesystem.h"

namespace dusk {
namespace filesystem {

NativeEntry FilesystemNativeEntries[] = {
  FILESYSTEM_NATIVE_LIST(NATIVE_ENTRY)
};

#define instance() (love::Module::getInstance<love::filesystem::Filesystem>(love::Module::M_FILESYSTEM))

NATIVE_DEF(Filesystem_OpenFile, 2) {
  using ::love::filesystem::File;

  std::string filename = vm::ConvertArgString(args, 0);
  std::string modestr = vm::ConvertArgString(args, 1);

  File::Mode mode = File::MODE_CLOSED;
  if (!File::getConstant(modestr.c_str(), mode)) {
    vm::PopError("Incorrect file open mode: %s", modestr);
  }

  File *file = instance()->newFile(filename.c_str());

  try
  {
    file->open(mode);
  }
  catch (love::Exception &e)
  {
    file->release();
    vm::PopError("%s", e.what());
  }

  // return the pointer to dart
  Dart_SetReturnValue(args,
    Dart_NewInteger(reinterpret_cast<intptr_t>(file)));
}

NATIVE_DEF(Filesystem_CloseFile, 1) {
  using ::love::filesystem::File;

  // get the pointer from dart
  Dart_Handle handle = Dart_GetNativeArgument(args, 0);
  intptr_t ptr = dart::bin::DartUtils::GetIntptrValue(handle);
  File *file = reinterpret_cast<File*>(ptr);

  // close file
  bool ret = file->close();
  Dart_SetReturnValue(args, Dart_NewBoolean(ret));
  file->release();
}

void Register() {
  vm::LibraryRegistry::RegisterLibrary("filesystem", source);
  REG_ENTRIES(FilesystemNativeEntries)
}

}  // namespace filesystem
}  // namespace dusk
