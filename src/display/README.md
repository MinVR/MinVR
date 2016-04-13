# Display Node Architecture

## VRDisplayNode

VRDisplayNode is the fundamental class that can be inherited to create a different types of displays.

## Folder Structure

The display directory is separated into several folders to handle different levels complexity and functionality:

* **factories** - Contains base factories interfaces for creating VRDisplayNodes.  This folder mostly allows users to extend MinVR through plugins.  Users can create their own factories which parse a VRDataIndex.  These then can be added to the VRMain list of available display factories.
* **nodes** - Contains the base display nodes that are defined in MinVR, specifically for graphics and command line rendering.
* **renderers** - Contains the base renderers defined in MinVR, specifically for handling render callbacks and controlling state.

***The base display directory contains the VRDisplayNode class which is all you need to start creating your own displays.***

## Quick Start Guide

#### Creating a New Display

To create a new display, simply inherit from MinVR::VRDisplayNode and override the render function:

  ```c++
  #include "display/VRDisplayNode.h"
  #include <iostream>
  
  /* MyDisplayNode sets myValue and uses the command line to display. */
  class MyDisplayNode : public MinVR::VRDisplayNode {
  
    void render(MinVR::VRRenderHandler& renderer) {
      std::cout << "This is called when this display node renders." << std::endl;
    
      // Pushes the current render state for node state changes
      renderer->pushState();
      
      // Add a value to state to be used by the user defined render function
      renderer.setState().setValue("myValue", 7);
      
      // Calls the user defined render function
      renderer.render();
      
      // Pops the current render state to revert to the previous state
      renderer->popState();
    }
  }
  ```
  
To use the display, create a MinVR::VRRenderer and use the display to call the user defined function:
    
  ```c++
  #include "MyDisplayNode.h"
  #include "display/renderers/concrete/VRCallbackRenderer.h"
  
  /* User defined function which the display node calls */
  void userRender(VRRenderState& state) {
    int myValue = state.getValue("myValue");
    std::cout << "My user defined render function.  myValue: " << myValue << std::endl;
  }
  
  int main(int argc, char **argv) {
    // Create display
    MyDisplayNode myDisplayNode;
    
    // Create VRCallbackRenderer which encapsulates a simple function pointer
    VRCallbackRenderer renderer(userRender);
    
    // Call render function
    myDisplayNode.render(renderer);
  }
  ```
  
Output for the above program should look like the following:

  ```
  ./example
  This is called when this display node renders
  My user defined render function.  myValue: 7
  ```


