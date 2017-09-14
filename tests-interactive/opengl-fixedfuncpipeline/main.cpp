#include <iostream>
#include <cmath>

#if defined(WIN32)
#define NOMINMAX
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

// MinVR header
#include <api/MinVR.h>
using namespace MinVR;



/** MyVRApp changes the clear color as frames progress. */
class MyVRApp : public VRApp {
public:
	MyVRApp(int argc, char** argv) : VRApp(argc, argv) {}

	void onVREvent(const VREvent &event) {
        
		if (isRunning()) {
			event.print();

			// Get the time since application began
			if (event.getName() == "FrameStart") {
				time = event.getDataAsFloat("ElapsedSeconds");
				return;
			}

			// Quit if the escape button is pressed
			if (event.getName() == "KbdEsc_Down") {
				shutdown();
			}
		}
	}

	void onVRRenderGraphicsContext(const VRGraphicsState &renderState) {
		// Print out when the window was opened and closed
		if (renderState.isInitialRenderCall()) {
			std::cout << "Window opened." << std::endl;

			// Create VBOs, VAOs, Textures, Shaders, and FrameBuffers
		}

		if (!isRunning()) {
			std::cout << "Window closed." << std::endl;
		}
	}

	void onVRRenderGraphics(const VRGraphicsState &renderState) {
		if (isRunning()) {
			// Get projection an view matrices
		  // These are not actually used in this example.
		  //const float* projectionMatrix = renderState.getProjectionMatrix();
		  //const float* viewMatrix = renderState.getViewMatrix();

			// Show gradient of red color over four seconds then restart
			float red = std::fmod(time/4.0,1.0);
			glClearColor(red, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			// Draw calls here
		}
	}

private:
	float time;
};

int main(int argc, char **argv) {
	MyVRApp app(argc, argv);
	app.run();
	return 0;
}
