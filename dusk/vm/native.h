
#ifndef DUSK_VM_NATIVE_H
#define DUSK_VM_NATIVE_H

#include <stdio.h>
#include <string.h>

// For building name of native functions
#define NATIVE_NAME(name) DuskNative_##name

// For building table of native functions
#define NATIVE_ENTRY(name, count)                                             \
    { "DuskNative_"#name, NATIVE_NAME(name), count },

// For forward declaration of native functions
#define NATIVE_DECL(name, count)                                              \
  void NATIVE_NAME(name)(Dart_NativeArguments args);

// For defination of native functions
#define NATIVE_DEF(name, count)                                               \
  void NATIVE_NAME(name)(Dart_NativeArguments args)

#define NATIVE_LOOKUP(entries)                                                \
static Dart_NativeFunction NativeLookup(Dart_Handle name,                     \
  int argument_count,                                                         \
  bool* auto_setup_scope) {                                                   \
  const char* function_name = 0;                                              \
  Dart_Handle result = Dart_StringToCString(name, &function_name);            \
  DART_CHECK_VALID(result);                                                   \
  *auto_setup_scope = true;                                                   \
  int num_entries = sizeof((entries)) / sizeof(struct NativeEntry);           \
  for (int i = 0; i < num_entries; i++) {                                     \
    struct NativeEntry* entry = &((entries)[i]);                              \
    if (!strcmp(function_name, entry->name_) &&                               \
      (entry->argument_count_ == argument_count)) {                           \
      return reinterpret_cast<Dart_NativeFunction>(entry->function_);         \
    }                                                                         \
  }                                                                           \
  return 0;                                                                   \
}

#define NATIVE_SYMBOL(entries)                                                \
static const uint8_t* NativeSymbol(Dart_NativeFunction nf) {                  \
int num_entries = sizeof((entries)) / sizeof(struct NativeEntry);             \
  for (int i = 0; i < num_entries; i++) {                                     \
    struct NativeEntry* entry = &((entries)[i]);                              \
    if (reinterpret_cast<Dart_NativeFunction>(entry->function_) == nf) {      \
      return reinterpret_cast<const uint8_t*>(entry->name_);                  \
    }                                                                         \
  }                                                                           \
  return 0;                                                                   \
}

#endif // !NATIVE_H
