# MinVR's Approach to Finding and Building External Dependencies

Context and Goals:
* Recall, nothing in the MinVR core library depends upon any external library -- no dependencies in the core.
* However, the plugins can depend on external libraries and several of the most important plugins do (e.g., OpenGL, GLFW).  These are needed to build even a simple desktop graphics test program.
* When ramping up a new system, we love the way modern tools like cmake and git have capabilities built in to automatically download and build dependencies.  We want to support this.  For new users, it will be great if they can just run cmake and have it automatically do everything needed in order to generate a working demo program.
* However, on shared systems, such as a Cave or tiled display installed in a lab or even on a personal machine for a  developer who has worked on several graphics projects, it is often the case that they already have some of these common dependencies (e.g., GLFW, GLEW) installed on their system.  So, we also want to find these and use them or have the user point cmake to a particular version he/she wishes to use.


Solution:
* The particular plugins to build are set via options defined in the root CMakeLists.txt file.
* Similarly, there are options to build or not build any MinVR test or example programs that require external dependenices, and the option descriptions state exactly which plugins they depend upon.
* When any of these options are set, the root CMakeLists.txt file adds an appropriate subdir to the build.
* The local CMakeLists.txt files inside these subdirs have the responsibility of calling a Use*() macro for each dependency.
* The Use*() macros (e.g., UseGLFW(), UseGLEW(), UseOpenGL()) are defined within files in the cmake directory, with one macro per file and one macro for every possible dependency that shows up in the plugins that are distributed with the MinVR core.
* Each Use*() macro tries:
  - 1. It first calls cmake's find_package() macro to try to find a pre-installed version of the dependency.  This built-in cmake macro looks for Find*.cmake files, which provide hints about where the dependencies might be located.  CMake ships with a number of these Find*.cmake files, and MinVR uses these when it can, but when we have to write our own versions of these files, we put them in the cmake/Modules directory.
  - 2a. If find_package() succeeds, then we are done, and we just proceed to the next step of the cmake build.
  - 2b. If find_package() fails and DEPENDENCIES_AUTOBUILD is OFF, then we output a status message that says the dependency is missing and, in case it is installed on the system but cmake is just having trouble finding it, our message also tells the user how to configure cmake to find it.
  - 2c. If find_package() fails and DEPENDENCIES_AUTOBUILD is ON, then we use ExternalProject_add() to download the dependency as an external project and build and install as part of the MinVR build and install processes.  In this case, the source for the project will be downloaded and built inside MinVR's external directory.  If DEPENDENCIES_INSTALL is ON, then these external projects get installed to /usr/local/share (or whatever prefix is set) along with MinVR when MinVR is installed.

  




