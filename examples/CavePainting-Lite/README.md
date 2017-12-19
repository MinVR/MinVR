# MinVR 3DUI Example Application:  CavePainting-Lite

This example program is meant to illustrate how to create a MinVR program that includes an interesting 3D user interface.  Here, the user interface assumes that each hand holds a 6 DOF tracker with one button on it.  A 3D cursor follows each hand -- the right hand cursor is a paintbrush, and the left hand is a cube.  When the right hand button is pressed and held then the brush begins to paint a trail of multi-colored spherical "3D paint blobs".  When the left hand button is pressed and held, this "grabs onto" the entire 3D painting so that the user can then translate and rotate the scene.  The interface is a super-simplified version of the CavePainting tool (Keefe et al., 2001).

In desktop mode, MinVR's builtin "Fake Trackers" are used to translate mouse and keyboard input into the same style of 6 degree-of-freedom VR tracker move events that real VR trackers would generate.  The advantage of this approach is that you can write your apps assuming that VR input will be available and then fake this input when debugging on a desktop.

Here are the CavePainting-Lite controls for both Desktop and VR modes:

| Function                           | Desktop Mode                       | VR Mode                            |
|------------------------------------|------------------------------------|------------------------------------|
| Start Painting                     | Mouse Left Button                  | Right Hand Button                  |
| Brush Move                         |                                    | Right Hand Tracker                 |
|   - Translate Parallel to Screen   | Move Mouse                         |                                    |
|   - Translate in/out of Screen     | Hold 'T' and Move Mouse Vertically |                                    |
|   - Rotate with Trackball          | Hold 'R' and Move Mouse Around     |                                    |
|   - Toggle Brush Tracker On/Off^1  | Keyboard '2'                       |                                    |
|------------------------------------|------------------------------------|------------------------------------|
| Grab Painting w/ Left Hand         | Keyboard 'SPACEBAR'                | Left Hand Button                   |
| Left Hand (Box Icon) Move          |                                    | Left Hand Tracker                  |
|   - Translate Parallel to Screen   | Move Mouse                         |                                    |
|   - Translate in/out of Screen     | Hold 'T' and Move Mouse Vertically |                                    |
|   - Rotate with Trackball          | Hold 'R' and Move Mouse Around     |                                    |
|   - Toggle Hand Tracker On/Off^1   | Keyboard '3'                       |                                    |
|------------------------------------|------------------------------------|------------------------------------|
| Head Tracking                      |                                    | Head Tracker                       |
|   - Look Around                    | Mouse Right Click and Drag         |                                    |
|   - Walk Forward/Backward          | Keyboard 'UP'/'DOWN' or 'W'/'Z'    |                                    |
|   - Turn Left/Right                | Keyboard 'LEFT'/'RIGHT' or 'A'/'S' |                                    |
|   - Toggle Head Tracker On/Off^2   | Keyboard '1'                       |                                    |

^1 The reason to toggle the paintbrush and hand trackers on/off is so that you can control one at a time.  Since we need two trackers but we only have one mouse, both "fake trackers" follow the mouse.  In VR, this would be like having both hands right on top of each other.  If you want to test a technique where there is some distance between the hands in desktop mode, then you can toggle on/off the trackers to move them separately.

^2 There is really no reason to need to turn the head tracker on/off in this app.  The toggling feature is built in to the way that MinVR implements fake trackers.
