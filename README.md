# MinVR

MinVR is an Open Source Project developed and maintained collaboratively by the University of Minnesota, Macalester College, and Brown University.

The goal of MinVR is to facilitate a variety of data visualization and virtual reality research projects by providing a robust, cross-platform VR toolkit for use with many different VR displays (e.g., CAVE's, PowerWalls, multi-touch stereoscopic tables, 3DTV's, head-mounted displays) and input devices (e.g., 6 degree-of-freedom trackers, multi-touch input devices, haptic devices, home-built devices).



## Quick Start Guide

### Prereqs

MinVR is built using the cross-platform CMake tool.  Before building MinVR, you will need to install CMake version 3.1 or higher (https://cmake.org/).  


### Background for CMake Beginners

If you are new to CMake, this is the minimum background you should have before building MinVR.

CMake is a cross-platform tool for generating native Unix Makefiles and project workspaces (e.g., Xcode projects, Visual Studio solutions) for building projects.  Projects use CMakeLists.txt files to describe how they should be built.  These files provide input for the cmake application, which uses them to generate project files for a variety of development environments.  CMake can generate Makefiles for building MinVR on linux systems or project files for building MinVR with popular IDEs (e.g., XCode, Visual Studio, Eclipse, CodeBLocks).  

There are three versions of the cmake application that are provided with the install from Kitware.  You can use any of these with MinVR: 
1. *cmake* is the command line version where all build options must be specified on the command line.
2. *ccmake* is a visual interface that runs in a terminal.  This is nice because all of the available build options are listed for you to see at once, and you can simply press the SPACEBAR to turn them on/off or type in specific values.  The one downside for ccmake is that the status messages that are output when CMake is running flash by so fast that they are hard to read.  So, if anything goes wrong, it can be more difficult to understand the error messages.
3. *cmake-gui* is an OS-specific windowed version.  Like ccmake, the GUI provides a useful list all of the available build options.  Unlike ccmake, it also includes a nice scrollable log of all of the status messages.  So, this is the preferred version for new users and for the first time you build a project.

With CMake, it's best to do an out-of-source build.  That means that when you start CMake you must tell it two paths: 1. the path to the MinVR source tree, AND 2. the path to the place where you would like all the built files to be created.  When running cmake-gui, these can be set interactively with a file selection dialog, but all three versions respond to command line options, and this is generally the easiest way to start up cmake.  So, we suggest starting *cmake*, *ccmake*, or *cmake-gui* from the command line.  First cd to the directory you wish to use for the built files, then pass a single required command line argument for the path to the MinVR source tree.  Usually, you will see this written as a series of 3 steps -- the common idiom for cmake builds:
```
# from within the root of the project's source directory
mkdir build
cd build
cmake ..
```

In addition to the path to the root source directory, you can pass a variety of other options to CMake on the command line.  If you are using *cmake* this is the only way to set important flags such as -D WITH_PLUGIN_GLFW=ON, to tell MinVR to build optional plugins.  So, with *cmake*, you will typically end up with a long string of arguments in order to setup the build you want.  With *ccmake* or *cmake-gui*, you can set all of these build options within the user interface.


### Download, Configure, Build, and Run a Test Graphics Program

Here's a complete annotated list of steps to display your first MinVR demo program.

1. Download the MinVR Source Tree
```
git clone http://github.com/MinVR/MinVR
```

2. Create an Initial CMake Build Configuration

```
cd MinVR
mkdir build
cd build
cmake-gui ..
```

Press the 'Configure' button. Then, select the Generator you would like to use from the list provided (e.g., Xcode, Visual Studio, Unix Makefiles).  Click Done.

Wait for XCode to do an initial configuration.  This will populate an initial list of CMake and MinVR build options.

Scroll through the options to confirm that ```DEPENDENCIES_AUTOBUILD``` and ```DEPENDENCIES_INSTALL``` are preset to ```ON```.  The main MinVR library does not depend on other external libraries, but several MinVR plugins do have dependencies.  The first time you build MinVR, it is useful to have these ```DEPENDENCIES_*``` flags set to ```ON``` because this will cause MinVR to automatically download and build any dependencies that are needed by the plugins if they are not already found installed on your system.


3. Select Plugins to Build and Re-Configure

Plugins and all other optional portions of the MinVR build (e.g., examples, documentation) can be turned on using options of the form ```WITH_*```.  Scroll down to those options now.  Let's build an example of a first 3D graphics program that can run in a desktop mode or VR mode.

Set ```WITH_ITEST_OPENGL_SHADERPIPELINE_WITH_API``` to ```ON```.  This will build the itest-opengl-shaderpipeline-with-api example application.

We also need to build two plugins.  If you search for itest-opengl-shaderpipeline-with-api in cmake-gui's output textbox, you'll see that there is a note that says that that this execuatable requires the OpenGL Plugin and a WindowToolkit Plugin, such as the GLFW Plugin.  (The FreeGlut Plugin could be used in place of GLFW if desired.)  Let's build the two required/recommended plugins:

Set ```WITH_OPENGL_PLUGIN``` to ```ON````.

Set ```WITH_GLFW_PLUGIN``` to ```ON```.

Now, click the Configure button again.  (In CMake, you need to re-configure every time you make a change to a build option.  Sometimes a change will also unlock new options.  It doesn't hurt to try different options and reconfigure as many times as you want.)


4. Generate the Build System / Project Files

Finally, with these options set, press the Generate button.  This is the step that will actually generate the Unix Makefiles, Visual Studio Solution File, or Xcode Project File needed to build MinVR.  


5. Build MinVR with the Specified Options

Click Open Project if you generated project files for an IDE, or if you generated Unix Makefiles return to your shell and the build directory.  Now, build the project as you normally would in these enviornments.  

For Xcode or Visual Studio ```click the triangle button```.

For Unix Makefiles ```type make```.


6. Run a Test Program in Desktop Mode

```bin/itest-opengl-shaderpipeline-with-api -c ../config/desktop.minvr```


### Run in VR Mode

You can run the same example application in a variety of VR displays supported by MinVR just by changing the configuration file specified on the command line.  MinVR ships with configs that work for some common displays, such as the Oculus Rift, HTC Vive, and various 3DTVs.  If you have a Vive, try running the following.

```bin/itest-opengl-shaderpipeline-with-api -c ../config/HTC_Vive.minvr```

MinVR also works with high-end displays, such as CAVEs and Powerwalls.  Some example config files for these environments are also included, but these high-end environments are not standardized, so these may writing a custom config file using one of the examples as a starting point.


## Installing the Library

CMake will generate an install target that you can run to install MinVR on your system.  The installation follows a linux convention, include files go in <prefix>/include/MinVR, libraries go in <prefix>/lib, binaries go in <prefix>/bin, and config files and cmake files for importing MinVR into other projects go in <prefix>/share/MinVR.  

By convention, you tell CMake the value for <prefix> by setting the CMAKE_INSTALL_PREFIX to /usr/local or some other desired path.  Once you have this configured, simply run:
```make install``` 
for Unix Makefiles.  Or build the 
```install``` 
target from within your IDE.


## Creating Your Own Example Program

The projects in the MinVR/examples folder are examples of stand-alone projects that use separate build systems.  They can be copied and moved out of the MinVR source tree to serve as starting points for building your own project.   

TODO!


## Developing and Debugging VR Programs in Desktop Mode

TODO!



## Creating Your Own MinVR Configuration File

TODO!



## Overview and Philosophy of the Project

MinVR is an Open Source Project developed and maintained collaboratively by the University of Minnesota, Macalester College, and Brown University.

The goal of MinVR is to facilitate a variety of data visualization and virtual reality research projects by providing a robust, cross-platform VR toolkit for use with many different VR displays (e.g., CAVE's, PowerWalls, multi-touch stereoscopic tables, 3DTV's, head-mounted displays) and input devices (e.g., 6 degree-of-freedom trackers, multi-touch input devices, haptic devices, home-built devices).

Informed by our collective experience with a variety of VR research platforms in the past decade, the approach is to create an as-minimal-as-possible (hence the name MinVR) library that at its core contains the key code necessary to support head-tracked stereoscopic VR on a variety of displays and with a variety of input devices.  But, to have this core written in a generic way that does not impose a dependency on application programmers to use specific display or input hardware or even a specific 3D graphics library.  Of course, every VR program requires some graphics, so the library also supports this, but this support along with support for specific input devices is implemented outside of the core using a plugin architecture.  In summary, the core has essentially no external dependencies and is designed to be common to all VR programs and support for specific graphics toolkits, input devices, and other non-essential features are all implemented in plugins.

The rationale for this design comes from our experiences with VR programming over the years.  Some key points follow:

Remove the dependency on a specific graphics toolkit:  One of our key insights is that different projects often require different graphics toolkits.  At the UMN, we have used raw OpenGL and glut, G3D (versions 8, 9, and 10), Open Scene Graph, VTK, OpenHaptics, and Unity3D for recent projects depending upon the type of data we wish to visualize.  To support this, we have had to many times rewrite core VR support routines (e.g., calculating projection matrices for head-tracked displays, reading data from VR input devices, synchronizing multiple rendering nodes on a network).  Although they are often programmed this way (we have always done it this way in the past), these routines do not actually need to depend upon a particular graphics library. We think MinVR may be the first VR toolkit to specifically try to remove this dependency and support multiple graphics toolkits.  

Support both HMDs and Caves:  Existing VR toolkits seem to either target the games industry via support for low-cost head-mounted displays or the high-end scientific visualization community via support for synchronized rendering on a computer cluster.  We use both styles of VR displays in our research, so would like a VR toolkit that supports both.  MinVR is designed to support both low-cost head-mounted displays and high-end VR environments, such as Caves and Brown University's amazing new YURT.

Support multi-threaded rendering:  This is becoming important to support as new machines with multiple GPUs become more common, but it is quite difficult to introduce the idea of multi-threaded rendering into a VR toolkit unless it was designed with this idea from the beginning.  We have worked extensively with the VRG3D library, for example, but could not identify a good way to modify that library to support multi-threaded rendering on our newest high-end computer with 4 graphics cards.

Support VR research:  This is an overarching principle.  We do strange things in our research :), such as combining multi-touch with VR displays, wiring up custom input devices, streaming data to VR from supercomputers, etc.  This tends to mean that we need full access to the source code for our VR toolkit, and that we need flexibility.  The VREvent class in MinVR is a good example.  Today, the most common VR input events come from 3D trackers and buttons, but in the future, VR may rely much more heavily on depth cameras, microphones, and yet-to-be-invented input devices.  That's why the data payload for each VREvent is defined so generically.  This, again, is in contrast to many existing VR toolkits, which make key assumptions about what input is available in a VR app, e.g., every VR app has a head position, a left hand position, and a right hand position.  Developing a convention like this to support hardware and software developers as they try to promote a new VR games industry is probably a good idea for that industry, but from the standpoint of VR research that strives to discover new modes of human-computer interaction, this type of convention can be quite limiting.

Easy to get started and to modify existing graphics programs to support VR:  We often work with students who need to be able to get started programming a VR application in a week, not a whole semester.  We also have many existing 3D graphics programs that we would like to run in VR.  This motivates having a minimal and simple API for application developers.  The VRMain class contains just 4 big functions that need to be added to an existing graphics program in order make it VR capable.  And, to create a new application, programmers just fill in two callback routines, one for handling user input, and one for drawing 3D graphics.

