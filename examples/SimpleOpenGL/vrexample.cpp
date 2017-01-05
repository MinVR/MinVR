#include <string>
#include <iostream>
#include <math.h>

#include <display/VRConsoleNode.h>
#include <main/VRMain.h>
#include <main/VREventHandler.h>
#include <main/VRGraphicsHandler.h>
#include <main/VRGraphicsStateInternal.h>
#include <math/VRMath.h>
#include <main/impl/VRDefaultAppLauncher.h>


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

using namespace MinVR;


class MyVRApp : public VREventHandler, public VRGraphicsHandler {
public:
  MyVRApp(int argc, char** argv) : _vrMain(NULL), _quit(false) {
		_vrMain = new VRMain();

		//std::string minVRCommandLine = std::string(argv[3]) + " /My/ConfigVal1=1 /VRSetupToStart=FrontWall";
		//VRDefaultAppLauncher launcher(argc, argv, minVRCommandLine);
		//_vrMain->initialize(launcher);

		//std::string args[] = {"test=3", "again=44"};
        //_vrMain->initialize(argc, argv, argv[1], std::vector<std::string>(args, args + sizeof(args) / sizeof(args[0])));

		std::string configFile = argv[1];
		_vrMain->initialize(argc, argv, configFile);

		//_vrMain->initialize(argc, argv);
      	_vrMain->addEventHandler(this);
		_vrMain->addRenderHandler(this);
        _horizAngle = 0.0;
        _vertAngle = 0.0;
		_radius =  15.0;
        _incAngle = -0.1f;
	}

	virtual ~MyVRApp() {
		_vrMain->shutdown();
		delete _vrMain;
	}

	// Callback for event handling, inherited from VREventHandler
	virtual void onVREvent(const VREvent &event) {
		//std::cout << "Event: " << eventName << std::endl;
		if (event.getName() == "/KbdEsc_Down") {
			_quit = true;
		}
        else if (event.getName() == "/MouseBtnLeft_Down") {
          _radius += 5.0 * _incAngle;
        }
        else if (event.getName() == "/MouseBtnRight_Down") {
          _radius -= 5.0 * _incAngle;
        }
        else if ((event.getName() == "/KbdLeft_Down") || (event.getName() == "/KbdLeft_Repeat")) {
          _horizAngle -= _incAngle;
        }
        else if ((event.getName() == "/KbdRight_Down") || (event.getName() == "/KbdRight_Repeat")) {
          _horizAngle += _incAngle;
        }
        else if ((event.getName() == "/KbdUp_Down") || (event.getName() == "/KbdUp_Repeat")) {
          _vertAngle -= _incAngle;
        }
        else if ((event.getName() == "/KbdDown_Down") || (event.getName() == "/KbdDown_Repeat")) {
          _vertAngle += _incAngle;
        }
      
        if (_horizAngle > 6.283185) _horizAngle -= 6.283185;
        if (_horizAngle < 0.0) _horizAngle += 6.283185;
      
        if (_vertAngle > 6.283185) _vertAngle -= 6.283185;
        if (_vertAngle < 0.0) _vertAngle += 6.283185;
	}

  
    virtual void onVRRenderGraphicsContext(const VRGraphicsState &state) {
        if (!state.getInternal()->getDataIndex()->exists("IsConsole", "/")) {
        }
    }

	int count;
  
