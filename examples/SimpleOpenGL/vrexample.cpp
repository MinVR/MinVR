#include <string>
#include <iostream>
#include <math.h>

#include <display/VRConsoleNode.h>
#include <main/VRMain.h>
#include <main/VREventHandler.h>
#include <main/VRRenderHandler.h>
#include <math/VRMath.h>


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


class MyVRApp : public VREventHandler, public VRRenderHandler {
public:
  MyVRApp(int argc, char** argv) : _vrMain(NULL), _quit(false) {
		_vrMain = new VRMain();
        _vrMain->initialize(argc, argv);
      	_vrMain->addEventHandler(this);
		_vrMain->addRenderHandler(this);
        _horizAngle = 0.0;
        _vertAngle = 0.0;
        _radius = 15.0;
        _incAngle = -0.1f;
	}

	virtual ~MyVRApp() {
		_vrMain->shutdown();
		delete _vrMain;
	}

	// Callback for event handling, inherited from VREventHandler
	virtual void onVREvent(const std::string &eventName, VRDataIndex *eventData) {
		std::cout << "Event: " << eventName << std::endl;
		if (eventName == "/KbdEsc_Down") {
			_quit = true;
		}
        else if (eventName == "/MouseBtnLeft_Down") {
          _radius += 5.0 * _incAngle;
        }
        else if (eventName == "/MouseBtnRight_Down") {
          _radius -= 5.0 * _incAngle;
        }
        else if ((eventName == "/KbdLeft_Down") || (eventName == "/KbdLeft_Repeat")) {
          _horizAngle -= _incAngle;
        }
        else if ((eventName == "/KbdRight_Down") || (eventName == "/KbdRight_Repeat")) {
          _horizAngle += _incAngle;
        }
        else if ((eventName == "/KbdUp_Down") || (eventName == "/KbdUp_Repeat")) {
          _vertAngle -= _incAngle;
        }
        else if ((eventName == "/KbdDown_Down") || (eventName == "/KbdDown_Repeat")) {
          _vertAngle += _incAngle;
        }
      
        if (_horizAngle > 6.283185) _horizAngle -= 6.283185;
        if (_horizAngle < 0.0) _horizAngle += 6.283185;
      
        if (_vertAngle > 6.283185) _vertAngle -= 6.283185;
        if (_vertAngle < 0.0) _vertAngle += 6.283185;
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
                // This is the typical case where the MinVR DisplayGraph contains
                // an OffAxisProjectionNode or similar node, which sets the
                // ProjectionMatrix and ViewMatrix based on head tracking data.
            
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
			    VRMatrix4 P = renderState->getValue("ProjectionMatrix", "/");
			    glLoadMatrixd(P.m);
              
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
              
			    VRMatrix4 V = renderState->getValue("ViewMatrix", "/");
			    glLoadMatrixd((M*V).m);
            }
            else {
                // If the DisplayGraph does not contain a node that sets the
                // ProjectionMatrix and ViewMatrix, then we must be in a non-headtracked
                // simple desktop mode.  We can just set the projection and modelview
                // matrices the same way as in adaptee.cpp.
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                gluPerspective(1.6*45.f, 1.f, 0.1f, 100.0f);
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
              
                double cameraPos[3];
                cameraPos[0] = _radius * cos(_horizAngle) * cos(_vertAngle);
                cameraPos[1] = -_radius * sin(_vertAngle);
                cameraPos[2] = _radius * sin(_horizAngle) * cos(_vertAngle);
    
                double cameraAim[3];
                cameraAim[0] = cos(_horizAngle) * sin(_vertAngle);
                cameraAim[1] = cos(_vertAngle);
                cameraAim[2] = sin(_horizAngle) * sin(_vertAngle);
              
                double targetPos[3] = {0.0f, 0.0f, 0.0f};
                gluLookAt (cameraPos[0], cameraPos[1], cameraPos[2],
                           targetPos[0], targetPos[1], targetPos[2],
                           cameraAim[0], cameraAim[1], cameraAim[2]);
            }

            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);
          
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

