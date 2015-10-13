
// dusk
#include "nativeutils.h"

// std
#include <stdarg.h>
#include <stdio.h>

namespace dusk {
namespace vm {

Dart_Handle PopError(const char* msg, ...) {
  va_list argList;
  char buffer[256] = { 0 };
  va_start(argList, msg);
  vsprintf(buffer, msg, argList);
  va_end(argList);
  Dart_Handle ret = Dart_PropagateError(Dart_NewApiError(buffer));
  // should be unreachable
  return ret;
}

static void WeakPersistentHandleFinalizer(
  void* isolate_callback_data,
  Dart_WeakPersistentHandle handle,
  void* peer) {
  love::Proxy* p = reinterpret_cast<love::Proxy*>(peer);
  printf("WeakPersistentHandleFinalizer %d\n", p->object->getReferenceCount());
  p->object->release();
  printf("Released %d\n", p->object->getReferenceCount());
  delete p;
}

// Associate a proxy object to a dart object,
// proxy object will be released when dart object
// is garbage collected.
// todo : test this, confirm size is only a hint
Dart_WeakPersistentHandle RegisterGC(
  Dart_Handle dartObject,
  love::Proxy* proxy,
  intptr_t sizeHint) {
  return Dart_NewWeakPersistentHandle(
    dartObject,
    reinterpret_cast<void*>(proxy),
    sizeHint,
    WeakPersistentHandleFinalizer);
}

bool ArgIsLoveType(Dart_NativeArguments args, int index, love::Type type) {
  // get pointer
  Dart_Handle handle = Dart_GetNativeArgument(args, index);
  intptr_t ptr = dart::bin::DartUtils::GetIntptrValue(handle);
  // check type
  love::Proxy *u = reinterpret_cast<love::Proxy*>(ptr);
  return love::typeFlags[u->type][type];
}

}  // namespace vm
}  // namespace dusk