	// Callback for rendering, inherited from VRRenderHandler
  virtual void onVRRenderGraphics(const VRGraphicsState &state) {
        if (!state.getInternal()->getDataIndex()->exists("IsConsole", "/")) {
          //VRConsoleNode *console = dynamic_cast<VRConsoleNode*>(callingNode);
          //console->println("Console output...");
        }
		else {

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glClearDepth(1.0f);
			count++;
			glClearColor(0.0, 0.0, 0.0, 1.f);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			//glClear(GL_DEPTH_BUFFER_BIT);

                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
			    glLoadMatrixf(state.getProjectionMatrix());
              
			    glMatrixMode(GL_MODELVIEW);
			    glLoadIdentity();
              
                // In the original adaptee.cpp program, keyboard and mouse commands
                // were used to adjust the camera.  Now that we are creating a VR
                // program, we want the camera (Projection and View matrices) to
                // be controlled by head tracking.  So, we switch to having the
                // keyboard and mouse control the Model matrix.  Guideline: In VR,
                // put all of the "scene navigation" into the Model matrix and
                // leave the Projection and View matrices for head tracking.
                VRMatrix4 M = VRMatrix4::translation(VRVector3(0.0, 0.0, -_radius)) *
                              VRMatrix4::rotationX(_vertAngle) *
                              VRMatrix4::rotationY(_horizAngle);
                glLoadMatrixd(M.m);
          
                glMultMatrixf(state.getViewMatrix());
          
    
            glBegin(GL_LINES);
            glColor3f(1.0, 1.0, 0.0);
            glVertex3f(8.0, 0.0, 0.0);
            glVertex3f(0.0, 0.0, 0.0);
          
            glColor3f(0.0, 1.0, 1.0);
            glVertex3f(0.0, 8.0, 0.0);
            glVertex3f(0.0, 0.0, 0.0);
          
            glColor3f(1.0, 0.0, 1.0);
            glVertex3f(0.0, 0.0, 8.0);
            glVertex3f(0.0, 0.0, 0.0);
          
            glEnd();
          
          
            glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
                                              // Top face (y = 1.0f)
                                            // Define vertices in counter-clockwise (CCW) order with normal
                                            // pointing out
          glColor3f(0.0f, 0.5f, 0.0f);     // Green
          glVertex3f( 1.0f, 1.0f, -1.0f);
          glVertex3f(-1.0f, 1.0f, -1.0f);
          glVertex3f(-1.0f, 1.0f,  1.0f);
          glVertex3f( 1.0f, 1.0f,  1.0f);
          
          // Bottom face (y = -1.0f)
          glColor3f(0.5f, 0.25f, 0.0f);     // Orange
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
          glEnd();  // End of drawing color-cube
          
          
          
          glTranslatef(1.5f, 7.0f, 0.0f);  // Move right and into the screen
          
          glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
                                            // Top face (y = 1.0f)
                                            // Define vertices in counter-clockwise (CCW) order with normal
                                            // pointing out
          glColor3f(0.0f, 0.5f, 0.0f);     // Green
          glVertex3f( 1.0f, 1.0f, -1.0f);
          glVertex3f(-1.0f, 1.0f, -1.0f);
          glVertex3f(-1.0f, 1.0f,  1.0f);
          glVertex3f( 1.0f, 1.0f,  1.0f);
          
          // Bottom face (y = -1.0f)
          glColor3f(0.5f, 0.25f, 0.0f);     // Orange
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
          glEnd();  // End of drawing color-cube
          
          // Render a pyramid consists of 4 triangles
          //   glLoadIdentity();                  // Reset the model-view matrix
          glTranslatef(-1.5f, 0.0f, -6.0f);  // Move left and into the screen
          
          glBegin(GL_TRIANGLES);           // Begin drawing the pyramid with 4 triangles
                                           // Front
          glColor3f(1.0f, 0.0f, 0.0f);     // Red
          glVertex3f( 0.0f, 1.0f, 0.0f);
          glColor3f(0.0f, 1.0f, 0.0f);     // Green
          glVertex3f(-1.0f, -1.0f, 1.0f);
          glColor3f(0.0f, 0.0f, 1.0f);     // Blue
          glVertex3f(1.0f, -1.0f, 1.0f);
          
          // Right
          glColor3f(1.0f, 0.0f, 0.0f);     // Red
          glVertex3f(0.0f, 1.0f, 0.0f);
          glColor3f(0.0f, 0.0f, 1.0f);     // Blue
          glVertex3f(1.0f, -1.0f, 1.0f);
          glColor3f(0.0f, 1.0f, 0.0f);     // Green
          glVertex3f(1.0f, -1.0f, -1.0f);
          
          // Back
          glColor3f(1.0f, 0.0f, 0.0f);     // Red
          glVertex3f(0.0f, 1.0f, 0.0f);
          glColor3f(0.0f, 1.0f, 0.0f);     // Green
          glVertex3f(1.0f, -1.0f, -1.0f);
          glColor3f(0.0f, 0.0f, 1.0f);     // Blue
          glVertex3f(-1.0f, -1.0f, -1.0f);
          
          // Left
          glColor3f(1.0f,0.0f,0.0f);       // Red
          glVertex3f( 0.0f, 1.0f, 0.0f);
          glColor3f(0.0f,0.0f,1.0f);       // Blue
          glVertex3f(-1.0f,-1.0f,-1.0f);
          glColor3f(0.0f,1.0f,0.0f);       // Green
          glVertex3f(-1.0f,-1.0f, 1.0f);
          glEnd();   // Done drawing the pyramid
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
    double _horizAngle, _vertAngle, _radius, _incAngle;
};



int main(int argc, char **argv) {

    MyVRApp app(argc, argv);
  	app.run();

	exit(0);
}

