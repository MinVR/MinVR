#!/usr/bin/python

import sys, os, inspect
import argparse
parser = argparse.ArgumentParser()
parser.add_argument("-d", help="MinVR Directory")
parser.add_argument("-c", help="MinVR Config")
parser.add_argument("-f", help="Config File")
parser.add_argument("-s", help="Config Values")
args = parser.parse_args()

minvr_dir = os.environ.get('MinVR_DIR')
if (args.d):
	minvr_dir = args.d

if (minvr_dir):
	sys.path.append(minvr_dir + "/plugins/MinVR_Python/python")
else:
	print("Please supply MinVR_DIR with environment variable 'MinVR_DIR' or -d")
	exit(0)

# --------- MinVR Implementation -----------------

# Import MinVR and OpenGL Dependencies
from MinVR import *
from OpenGL.GL import *
from OpenGL.GLU import *


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

	# Renders the scene
	def onVRRenderScene(self, renderState):
		width = renderState.getValue("WindowWidth","/")
		height = renderState.getValue("WindowHeight","/")
		ProjectionMatrix = list(renderState.getValue("ProjectionMatrix","/")[0:16])
		ViewMatrix = list(renderState.getValue("ViewMatrix","/")[0:16])
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


