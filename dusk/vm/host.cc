
// dusk
#include "dusk/vm/host.h"
#include "dusk/vm/libregistry.h"

// std
#include <stdio.h>
#include <string.h>

// dart
#include "bin/dartutils.h"
#include "bin/thread.h"
#include "bin/builtin.h"
#include "bin/vmservice_impl.h"
#include "bin/isolate_data.h"
#include "bin/dbg_connection.h"
#include "bin/platform.h"
#include "bin/process.h"
#include "bin/eventhandler.h"
#include "platform/hashmap.h"
#include "platform/assert.h"

namespace dart {
namespace bin {
  extern const uint8_t* vm_isolate_snapshot_buffer;
  extern const uint8_t* isolate_snapshot_buffer;
}  // namespace bin
}  // namespace dart

static const char* DEFAULT_VM_SERVICE_SERVER_IP = "127.0.0.1";
static const int DEFAULT_VM_SERVICE_SERVER_PORT = 8181;
// VM Service options.
static bool start_vm_service = false;
static const char* vm_service_server_ip = DEFAULT_VM_SERVICE_SERVER_IP;
static int vm_service_server_port = -1;

namespace dusk {
namespace vm {

const char* vm_flags[] = {
  "--enable_type_checks",
  "--no_write_protect_code",
};
const bool has_trace_loading = false;
char* script_name = "main.dart";
const char* commandline_package_root;
const char* commandline_packages_file;
// todo : change these constants to args?

static dart::HashMap* environment = NULL;
static Dart_Isolate isolate_;
// todo : this doesn't need to be static

Host::Host() {
  // todo : initialize environment
}

bool Host::Initialize()
{
  if (initialized_) return false;

  dart::bin::Thread::InitOnce();

  if (!dart::bin::DartUtils::SetOriginalWorkingDirectory()) {
    printf("Error determining current directory\n");
    exit(1);
  }

  Dart_SetVMFlags((int)(sizeof(vm_flags) / sizeof(vm_flags[0])), vm_flags);
  //Dart_SetVMFlags(0, 0); // fixme

  // start event handler
  dart::bin::EventHandler::Start();

  // no debugger // todo : add debugger?
  dart::bin::DebuggerConnectionHandler::InitForVmService();

  if (!Dart_Initialize(
    dart::bin::vm_isolate_snapshot_buffer,
    CreateIsolateAndSetup,
    0,
    0,
    0,
    dart::bin::DartUtils::OpenFile,
    dart::bin::DartUtils::ReadFile,
    dart::bin::DartUtils::WriteFile,
    dart::bin::DartUtils::CloseFile,
    dart::bin::DartUtils::EntropySource))
  {
    printf("Dart initialzation failed\n");
    return false;
  }
  initialized_ = true;
  return true;
}

void Host::Start() {
  // ensure vm dart initialized
  if (!initialized_) {
    bool ret = Initialize();
    if (!ret) return;
  }

  char* error = 0;
  int exit_code = 0;

  Dart_Isolate isolate = CreateIsolateAndSetupHelper(script_name,
    "main",
    commandline_package_root,
    commandline_packages_file,
    NULL,
    &error,
    &exit_code);

  if (isolate == 0) {
    printf("create isolate error %s\n", error);
    free(error);
    exit(exit_code);
  }

  Dart_EnterIsolate(isolate);
  Dart_EnterScope();

  Dart_Handle root_lib = Dart_RootLibrary();
  Dart_Handle builtin_lib =
    dart::bin::Builtin::LoadAndCheckLibrary(dart::bin::Builtin::kBuiltinLibrary);
  Dart_Handle result = Dart_LibraryImportLibrary(builtin_lib, root_lib, Dart_Null());

  // force precompile
  //result = Dart_Precompile();

  if (Dart_IsNull(root_lib)) {
    printf("Unable to find root library for '%s'\n", script_name);
  }

  Dart_ExitScope();
  Dart_ExitIsolate();
}

void Host::OnLoad()
{
  Dart_EnterIsolate(isolate_);
  Dart_EnterScope();

  // invoke draw
  VmInvoke("load", 0, 0);

  Dart_RunLoop();

  Dart_ExitScope();
  Dart_ExitIsolate();
}

void Host::OnUpdate(double dt)
{
  Dart_EnterIsolate(isolate_);
  Dart_EnterScope();

  // invoke update
  Dart_Handle args[1] = { Dart_NewDouble(dt) };
  VmInvoke("update", 1, args);

  Dart_RunLoop();

  Dart_ExitScope();
  Dart_ExitIsolate();
}

void Host::OnDraw()
{
  Dart_EnterIsolate(isolate_);
  Dart_EnterScope();

  // invoke draw
  VmInvoke("draw", 0, 0);

  Dart_RunLoop();

  Dart_ExitScope();
  Dart_ExitIsolate();
}

void Host::OnEvent(std::string name, int64_t a = 0, int64_t b = 0, int64_t c = 0, int64_t d = 0)
  // todo : proper arguments
{
  Dart_EnterIsolate(isolate_);
  Dart_EnterScope();

  // invoke event
  Dart_Handle args[5] = {
    Dart_NewStringFromCString(name.c_str()),
    Dart_NewInteger(a),
    Dart_NewInteger(b),
    Dart_NewInteger(c),
    Dart_NewInteger(d),
  };
  VmInvoke("event", 5, args);

  Dart_RunLoop();
  // todo : need runloop here?

  Dart_ExitScope();
  Dart_ExitIsolate();
}

void Host::Stop()
{
  Dart_EnterIsolate(isolate_);
  Dart_ShutdownIsolate();

  dart::bin::Process::TerminateExitCodeHandler();
  Dart_Cleanup();

  isolate_ = 0;
  initialized_ = false;

  // free environment if any.
  if (environment != NULL) {
    for (dart::HashMap::Entry* p = environment->Start();
      p != NULL;
      p = environment->Next(p)) {
      free(p->key);
      free(p->value);
    }
    delete environment;
  }

  exit(dart::bin::Process::GlobalExitCode());
}

void Host::VmInvoke(const char* function, int argc, Dart_Handle* argv) {
  Dart_EnterScope();
  Dart_Handle library = Dart_RootLibrary();
  Dart_Handle name = Dart_NewStringFromCString(function);
  Dart_Handle result = Dart_Invoke(library, name, argc, argv);

  if (Dart_IsError(result)) {
    if (Dart_IsApiError(result)) {
      // keep silent since function not found is allowed
      // todo : other errors?
      // printf("Dart API Error: %s\n", Dart_GetError(result));
    }
    else if (Dart_IsUnhandledExceptionError(result)) {
      printf("Dart Unhandled Exception: %s\n", Dart_GetError(result));
    }
    else if (Dart_IsCompilationError(result)) {
      printf("Dart Compilation Error: %s\n", Dart_GetError(result));
    }
    else if (Dart_IsFatalError(result)) {
      printf("Dart Fatal Error: %s\n", Dart_GetError(result));
    }
  }

  Dart_ExitScope();
}

void Host::ShutdownIsolate(void* callback_data) {
  dart::bin::IsolateData* isolate_data = reinterpret_cast<dart::bin::IsolateData*>(callback_data);
  delete isolate_data;
}

Dart_Isolate Host::CreateIsolateAndSetupHelper(const char* script_uri,
  const char* main,
  const char* package_root,
  const char* packages_file,
  Dart_IsolateFlags* flags,
  char** error,
  int* exit_code)
{
  dart::bin::IsolateData* isolate_data = new dart::bin::IsolateData(script_uri,
    package_root,
    packages_file);

  Dart_Isolate isolate = Dart_CreateIsolate(script_uri,
    main,
    dart::bin::isolate_snapshot_buffer, // todo
    flags,
    isolate_data,
    error);

  if (isolate == NULL) {
    printf("Cannot create isolate: %s", *error);
    return NULL;
  }

  Dart_EnterScope();

  if (dart::bin::isolate_snapshot_buffer != NULL) { // todo
    // Setup the native resolver as the snapshot does not carry it.
    dart::bin::Builtin::SetNativeResolver(dart::bin::Builtin::kBuiltinLibrary);
    dart::bin::Builtin::SetNativeResolver(dart::bin::Builtin::kIOLibrary);
  }

  // set up the library tag handler for this isolate.
  Dart_Handle result = Dart_SetLibraryTagHandler(
    dusk::vm::LibraryRegistry::LibraryTagHandler);

  if (Dart_IsError(result)) {
    *error = strdup(Dart_GetError(result));
    *exit_code = 1;
    return NULL;
  }

  // for service isolate
  if (Dart_IsServiceIsolate(isolate)) {
    // If this is the service isolate, load embedder specific bits and return.
    if (!dart::bin::VmService::Setup(vm_service_server_ip, vm_service_server_port)) {
      *error = strdup(dart::bin::VmService::GetErrorMessage());
      return NULL;
    }

    Dart_ExitScope();
    Dart_ExitIsolate();
    return isolate;
  }

  // main isolate handle
  isolate_ = isolate;

  // load builtin library
  Dart_Handle builtin_lib =
    dart::bin::Builtin::LoadAndCheckLibrary(dart::bin::Builtin::kBuiltinLibrary);
  if (Dart_IsError(builtin_lib)) {
    *error = strdup(Dart_GetError(builtin_lib));
    *exit_code = 1;
    return NULL;
  }

  // todo : load other libraries?

  // prepare for script loading
  result = dart::bin::DartUtils::PrepareForScriptLoading(package_root,
    packages_file,
    false,
    has_trace_loading,
    builtin_lib);
  if (Dart_IsError(result)) {
    *error = strdup(Dart_GetError(result));
    *exit_code = 1;
    return NULL;
  }

  // environment callback enables embedder to provide value // todo ; add this feature
  result = Dart_SetEnvironmentCallback(EnvironmentCallback);
  if (Dart_IsError(result)) {
    *error = strdup(Dart_GetError(result));
    *exit_code = 1;
    return NULL;
  }

  // load the script.
  result = dart::bin::DartUtils::LoadScript(script_uri, builtin_lib);
  if (Dart_IsError(result)) {
    *error = strdup(Dart_GetError(result));
    *exit_code = 1;
    return NULL;
  }

  // Run event-loop and wait for script loading to complete.
  result = Dart_RunLoop();
  if (Dart_IsError(result)) {
    *error = strdup(Dart_GetError(result));
    *exit_code = 1;
    return NULL;
  }

  dart::bin::Platform::SetPackageRoot(package_root);

  dart::bin::DartUtils::SetupIOLibrary(script_uri);

  Dart_ExitScope();
  Dart_ExitIsolate();

  // make the isolate runnable so that it is ready to handle messages.
  bool ret = Dart_IsolateMakeRunnable(isolate);
  if (!ret) {
    *error = strdup("Invalid isolate state - Unable to make it runnable");
    Dart_EnterIsolate(isolate);
    Dart_ShutdownIsolate();
    return NULL;
  }

  return isolate;
}

Dart_Isolate Host::CreateIsolateAndSetup(const char* script_uri,
  const char* main,
  const char* package_root,
  Dart_IsolateFlags* flags,
  void* data, char** error)
{
  // The VM should never call the isolate helper with a NULL flags.
  ASSERT(flags != NULL);
  ASSERT(flags->version == DART_FLAGS_CURRENT_VERSION);
  dart::bin::IsolateData* parent_isolate_data = reinterpret_cast<dart::bin::IsolateData*>(data);
  int exit_code = 0;
  if (script_uri == NULL) {
    if (data == NULL) {
      *error = strdup("Invalid 'callback_data' - Unable to spawn new isolate");
      return NULL;
    }
    script_uri = parent_isolate_data->script_url;
    if (script_uri == NULL) {
      *error = strdup("Invalid 'callback_data' - Unable to spawn new isolate");
      return NULL;
    }
  }
  const char* packages_file = NULL;
  if (package_root == NULL) {
    if (parent_isolate_data != NULL) {
      package_root = parent_isolate_data->package_root;
      packages_file = parent_isolate_data->packages_file;
    }
  }
  return CreateIsolateAndSetupHelper(script_uri,
    main,
    package_root,
    packages_file,
    flags,
    error,
    &exit_code);
}

Dart_Handle Host::EnvironmentCallback(Dart_Handle name) {
  uint8_t* utf8_array;
  intptr_t utf8_len;
  Dart_Handle result = Dart_Null();
  Dart_Handle handle = Dart_StringToUTF8(name, &utf8_array, &utf8_len);
  if (Dart_IsError(handle)) {
    handle = Dart_ThrowException(
      dart::bin::DartUtils::NewDartArgumentError(Dart_GetError(handle)));
  }
  else {
    char* name_chars = reinterpret_cast<char*>(malloc(utf8_len + 1));
    memmove(name_chars, utf8_array, utf8_len);
    name_chars[utf8_len] = '\0';
    const char* value = NULL;
    if (environment != NULL) {
      dart::HashMap::Entry* entry = environment->Lookup(
        reinterpret_cast<void*>(name_chars),
        dart::HashMap::StringHash(name_chars),
        false);
      if (entry != NULL) {
        value = reinterpret_cast<char*>(entry->value);
      }
    }
    if (value != NULL) {
      result = Dart_NewStringFromUTF8(reinterpret_cast<const uint8_t*>(value),
        strlen(value));
    }
    free(name_chars);
  }
  return result;
}

}  // namespace vm
}  // namespace dusk