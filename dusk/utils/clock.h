
#ifndef DUSK_CLOCK_H
#define DUSK_CLOCK_H

namespace dusk {
namespace utils {

class Clock {
 public:
  static double getTime();

  static double getDt();

 private:
  static double then;
};

}  // namespace utils
}  // namespace dusk

#endif