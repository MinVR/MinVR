# MinVR

## Getting Started

#### Download Repository

  ```
  git clone http://github.com/ivlab/MinVR2
  cd MinVR2
  ```
  
#### Configure and build

* Linux and Mac (command line)

    ```
    mkdir build
    cd build
    cmake ..
    make
    ```
    
* Mac (Xcode)

    ```
    mkdir build
    cd build
    cmake .. -G Xcode
    # Open project in Xcode and build
    ```
    
* Windows (Visual Studio)

    ```
    mkdir build
    cd build
    cmake .. -G "Visual Studio 12 Win64"
    # Open project in Visual Studio and build
    ```

#### Other Configure Options

  ```
  # Build debug
  cmake .. -DCMAKE_BUILD_TYPE=DEBUG
  
  # Build with base graphics plugins (base graphics plugins will be built)
  cmake .. -DBASE_GRAPHICS_PLUGINS
  
  # Build with base plugins (base graphic and device plugins will be built)
  cmake .. -DBASE_PLUGINS
  ```

## Overview of Directories and Current Code Status

### main

Overview:  VRMain (name still under debate, has also been called VRCore and VREngine) is the key API for application programmers.  It provides the "big 4" functions that need to be added to any project in order to use MinVR.  There is an example program in this directory as well.

Status:  May not even compile yet, but serves as a good working spec for this really important API.

Lead:  Dan K. submits this as a proposal to the group for feedback...  Would be great for the Brown team and others at UMN (Dan O., maybe Seth) to take a look and provide design feedback. 


### event

Overview:  VREvent is one of the most important classes in the whole library.  There is an interesting/important design proposal here to try to make the event data payload very flexible.  This is currently implemented using the VRByteData and VRByteStream classes.

Status:  This has compiled and worked with just the caveat that it does not yet check for big/littleendianness of the current machine.  There is a simple test program inside the tests/eventdata subdir that may or may not still compile.  Would be good to resurrect that and use it as a test.  Ultimately, would like to improve this by replacing ByteData with DataStruct.

Lead:  Brown team has ideas about how to best implement something like a flexible DataStruct class that could be a nice replacement for the ByteData and ByteStream classes used currently.  This seems like the next step.  One of the key issues with the current ByteData approach is that it is not self-revealing, i.e., it's just bytes with no header info, so programmers need to know what types based only upon the name of the event in order to correctly access the data.  


### display

Overview:  Like, InputDevices, the idea here is that VR programs will have a list of current InputDevices and a list of current DisplayDevices.  For DisplayDevices, this list is contained within the DisplayManager class.  DisplayManager's job is to each frame prep each DisplayDevice for drawing and then call the application's draw() routine.  Many apps will have multiple DisplayDevices, e.g., multiple walls in a Cave.  The default is to render these sequentially in a for loop.  Alternatively, a subclass called VRDisplayManagerThreaded knows how to render each DisplayDevice in its own thread.

DisplayDevices do not necessarily need to be OpenGL-based graphics displays.  You could have an audio display or a haptic display or an offscreen display that writes images to disk or sends them over a network connection.  So, DisplayDevice is very generic.  However, the most typical display will use 3D graphics, and these graphics are always rendered to a VRGraphicsWindow.  VRGraphicsWindow is an abstract class that will be implemented in plugins for particular windowing systems, like glut.  

The rest of the classes here are much rougher in terms of the ideas.  Current working idea is something like:

```
GraphicsWindow is a DisplayDevice
  has Viewport1
    has VRProjectionMethod (or maybe VRFrustum or VRCameraRig, best name?)
    has VRStereoFormatter
  has Viewport2
    has VRProjectionMethod
    has VRStereoFormatter
```

Implemenations of VRProjectionMethod could be:  VRHeadTrackedPanel, VRHeadTrackedCurvedPanel (if needed for the YURT), VRHMD, VRDesktopCamera

Implementations of VRStereoFormatter could be:  VRMono, VRQuadBufferedStereo, VRSideBySideStereo, VRColumnInterlacedStereo


Status:  Very much a work in progress.  This is a tricky design to do in a general way while also trying to avoid making it overly complicated.

Lead:  Dan K. is still enjoying thinking about this and trying to refine it, but welcoming comments and discussion if others can understand what is there now enough to comment.  If time, will try to draw up a UML diagram to facilitate discussion.


### net

Overview:  VRNetInterface handles synchronization for rendering and events when running on a cluster.  There are two implementations of the interface, one for servers and one for clients.  

Status:  The network code needs to be extended to support Windows.  This has been tested and works on OSX with the current events that use ByteData.  

Lead:  Extending the network code to also work on windows would be a good task for a non-graphics programmer.


### math

Overview:  We need just a little bit of support for linear algebra math (3D vectors and 4x4 matrices) in order to calculate projection matrices and do other things in the display code.  I think that we should take the approach of assuming that app programmers will pull in their own math library when they pull in their own graphics toolkit, so I would view this as support for math inside MinVR only.  We don't expect others to use our Vector and Matrix classes.

