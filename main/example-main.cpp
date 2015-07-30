
#include "VRCore.h"



void handleEvents(const std::vector<VREvent> &newEvents) {
  for (std::vector<VREvent>::iterator it = inputEvents.begin(); it < inputEvents.end(); it++) {
    if (it->getName() == "Wand_Left_Btn_down") {
      // do something...
    }
    // else if ...
  }
}



void renderGraphics(const VRRenderState &state) {
  cout << "drawing" << endl;
}




int main(int argc, char **argv) {

	VRSettings settings;

	// TODO: Use argc and argv to specify xml files to load for settings

	VRCore::instance()->initialize(settings);
	VRCore::instance()->setEventCallback(&handleEvents);
	VRCore::instance()->setRenderCallback(&renderGraphics);

	bool done = false;
	while (!done) {

		VRCore::instance()->synchronizeAndProcessEvents();

		VRCore::instance()->renderOnAllDisplays();

	}

	VRCore::instance()->shutdown();
}


