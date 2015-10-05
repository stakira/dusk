
#include "dusk.h"

int main(int argc, char** argv) {
	dusk::DuskDelegate *duskDelegate = new dusk::DuskDelegate();
	duskDelegate->Initialize();

	duskDelegate->Run();

	duskDelegate->Cleanup();
	delete duskDelegate;

	return 0;
}
