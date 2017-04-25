# MinVR OpenGL Multithreaded Tests

This set of tests use the model view controller pattern to enable multithreading for MinVR.
Advantage of the MinVR_Threading plugin to add the ThreadGroupNode display node.  The
ThreadGroupNode node synchronizes displays at the render, finish rendering, and display 
finished rendering.

## Run the tests

To execute the python test programs, run the following from the command line:

  ```bash
  # Normal output without multithreading
  build/bin/itest-opengl-multithreaded -c desktop
  # Opens two multithreaded windows
  build/bin/itest-opengl-multithreaded -c desktop-multithreaded
  # Opens four multithreaded windows with one shared context for the vbo
  build/bin/itest-opengl-multithreaded -c ivlabcave-desktop
  python example2D.py -d /path/to/MinVR/install_or_build_directory -c desktop-oldopengl
  ```

## Overview

The following describes how the test works in more detail.  Below is an overview of how the
code relates to the Model View Controller pattern:

![MVC Overview](doc/mvc_diagram.png)


