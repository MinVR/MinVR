# MinVR

MinVR is an Open Source Project developed and maintained collaboratively by the University of Minnesota, Macalester College, and Brown University.

The goal of MinVR is to facilitate a variety of data visualization and virtual reality research projects by providing a robust, cross-platform VR toolkit for use with many different VR displays (e.g., CAVE's, PowerWalls, multi-touch stereoscopic tables, 3DTV's, head-mounted displays) and input devices (e.g., 6 degree-of-freedom trackers, multi-touch input devices, haptic devices, home-built devices).

## Getting Started

#### Download Repository

  ```
  git clone http://github.com/MinVR/MinVR
  cd MinVR
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
  # Build debug (makefile build)
  cmake .. -DCMAKE_BUILD_TYPE=DEBUG
  
  # Build without any plugins (base plugins will be not built)
  cmake .. -DBASE_PLUGINS=OFF
  
  # Build with VRPN plugin
  cmake .. -DMINVR_VRPN_PLUGIN=ON

  # Build with other available plugin
  cmake .. -DMINVR_<Plugin Name>_PLUGIN=ON
  ```

#### Run Example

MinVR applications can be run with different configurations by passing in environment setup xml files.

* Simple OpenGL Example

```
# From base directory
build/bin/SimpleOpenGL examples/SimpleOpenGL/desktop.xml
```

* Python Example (MinVR applicaitons can be written in Python)

```
cd examples/PythonExample

# Simple example
python example.py desktop.xml

# Rotating cube example
python cube.py desktop.xml
```

## Overview

MinVR is an Open Source Project developed and maintained collaboratively by the University of Minnesota, Macalester College, and Brown University.

The goal of MinVR is to facilitate a variety of data visualization and virtual reality research projects by providing a robust, cross-platform VR toolkit for use with many different VR displays (e.g., CAVE's, PowerWalls, multi-touch stereoscopic tables, 3DTV's, head-mounted displays) and input devices (e.g., 6 degree-of-freedom trackers, multi-touch input devices, haptic devices, home-built devices).

Informed by our collective experience with a variety of VR research platforms in the past decade, the approach is to create an as-minimal-as-possible (hence the name MinVR) library that at its core contains the key code necessary to support head-tracked stereoscopic VR on a variety of displays and with a variety of input devices.  But, to have this core written in a generic way that does not impose a dependency on application programmers to use specific display or input hardware or even a specific 3D graphics library.  Of course, every VR program requires some graphics, so the library also supports this, but this support along with support for specific input devices is implemented outside of the core using a plugin architecture.  In summary, the core has essentially no external dependencies and is designed to be common to all VR programs and support for specific graphics toolkits, input devices, and other non-essential features are all implemented in plugins.

The rationale for this design comes from our experiences with VR programming over the years.  Some key points follow:

Remove the dependency on a specific graphics toolkit:  One of our key insights is that different projects often require different graphics toolkits.  At the UMN, we have used raw OpenGL and glut, G3D (versions 8, 9, and 10), Open Scene Graph, VTK, OpenHaptics, and Unity3D for recent projects depending upon the type of data we wish to visualize.  To support this, we have had to many times rewrite core VR support routines (e.g., calculating projection matrices for head-tracked displays, reading data from VR input devices, synchronizing multiple rendering nodes on a network).  Although they are often programmed this way (we have always done it this way in the past), these routines do not actually need to depend upon a particular graphics library. We think MinVR may be the first VR toolkit to specifically try to remove this dependency and support multiple graphics toolkits.  

Support both HMDs and Caves:  Existing VR toolkits seem to either target the games industry via support for low-cost head-mounted displays or the high-end scientific visualization community via support for synchronized rendering on a computer cluster.  We use both styles of VR displays in our research, so would like a VR toolkit that supports both.  MinVR is designed to support both low-cost head-mounted displays and high-end VR environments, such as Caves and Brown University's amazing new YURT.

Support multi-threaded rendering:  This is becoming important to support as new machines with multiple GPUs become more common, but it is quite difficult to introduce the idea of multi-threaded rendering into a VR toolkit unless it was designed with this idea from the beginning.  We have worked extensively with the VRG3D library, for example, but could not identify a good way to modify that library to support multi-threaded rendering on our newest high-end computer with 4 graphics cards.

Support VR research:  This is an overarching principle.  We do strange things in our research :), such as combining multi-touch with VR displays, wiring up custom input devices, streaming data to VR from supercomputers, etc.  This tends to mean that we need full access to the source code for our VR toolkit, and that we need flexibility.  The VREvent class in MinVR is a good example.  Today, the most common VR input events come from 3D trackers and buttons, but in the future, VR may rely much more heavily on depth cameras, microphones, and yet-to-be-invented input devices.  That's why the data payload for each VREvent is defined so generically.  This, again, is in contrast to many existing VR toolkits, which make key assumptions about what input is available in a VR app, e.g., every VR app has a head position, a left hand position, and a right hand position.  Developing a convention like this to support hardware and software developers as they try to promote a new VR games industry is probably a good idea for that industry, but from the standpoint of VR research that strives to discover new modes of human-computer interaction, this type of convention can be quite limiting.

Easy to get started and to modify existing graphics programs to support VR:  We often work with students who need to be able to get started programming a VR application in a week, not a whole semester.  We also have many existing 3D graphics programs that we would like to run in VR.  This motivates having a minimal and simple API for application developers.  The VRMain class contains just 4 big functions that need to be added to an existing graphics program in order make it VR capable.  And, to create a new application, programmers just fill in two callback routines, one for handling user input, and one for drawing 3D graphics.











