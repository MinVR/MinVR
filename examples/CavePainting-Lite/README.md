# MinVR 3DUI Example Application:  CavePainting-Lite

This example program is meant to illustrate how to create a MinVR program
that includes an interesting 3D user interface.  Here, the user interface
assumes that each hand holds a 6 DOF tracker with one button on it.  A
3D cursor follows each hand -- the right hand cursor is a paintbrush, and
the left hand is a cube.  When the right hand button is pressed and held
then the brush begins to paint a trail of multi-colored spherical "3D paint
blobs".  When the left hand button is pressed and held, this "grabs onto"
the entire 3D painting so that the user can then translate and rotate
the scene.  The interface is a super-simplified version of the CavePainting
tool (Keefe et al., 2001).

Desktop Controls:
```
LeftMouseBtn  -- hold and move mouse to start painting with the paintbrush tracker
RightMouseBtn -- hold and move mouse to look around (moving the head tracker)
SPACE         -- hold and move mouse to grab on to the painting with the left hand tracker
MOUSE         -- translates mouse movement to "fake" tracker events for hands and head
R             -- hold and move mouse to rotate the hand trackers
1             -- press to toggle tracker 1 (head tracker) on/off
2             -- press to toggle tracker 2 (paintbrush) on/off
3             -- press to toggle tracker 3 (left hand) on/off
ARROW KEYS    -- move the head tracker forward/back or turn left/right
```
