#include <iostream>



// MinVR header
#include <api/MinVR.h>
#include <VRG3DApp.h>

using namespace MinVR;

// Just included for some simple Matrix math used below
// This is not required for use of MinVR in general
#include <math/VRMath.h>


/**
 * MyVRApp is an example of a modern OpenGL using VBOs, VAOs, and shaders.  MyVRApp inherits
 * from VRGraphicsApp, which allows you to override onVREvent to get input events, onRenderContext
 * to setup context sepecific objects, and onRenderScene that renders to each viewport.
 */
class MyVRApp : public VRG3DApp {
public:
    MyVRApp(int argc, char** argv) : VRG3DApp(argc, argv) {
    }


    void onAnalogChange(const VRAnalogEvent &state) {
  
    }
    
    void onButtonDown(const VRButtonEvent &state) {
        if (state.getName() == "KbdEsc_Down") {
            shutdown();
        }
        
    }
    
    void onButtonUp(const VRButtonEvent &state) {
      
    }
    
    void onCursorMove(const VRCursorEvent &state) {}
    
    void onTrackerMove(const VRTrackerEvent &state) {}

    
    
    void onRenderAudio(const VRAudioState& state) {}
    
    
    void onRenderConsole(const VRConsoleState& state) {}

    
    void onRenderGraphicsContext(const VRGraphicsState& state) {
      VRG3DApp::onRenderGraphicsContext(state);

      if (state.isInitialRenderCall());
      {
        /*
         Make sure GlCaps is initialized.
        */
        G3D::GLCaps::init();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
       
      }

      // You can make opengl calls
      glClear(GL_COLOR_BUFFER_BIT);
    }
    
    void onRenderGraphicsScene(const VRGraphicsState& state) {
      VRG3DApp::onRenderGraphicsScene(state);
      G3D::Draw::sphere(G3D::Sphere(
        G3D::Vector3(0.5f, 0.0f, -1.0f), 0.2f), myRenderDevice,
        G3D::Color3::blue(), G3D::Color4::clear());
      G3D::Draw::sphere(G3D::Sphere(
        G3D::Vector3(0.0f, 0.0f, -1.0f), 0.2f), myRenderDevice,
        G3D::Color3::red(), G3D::Color4::clear());
      G3D::Draw::sphere(G3D::Sphere(
        G3D::Vector3(-0.5f, 0.0f, -1.0f), 0.2f), myRenderDevice,
        G3D::Color3::yellow(), G3D::Color4::clear()); 

     
    }
    
    
    void onRenderHaptics(const VRHapticsState& state) {}
    
    
    

private:
	
  /** Lighting environment */
  G3D::LightingRef         lighting;
  G3D::SkyParameters       skyParameters;
  G3D::SkyRef              sky;
  G3D::IFSModelRef         model;
  ShaderRef           phongShader;
  float               reflect;
  float               shine;
  float               diffuse;
  float               specular;
  int                 diffuseColorIndex;
  int                 specularColorIndex;
  G3D::Texture::Ref   floorTexture;
  G3D::Texture::Ref   wallTexture;

  void configureShaderArgs(const LightingRef localLighting, const float* cameraPos)
  {
    const GLight& light = lighting->lightArray[0];
    Vector3 pos(cameraPos[0], cameraPos[1], cameraPos[2]);
    phongShader->args.set("wsLight", Vector3(0, 5, 0));
    phongShader->args.set("lightColor", light.color);
    phongShader->args.set("wsEyePosition", pos);
    phongShader->args.set("ambientLightColor", localLighting->ambientAverage());

    //Color3 color = Color3::white().rgb();
    Color3 color = Color3::rainbowColorMap((float)5 / 10);
    phongShader->args.set("diffuseColor", color.rgb());
    //phongShader->args.set("diffuse", diffuse);
    phongShader->args.set("diffuse", 0.6f);

    color = Color3::rainbowColorMap((float)5 / 10);
    phongShader->args.set("specularColor", color.rgb());
    phongShader->args.set("specular", 0.5f);
    //phongShader->args.set("specular", 1.0f);
    phongShader->args.set("shine", 20.0f);
    phongShader->args.set("reflect", 0.1f);

    //phongShader->args.set("environmentMap", lighting->environmentMap);
    //phongShader->args.set("environmentMapColor", lighting->environmentMapColor);
  }
  
  void makeColorList(GFontRef iconFont)
  {
    /*const char* block = "gggggg";

    float size = 18;
    int N = 10;
    colorList.append(GuiText(block, iconFont, size, Color3::black(), Color4::clear()));
    for (int i = 0; i < N; ++i) {
      colorList.append(GuiText(block, iconFont, size, Color3::rainbowColorMap((float)i / N), Color4::clear()));
    }
    colorList.append(GuiText(block, iconFont, size, Color3::white(), Color4::clear()));

    */
  }

};



/// Main method which creates and calls application
int main(int argc, char **argv) {
	//MyVRApp app(argc, argv);
  MyVRApp app(argc, argv);
	app.run();
    app.shutdown();
	return 0;
}
