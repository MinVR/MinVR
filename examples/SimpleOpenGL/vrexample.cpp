
// OpenGL platform-specific headers
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

// Just included for some simple Matrix math used below
// This is not required for use of MinVR in general
#include <math/VRMath.h>


/** MyVRApp is a subclass of VRApp and overrides two key methods: 1. onVREvent(..)
    and 2. onVRRenderGraphics(..).  This is all that is needed to create a
    simple graphics-based VR application and run it on any display configured
    for use with MinVR.
 */
class MyVRApp : public VRApp {
public:
    MyVRApp(int argc, char** argv, const std::string& configFile) : VRApp(argc, argv, configFile) {
        _horizAngle = 0.0;
        _vertAngle = 0.0;
        _radius =  15.0;
        _incAngle = -0.1f;
    }

    virtual ~MyVRApp() {}

	// Callback for event handling, inherited from VRApp
	virtual void onVREvent(const VREvent &event) {

        event.print();
        
		if (event.getName() == "KbdEsc_Down") {
            shutdown();
            return;
		}
        else if (event.getName() == "MouseBtnLeft_Down") {
          _radius += 5.0 * _incAngle;
        }
        else if (event.getName() == "MouseBtnRight_Down") {
          _radius -= 5.0 * _incAngle;
        }
        else if ((event.getName() == "KbdLeft_Down") || (event.getName() == "KbdLeft_Repeat")) {
          _horizAngle -= _incAngle;
        }
        else if ((event.getName() == "KbdRight_Down") || (event.getName() == "KbdRight_Repeat")) {
          _horizAngle += _incAngle;
        }
        else if ((event.getName() == "KbdUp_Down") || (event.getName() == "KbdUp_Repeat")) {
          _vertAngle -= _incAngle;
        }
        else if ((event.getName() == "KbdDown_Down") || (event.getName() == "KbdDown_Repeat")) {
          _vertAngle += _incAngle;
        }
      
        if (_horizAngle > 6.283185) _horizAngle -= 6.283185;
        if (_horizAngle < 0.0) _horizAngle += 6.283185;
      
        if (_vertAngle > 6.283185) _vertAngle -= 6.283185;
        if (_vertAngle < 0.0) _vertAngle += 6.283185;
	}

    
	// Callback for rendering, inherited from VRRenderHandler
    virtual void onVRRenderGraphics(const VRGraphicsState &state) {
      
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LEQUAL);
      glClearDepth(1.0f);
      glClearColor(0.0, 0.0, 0.0, 1.f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      
      glMatrixMode(GL_PROJECTION);
      glLoadMatrixf(state.getProjectionMatrix());
      
      glMatrixMode(GL_MODELVIEW);
      glLoadMatrixf(state.getViewMatrix());
      
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
      glMultMatrixd(M.m);
      
      
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


protected:
    double _horizAngle, _vertAngle, _radius, _incAngle;
};



int main(int argc, char **argv) {
    MyVRApp app(argc, argv, argv[1]);
  	app.run();
	exit(0);
}

