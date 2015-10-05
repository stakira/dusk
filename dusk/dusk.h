
#ifndef DUSK_H
#define DUSK_H

#include "dusk/vm/host.h"
#include "window/window.h"

namespace dusk {

class DuskDelegate {

public:

  DuskDelegate();

  void Initialize();

  void Run();

  void Cleanup();

private:

  dusk::vm::Host *vmhost_ = 0;

};

}

#endif // !DUSK_H
