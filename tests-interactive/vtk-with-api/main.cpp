#include <iostream>

#ifdef _WIN32
#include "GL/glew.h"
#include "GL/wglew.h"
#elif (!defined(__APPLE__))
#include "GL/glxew.h"
#endif

// OpenGL Headers
#if defined(WIN32)
#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#elif defined(__APPLE__)
#define GL_GLEXT_PROTOTYPES
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

// MinVR header
#include <api/MinVR.h>

using namespace MinVR;

// Just included for some simple Matrix math used below
// This is not required for use of MinVR in general
#include <math/VRMath.h>

#include <MinVR-2.0/MinVR_VTK/VRVTKApp.h>

#include <vtkPolyDataMapper.h>
#include <vtkCubeSource.h>




#include <vtkAutoInit.h>


VTK_MODULE_INIT(vtkRenderingOpenGL2);
//VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);

void checkGLError()
{
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR){
        std::cout << err;
    }  
}

/**
 * MyVRApp is an example of a modern OpenGL using VBOs, VAOs, and shaders.  MyVRApp inherits
 * from VRGraphicsApp, which allows you to override onVREvent to get input events, onRenderContext
 * to setup context sepecific objects, and onRenderScene that renders to each viewport.
 */
class MyVRApp : public VRVTKApp {
public:
  MyVRApp(int argc, char** argv) : VRVTKApp(argc, argv) {
  }


  void onAnalogChange(const VRAnalogEvent& state) {
    if (state.getName() == "FrameStart") {
      double time = state.getValue();
      actor->RotateY(0.5* time);
    
      return;
    }
  }

  void onButtonDown(const VRButtonEvent& state) {
    if (state.getName() == "KbdEsc_Down") {
      shutdown();
    }
  }

  void onButtonUp(const VRButtonEvent& state) {}

  void onCursorMove(const VRCursorEvent& state) {}

  void onTrackerMove(const VRTrackerEvent& state) {}



  void onRenderAudio(const VRAudioState& state) {}


  void onRenderConsole(const VRConsoleState& state) {}


  void onRenderGraphicsContext(const VRGraphicsState& state) {
    // If this is the inital call, initialize context variables
    VRVTKApp::onRenderGraphicsContext(state);
    if (state.isInitialRenderCall()) {
     
     
      actor->SetMapper(mapper);
      ren->AddActor(actor);
      vtkNew<vtkCubeSource> cs;
      mapper->SetInputConnection(cs->GetOutputPort());
      ren->ResetCamera();

      
      
    }
  }

  void onRenderGraphicsScene(const VRGraphicsState& state) {

    // Enable depth testing. Demonstrates OpenGL context being managed by external
  // application i.e. GLUT in this case.
    glEnable(GL_DEPTH_TEST);
    checkGLError();
    // Buffers being managed by external application i.e. GLUT in this case.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
    checkGLError();
    glClearDepth(1.0f);
    checkGLError();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the color buffer
    checkGLError();

    glFlush(); // Render now
    checkGLError();
   

    
    VRVTKApp::onRenderGraphicsScene(state);
  

  }


  void onRenderHaptics(const VRHapticsState& state) {}


private:
  vtkNew<vtkPolyDataMapper> mapper;
  vtkNew<vtkActor> actor;
 
};



/// Main method which creates and calls application
int main(int argc, char** argv) {
  MyVRApp app(argc, argv);
  app.run();
  app.shutdown();
  return 0;
}