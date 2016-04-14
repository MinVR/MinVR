

Compile the example app, then run it with any of these commands:


// Opens a single 3D graphics display window on the desktop, no stereo
vrexample desktop.xml


// Opens a fullscreen 3D graphics window with side-by-side stereo
vrexample 3DTV.xml


// Forks 3 processes (quadbuffered stereo, one per graphics window) in 
// the layout expected by the UMN IV/LAB Cave.  The original process
// becomes a console-only server that loads input devices.
vrexample ivlabcave.xml


// The first line starts a single process that loads the Projector #1 
// VRSetup specified in the Brown Univeristy YURT config file. The 
// next line does the same but for Projector #2.  The last line starts
// a console-only server that loads the YURT input devices.  To really
// run in the YURT, these commands would need to be run in a script
// that ssh-es into the appropriate machine, then runs the command.
vrexample YURT.xml Proj01
vrexample YURT.xml Proj02
...
vrexample YURT.xml Proj69
vrexample YURT.xml Server
