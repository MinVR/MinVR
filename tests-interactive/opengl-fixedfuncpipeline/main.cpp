#include <iostream>
#include <cmath>

#if defined(WIN32)
#define NOMINMAX
#include <WinSock2.h>
#include <windows.h>
#include <GL/gl.h>
#include <gl/GLU.h>
#elif defined(__APPLE__)
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
#endif


#include <main/VRMain.h>
using namespace MinVR;


/** MyVRApp changes the clear color as frames progress. */
class MyVRApp : public VREventHandler, public VRRenderHandler {
public:
	MyVRApp(int argc, char** argv) {
        _main = new VRMain();
        _main->addEventHandler(this);
        _main->addRenderHandler(this);
        _main->initialize(argc,argv);
    }

	void onVREvent(const VRDataIndex &event) {

        event.printStructure();

		// Get the time since application began
		if (event.getName() == "FrameStart") {
			time = event.getValue("ElapsedSeconds");
		}

        // Quit if the escape button is pressed
        else if (event.getName() == "KbdEsc_Down") {
            _main->shutdown();
        }
	}

	void onVRRenderContext(const VRDataIndex &stateData) {
        if (stateData.exists("IsGraphics")) {
        }
	}

	void onVRRenderScene(const VRDataIndex &stateData) {
        if (stateData.exists("IsGraphics")) {
            VRFloatArray projMat = stateData.getValue("ProjectionMatrix");
            VRFloatArray viewMat = stateData.getValue("ViewMatrix");

            // Show gradient of red color over four seconds then restart
            float red = (float)std::fmod(time/4.0f,1.0);
            glClearColor(red, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            // Draw calls here
        }
	}

    void run() {
        while (_main->mainloop()) {}
    }

private:
	float time;
    VRMain *_main;
};



int main(int argc, char **argv) {
	MyVRApp app(argc, argv);
	app.run();
	return 0;
}
