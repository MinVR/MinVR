#include <string>
#include <iostream>

#include <display/VRConsoleNode.h>
#include <main/VRMain.h>
#include <main/VREventHandler.h>
#include <main/VRRenderHandler.h>
#include <math/VRMath.h>


#if defined(WIN32)
#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#elif defined(__APPLE__)
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
#endif

using namespace MinVR;


class MyVRApp : public VREventHandler, public VRRenderHandler {
public:
	MyVRApp(const std::string &configFile, const std::string vrSetups) : _vrMain(NULL), _quit(false) {
		_vrMain = new VRMain();
        _vrMain->initialize(configFile, vrSetups);
      	_vrMain->addEventHandler(this);
		_vrMain->addRenderHandler(this);
	}

	virtual ~MyVRApp() {
		_vrMain->shutdown();
		delete _vrMain;
	}

	// Callback for event handling, inherited from VREventHandler
	virtual void onVREvent(const std::string &eventName, VRDataIndex *eventData) {
		std::cout << "Event: " << eventName << std::endl;
		if (eventName == "ESC_down") {
			_quit = true;
		}
	}

  
    virtual void onVRRenderContext(VRDataIndex *renderState, VRDisplayNode *callingNode) {
        if (!renderState->exists("IsConsole", "/")) {
          glClearColor(0.5, 0.5, 0.5, 1.f);
        }
    }

  
	// Callback for rendering, inherited from VRRenderHandler
	virtual void onVRRenderScene(VRDataIndex *renderState, VRDisplayNode *callingNode) {
		if (renderState->exists("IsConsole", "/")) {
			VRConsoleNode *console = dynamic_cast<VRConsoleNode*>(callingNode);
			console->println("Console output...");
		}
		else {
            if (renderState->exists("ProjectionMatrix", "/")) {
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
			    VRMatrix4 P = renderState->getValue("ProjectionMatrix", "/");
			    glLoadMatrixd(P.m);
            }
            else {
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                gluPerspective(1.6*45.f, 1.f, 0.1f, 100.0f);
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                gluLookAt(5.f, 5.f, 5.f,  0.f, 0.f, 0.f,  0.f, 1.f, 0.f);
            }

            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);
          
			glBegin(GL_QUADS);              // Begin drawing the color cube with 6 quads
			// Top face (y = 1.0f)
			glColor3f(0.0f, 0.5f, 0.0f);    // Green
			glVertex3f( 1.0f, 1.0f, -1.0f);
			glVertex3f(-1.0f, 1.0f, -1.0f);
			glVertex3f(-1.0f, 1.0f,  1.0f);
			glVertex3f( 1.0f, 1.0f,  1.0f);

			// Bottom face (y = -1.0f)
			glColor3f(0.5f, 0.25f, 0.0f);    // Orange
			glVertex3f( 1.0f, -1.0f,  1.0f);
			glVertex3f(-1.0f, -1.0f,  1.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			glVertex3f( 1.0f, -1.0f, -1.0f);

			// Front face  (z = 1.0f)
			glColor3f(0.5f, 0.0f, 0.0f);     // Red
			glVertex3f( 1.0f,  1.0f, 1.0f);
			glVertex3f(-1.0f,  1.0f, 1.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);
			glVertex3f( 1.0f, -1.0f, 1.0f);

			// Back face (z = -1.0f)
			glColor3f(0.5f, 0.5f, 0.0f);     // Yellow
			glVertex3f( 1.0f, -1.0f, -1.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			glVertex3f(-1.0f,  1.0f, -1.0f);
			glVertex3f( 1.0f,  1.0f, -1.0f);

			// Left face (x = -1.0f)
			glColor3f(0.0f, 0.0f, 0.5f);     // Blue
			glVertex3f(-1.0f,  1.0f,  1.0f);
			glVertex3f(-1.0f,  1.0f, -1.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			glVertex3f(-1.0f, -1.0f,  1.0f);

			// Right face (x = 1.0f)
			glColor3f(0.5f, 0.0f, 0.5f);     // Magenta
			glVertex3f(1.0f,  1.0f, -1.0f);
			glVertex3f(1.0f,  1.0f,  1.0f);
			glVertex3f(1.0f, -1.0f,  1.0f);
			glVertex3f(1.0f, -1.0f, -1.0f);
			glEnd();                         // End of drawing color-cube
          
		}
	}

	void run() {
		while (!_quit) {
			_vrMain->mainloop();
		}
	}

protected:
	VRMain *_vrMain;
	bool _quit;
};



int main(int argc, char **argv) {

	if ((argc < 2) || (argc > 3) || ((argc==2) && (std::string(argv[1]) == "-h"))) {
		std::cout << "Usage:" << std::endl;
		std::cout << "vrexample <config-file-name.xml> [vrsetup-name]" << std::endl;
		std::cout << "     <config-file-name.xml> is required and is the name of a MinVR config file." << std::endl;
		std::cout << "     [vrsetup-name] is optional and is a comma-separated list of VRSetups" << std::endl;
		std::cout << "     to load in this process.  If more than one VRSetup is listed, new" << std::endl;
		std::cout << "     processes will be forked." << std::endl;
        exit(0);
	}

    std::string config = argv[1];
    std::string setup = "";
    if (argc >= 3) {
      setup = argv[2];
    }
  
    MyVRApp app(config, setup);
	app.run();

	exit(0);
}

