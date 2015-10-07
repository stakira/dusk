
#ifndef DUSK_VM_HOST_H
#define DUSK_VM_HOST_H

// std
#include <string>

// dart
#include "include/dart_api.h"

namespace dusk {
namespace vm {

class Host {

public:

  Host();

  bool Initialize();

  void Start();

  void OnLoad();

  void OnUpdate(double dt);

  void OnDraw();

  void OnEvent(std::string name, int64_t type, int64_t a, int64_t b, int64_t c);

  void Stop();

private:

  void VmInvoke(const char* function, int argc, Dart_Handle* argv);

  // callbacks

  static void ShutdownIsolate(void* callback_data);

  static Dart_Isolate CreateIsolateAndSetupHelper(
    const char* script_uri,
    const char* main,
    const char* package_root,
    const char* packages_file,
    Dart_IsolateFlags* flags,
    char** error,
    int* exit_code);

  static Dart_Isolate CreateIsolateAndSetup(
    const char* script_uri,
    const char* main,
    const char* package_root,
    Dart_IsolateFlags* flags,
    void* data,
    char** error);

  static Dart_Handle EnvironmentCallback(Dart_Handle name);

  bool initialized_ = false;

};

}
}

#endif  // DUSK_VM_HOST_H
