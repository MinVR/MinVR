### Description
The VTK plugin integrates the  [Visualization Tool Kit VTK](https://www.vtk.org/) to analyze and display scientific data in multi display systems with the flexibility of using multydisplay and multiplatform systems provided by MinVR.


### Building

:warning: This plugin was built and tested using vtk 9.0. We higly recommend you to use an equal or later version.

Download the vtk sdk library from [VTK](https://vtk.org/download/). 
 
During the cmake configuration step first enable the option to build the plugin (***WITH_PLUGIN_VTK***) and then point VTK_INCLUDE to "YOUR_PATH_TO_VTK/include/vtk" and VTK_LIBRARY to "YOUR_PATH_TO_VTK/lib/vtk". The dll's are in the folder "YOUR_PATH_TO_VTK/bin".

### Usage

The VTK is an extension of MinVR::VRApp. First, add the plugin in your configuration file:

```xml
  <VTKPlugin pluginType="MinVR_VTK"/>
```

Create a class that extends from VRApp, and in the onRenderGraphicsContext and onRenderGraphicsScene functions make sure to call the super class's method. This will ensure that vtk redirect its rendering to MinVR.
The following is an example of the necessary code:

```c++ 

#include <MinVR-2.0/MinVR_VTK/VRVTKApp.h>

class MyVRApp : public VRVTKApp 
{
   MyVRApp(int argc, char** argv) : VRVTKApp(argc, argv) {
  }
  
  void onRenderGraphicsContext(const VRGraphicsState& state) {
    // If this is the inital call, initialize context variables
    VRVTKApp::onRenderGraphicsContext(state);
    ..              ..
    . Your code Here (Your initial Render call probably goes here).
    ..               ..
  }
  
  void onRenderGraphicsScene(const VRGraphicsState& state) {
    ..             ..
    . Your code Here .
    ..               ..
    //at the end of your render code,  call the super class method
    VRVTKApp::onRenderGraphicsScene(state);
  }
}
```
To see an example on how to use it, during the cmake configuration select **WIH_VTK_TEST**. There is also a configuration file example **desktop_VTK.minvr**

