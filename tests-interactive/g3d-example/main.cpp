#include <iostream>



// MinVR header
#include <api/MinVR.h>
#include <VRG3DApp.h>

using namespace MinVR;

// Just included for some simple Matrix math used below
// This is not required for use of MinVR in general
#include <math/VRMath.h>

#include "VRG3DApp.h"

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
    
    void onButtonUp(const VRButtonEvent &state) {}
    
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
        GLCaps::init();

        std::string skydataDir = "F:\\vrg3d-code\\G3D\\data-files\\sky\\";
        
        sky = Sky::fromFile(skydataDir);


        skyParameters = G3D::SkyParameters(G3D::toSeconds(11, 00, 00, AM));
        lighting = G3D::Lighting::fromSky(sky, skyParameters, Color3::white());

        // This simple demo has no shadowing, so make all lights unshadowed
        lighting->lightArray.append(lighting->shadowedLightArray);
        lighting->shadowedLightArray.clear();


       phongShader = G3D::Shader::fromFiles("F:\\vrg3d-code\\G3D\\samples\\shader\\data-files\\phong.vrt", "F:\\vrg3d-code\\G3D\\samples\\shader\\data-files\\phong.pix");
       model = G3D::IFSModel::fromFile(System::findDataFile("F:\\vrg3d-code\\G3D\\data-files\\ifs\\teapot.ifs"));

       //// makeColorList(G3D::GFont::fromFile(G3D::System::findDataFile("icon.fnt")));

       // // Color 1 is red
       // diffuseColorIndex = 1;
       // // Last color is white
       // //specularColorIndex = colorList.size() - 1;
       // specularColorIndex = 2;

        //GLCaps::init();
        Texture::Settings settings;
        settings.interpolateMode = Texture::TRILINEAR_MIPMAP;
        settings.wrapMode = WrapMode::TILE;
        Texture::Dimension        dim = Texture::DIM_2D_NPOT;
        Texture::Preprocess preProcess;
        double                    bright = 1.0;
        preProcess.modulate = Color4(bright, bright, bright);
        /* floorTexture = Texture::fromFile(
           "F:\\vrg3d-code\\G3D\\data-files\\image\\checkerboard.jpg",
           TextureFormat::RGBA8(), dim, settings, preProcess);*/
        floorTexture = Texture::fromFile(
          "F:\\DrawingOnAir-master\\share\\images\\32grid.jpg",
          TextureFormat::RGBA8(), dim, settings, preProcess);
        wallTexture = Texture::fromFile(
          "F:\\DrawingOnAir-master\\share\\images\\wall-grid.jpg",
          TextureFormat::RGBA8(), dim, settings, preProcess);
      }
    }
    
    void onRenderGraphicsScene(const VRGraphicsState& state) {
      VRG3DApp::onRenderGraphicsScene(state);
      /*G3D::Draw::sphere(G3D::Sphere(
        G3D::Vector3(0.0f, -1.0f, -1.0f), 0.5f), myRenderDevice,
        G3D::Color3::blue(), G3D::Color4::clear());
      G3D::Draw::sphere(G3D::Sphere(
        G3D::Vector3(0.0f, 1.0f, -1.0f), 0.5f), myRenderDevice,
        G3D::Color3::red(), G3D::Color4::clear());
      G3D::Draw::sphere(G3D::Sphere(
        G3D::Vector3(5.0f, 0.0f, -1.0f), 0.5f), myRenderDevice,
        G3D::Color3::yellow(), G3D::Color4::clear());*/

      G3D::SkyParameters localSky = skyParameters;
      G3D::LightingRef   localLighting = lighting;

      myRenderDevice->setColorClearValue(G3D::Color3(0.1f, 0.5f, 1.0f));
      myRenderDevice->clear(false, true, true);
      sky->render(myRenderDevice, localSky);

      myRenderDevice->pushState();
      // Pose our model based on the manipulator axes
      Surface::Ref posedModel = model->pose(CoordinateFrame());

      // Enable the shader
      configureShaderArgs(localLighting, state.getCameraPos());
      myRenderDevice->setShader(phongShader);

      // Send model geometry to the graphics card
      myRenderDevice->setObjectToWorldMatrix(posedModel->coordinateFrame());
      posedModel->sendGeometry(myRenderDevice);
      myRenderDevice->popState();



      myRenderDevice->pushState();
      myRenderDevice->setColor(Color3(0.8, 0.8, 0.8));
      myRenderDevice->setTexture(0, floorTexture);
      myRenderDevice->setCullFace(RenderDevice::CULL_NONE);

      double y = -0.32001 - 0.001;
      double w = 1.3 * 2.0;
      double h = 0.7 * 2.0;
      myRenderDevice->beginPrimitive(PrimitiveType::QUADS);
      myRenderDevice->setNormal(Vector3(0, 1, 0));
      myRenderDevice->setTexCoord(0, Vector2(0, 1));
      myRenderDevice->sendVertex(Vector3(-w / 2.0, y, h / 2.0));
      myRenderDevice->setTexCoord(0, Vector2(0, 0));
      myRenderDevice->sendVertex(Vector3(-w / 2.0, y, -h / 2.0));
      myRenderDevice->setTexCoord(0, Vector2(1, 0));
      myRenderDevice->sendVertex(Vector3(w / 2.0, y, -h / 2.0));
      myRenderDevice->setTexCoord(0, Vector2(1, 1));
      myRenderDevice->sendVertex(Vector3(w / 2.0, y, h / 2.0));
      myRenderDevice->endPrimitive();
      myRenderDevice->popState();
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