Status:  Made a very simplistic Vec3 class in order to test the VREvent class.  No other work on this.

Lead:  ??  Just implement as necessary as we get into creating projection matrices etc.


### plugin

Overview:  This is a framework that Dan O. built for MinVR1 to support plugins.  

Status:  Dan O. has been using this, so seems robust.  Explaining how this is setup would be a good topic for an upcoming meeting.

Lead:  Dan O.


### config

Overview:  Support for XML-based config files.

Status:  ??

Lead:  Brown team.







## Philosophical Ramblings

MinVR is an Open Source Project developed and maintained collaboratively by the University of Minnesota and Brown University.

The goal of MinVR is to facilitate a variety of data visualization and virtual reality research projects by providing a robust, cross-platform VR toolkit for use with many different VR displays (e.g., CAVE's, PowerWalls, multi-touch stereoscopic tables, 3DTV's, head-mounted displays) and input devices (e.g., 6 degree-of-freedom trackers, multi-touch input devices, haptic devices, home-built devices).

Informed by our collective experience with a variety of VR research platforms in the past decade, the approach is to create an as-minimal-as-possible (hence the name MinVR) library that at its core contains the key code necessary to support head-tracked stereoscopic VR on a variety of displays and with a variety of input devices.  But, to have this core written in a generic way that does not impose a dependency on application programmers to use specific display or input hardware or even a specific 3D graphics library.  Of course, every VR program requires some graphics, so the library also supports this, but this support along with support for specific input devices is implemented outside of the core using a plugin architecture.  In summary, the core has essentially no external dependencies and is designed to be common to all VR programs and support for specific graphics toolkits, input devices, and other non-essential features are all implemented in plugins.

The rationale for this design comes from our experiences with VR programming over the years.  Some key points follow:

Remove the dependency on a specific graphics toolkit:  One of our key insights is that different projects often require different graphics toolkits.  At the UMN, we have used raw OpenGL and glut, G3D (versions 8, 9, and 10), Open Scene Graph, VTK, OpenHaptics, and Unity3D for recent projects depending upon the type of data we wish to visualize.  To support this, we have had to many times rewrite core VR support routines (e.g., calculating projection matrices for head-tracked displays, reading data from VR input devices, synchronizing multiple rendering nodes on a network).  Although they are often programmed this way (we have always done it this way in the past), these routines do not actually need to depend upon a particular graphics library. We think MinVR may be the first VR toolkit to specifically try to remove this dependency and support multiple graphics toolkits.  

Support both HMDs and Caves:  Existing VR toolkits seem to either target the games industry via support for low-cost head-mounted displays or the high-end scientific visualization community via support for synchronized rendering on a computer cluster.  We use both styles of VR displays in our research, so would like a VR toolkit that supports both.  MinVR is designed to support both low-cost head-mounted displays and high-end VR environments, such as Caves and Brown University's amazing new YURT.

Support multi-threaded rendering:  This is becoming important to support as new machines with multiple GPUs become more common, but it is quite difficult to introduce the idea of multi-threaded rendering into a VR toolkit unless it was designed with this idea from the beginning.  We have worked extensively with the VRG3D library, for example, but could not identify a good way to modify that library to support multi-threaded rendering on our newest high-end computer with 4 graphics cards.

Support VR research:  This is an overarching principle.  We do strange things in our research :), such as combining multi-touch with VR displays, wiring up custom input devices, streaming data to VR from supercomputers, etc.  This tends to mean that we need full access to the source code for our VR toolkit, and that we need flexibility.  The VREvent class in MinVR is a good example.  Today, the most common VR input events come from 3D trackers and buttons, but in the future, VR may rely much more heavily on depth cameras, microphones, and yet-to-be-invented input devices.  That's why the data payload for each VREvent is defined so generically.  This, again, is in contrast to many existing VR toolkits, which make key assumptions about what input is available in a VR app, e.g., every VR app has a head position, a left hand position, and a right hand position.  Developing a convention like this to support hardware and software developers as they try to promote a new VR games industry is probably a good idea for that industry, but from the standpoint of VR research that strives to discover new modes of human-computer interaction, this type of convention can be quite limiting.

Easy to get started and to modify existing graphics programs to support VR:  We often work with students who need to be able to get started programming a VR application in a week, not a whole semester.  We also have many existing 3D graphics programs that we would like to run in VR.  This motivates having a minimal and simple API for application developers.  The VRMain class contains just 4 big functions that need to be added to an existing graphics program in order make it VR capable.  And, to create a new application, programmers just fill in two callback routines, one for handling user input, and one for drawing 3D graphics.



### Build and Install

Build the project:
  ```
  make
  # build release
  make opt
  # build debug
  make debug
  ```

Install the project:
  ```
  make install
  ```

