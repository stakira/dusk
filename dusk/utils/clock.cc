
#include "clock.h"
#include <windows.h>

namespace dusk {
namespace utils {

double Clock::then = 0;

#ifdef _MSC_VER
static LARGE_INTEGER getFrequency() {
  LARGE_INTEGER frequency;
  QueryPerformanceFrequency(&frequency);
  return frequency;
}

double Clock::getTime() {
  // Force the following code to run on first core
  // (see http://msdn.microsoft.com/en-us/library/windows/desktop/ms644904(v=vs.85).aspx)
  HANDLE currentThread = GetCurrentThread();
  DWORD_PTR previousMask = SetThreadAffinityMask(currentThread, 1);

  // Get the frequency of the performance counter
  // (it is constant across the program lifetime)
  static LARGE_INTEGER frequency = getFrequency();

  // Get the current time
  LARGE_INTEGER time;
  QueryPerformanceCounter(&time);

  // Restore the thread affinity
  SetThreadAffinityMask(currentThread, previousMask);

  // Return the current time as milliseconds
  return 1000.0 * time.QuadPart / frequency.QuadPart;
}

#elif __APPLE__
// todo : linux and apple code are not tested
double Clock::getTime() {
  static mach_timebase_info_data_t frequency = {0, 0};
  if (frequency.denom == 0)
    mach_timebase_info(&frequency);
  Uint64 nanoseconds =
    mach_absolute_time() * frequency.numer / frequency.denom;
  return static_cast<double>(nanoseconds) / 1000000.0;
}

#elif __linux__
// todo : linux and apple code are not tested
double Clock::getTime() {
  timespec time;
  clock_gettime(CLOCK_MONOTONIC, &time);
  return static_cast<Uint64>(time.tv_sec) * 1000.0 +
    time.tv_nsec / 1000000.0;
}

#endif  // _MSC_VER

double Clock::getDt() {
  double now = getTime();
  double dt = now - then;
  then = now;
  return dt;
}

}  // namespace utils
}  // namespace dusk
