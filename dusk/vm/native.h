
#ifndef DUSK_VM_NATIVE_H
#define DUSK_VM_NATIVE_H

// std
#include <stdio.h>
#include <string.h>

// dart
#include "dart_api.h"

// For building name of native functions
#define NATIVE_NAME(name) DuskNative_##name

// For building table of native functions
#define NATIVE_ENTRY(name, count)                                             \
  { "DuskNative_"#name, count, NATIVE_NAME(name) },

// For forward declaration of native functions
#define NATIVE_DECL(name, count)                                              \
  void NATIVE_NAME(name)(Dart_NativeArguments args);

// For defination of native functions
#define NATIVE_DEF(name, count)                                               \
  void NATIVE_NAME(name)(Dart_NativeArguments args)

#define REG_ENTRIES(NativeEntries)                                            \
for (int i = 0; i < sizeof(NativeEntries) / sizeof(NativeEntry); ++i) {       \
  vm::LibraryRegistry::RegisterNativeFunction(                                \
    NativeEntries[i].name,                                                    \
    NativeEntries[i].numArgs,                                                 \
    NativeEntries[i].function);                                               \
}

// Struct to represent a native function
typedef struct {
  const char* name;
  int numArgs;
  Dart_NativeFunction function;
} NativeEntry;

#endif // !NATIVE_H
