#!/usr/bin/python

# --------- Initialize MinVR from Command Line (Copy this for any python implementation) -----------------
# The most important thing to pass in is the path ("-d" option) to the MinVR directory which could be the
# install directory or the build directory.  Another approach is to set the "MinVR_DIR" environment
# variable pointing to the install path.  If the "-d" option is not supplied, it will use the environment
# variable.

import sys, os, inspect
import argparse
parser = argparse.ArgumentParser()
parser.add_argument("-d", help="MinVR Directory (i.e. /path/to/MinVR)")
parser.add_argument("-c", help="Optional: MinVR Config")
parser.add_argument("-f", help="Optional: Config File")
parser.add_argument("-s", help="Optional: Config Values")
args = parser.parse_args()

minvr_dir = os.environ.get('MinVR_DIR')
if (args.d):
	minvr_dir = args.d

if (minvr_dir):
	sys.path.append(minvr_dir + "/plugins/MinVR_Python/python")
else:
	print("Please supply MinVR_DIR with environment variable 'MinVR_DIR' or -d")
	exit(0)

# --------- MinVR Application Implementation -----------------
# In order to use MinVR, one only needs to import MinVR.  If graphics are desired
# a user can include OpenGL python libraries.

# Import MinVR and OpenGL Dependencies
from MinVR import *
from OpenGL.GL import *
from OpenGL.GLU import *

# Cube vertices
verticies = (
    (1, -1, -1),
    (1, 1, -1),
    (-1, 1, -1),
    (-1, -1, -1),
    (1, -1, 1),
    (1, 1, 1),
    (-1, -1, 1),
    (-1, 1, 1)
    )

# Cube edges
edges = (
    (0,1),
    (0,3),
    (0,4),
    (2,1),
    (2,3),
    (2,7),
    (6,3),
    (6,4),
    (6,7),
    (5,1),
    (5,4),
    (5,7)
    )

# Draws a cube
def Cube():
    glBegin(GL_LINES)
    for edge in edges:
        for vertex in edge:
            glVertex3fv(verticies[vertex])
    glEnd()

# Application class which handles events and rendering
class App(VREventHandler, VRRenderHandler):
	def __init__(self):
		self.loop = True
		self.time = 0.0

	# Called when an event is passed from MinVR
	def onVREvent(self, eventName, event):
		#print eventName
		if eventName == "KbdEsc_Down":
			self.loop = False
		elif eventName == "FrameStart":
			self.time = event.getValue("ElapsedSeconds", eventName)

	# Renders at the context level
	def onVRRenderContext(self, renderState):
		initRender = renderState.getValue("InitRender","/")
		if initRender:
			print "Initialize context variables"

	# Renders the scene
	def onVRRenderScene(self, renderState):
		width = renderState.getValue("WindowWidth","/")
		height = renderState.getValue("WindowHeight","/")
		ProjectionMatrix = renderState.getValue("ProjectionMatrix","/")
		ViewMatrix = renderState.getValue("ViewMatrix","/")
		ratio = width / height;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST)
		glMatrixMode(GL_PROJECTION)
		glLoadMatrixf(ProjectionMatrix)
		glMatrixMode(GL_MODELVIEW)
		glLoadIdentity()
		glMatrixMode(GL_MODELVIEW)
		glLoadMatrixf(ViewMatrix)
		glRotatef(self.time*30, self.time*10, self.time*30, self.time*30)
		glBegin(GL_TRIANGLES)
		glColor3f(1, 0, 0)
		glVertex3f(-0.6, -0.4, 0.0)
		glColor3f(0.0, 1.0, 0.0)
		glVertex3f(0.6, -0.4, 0.0)
		glColor3f(0.0, 0.0, 1.0)
		glVertex3f(0.0, 0.60, 0.0)
		glEnd();
		Cube()

# ----------- Main program ------------------

# Create application
app = App()

# Create VRMain instance passing in vrsetup configuration
vrmain = VRMain(minvr_dir, sys.argv)

# Add event handler and render handler
vrmain.addEventHandler(app)
vrmain.addRenderHandler(app)

# Main loop
while app.loop:
	vrmain.mainloop()

# Shutdown MinVR
vrmain.shutdown()


