#!/usr/bin/python

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

# --------- MinVR Implementation -----------------

# Import MinVR and OpenGL Dependencies
from MinVR import *
from OpenGL.GL import *

# Application class which handles events and rendering
class App(VREventHandler, VRRenderHandler):
	def __init__(self):
		self.loop = True
		self.rotateAngle = 0.0

	# Called when an event is passed from MinVR
	def onVREvent(self, eventName, event):
		#print eventName
		if eventName == "KbdEsc_Down":
			self.loop = False
		elif eventName == "KbdRight_Down" or eventName == "KbdRight_Repeat":
			self.rotateAngle += 0.05
		elif eventName == "KbdLeft_Down" or eventName == "KbdLeft_Repeat":
			self.rotateAngle -= 0.05

	# Renders at the context level
	def onVRRenderContext(self, renderState):
		initRender = renderState.getValue("InitRender","/")
		if initRender:
			print "Initialize context variables"

	# Renders the scene
	def onVRRenderScene(self, renderState):
		glClear(GL_COLOR_BUFFER_BIT)
		width = renderState.getValue("WindowWidth","/")
		height = renderState.getValue("WindowHeight","/")
		ratio = width / height;
		glClear(GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-ratio, ratio, -1.0, 1.0, 1.0, -1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(self.rotateAngle*50.0, 0.0, 0.0, 1.0)
		glBegin(GL_TRIANGLES)
		glColor3f(1, 0, 0)
		glVertex3f(-0.6, -0.4, 0.0)
		glColor3f(0.0, 1.0, 0.0)
		glVertex3f(0.6, -0.4, 0.0)
		glColor3f(0.0, 0.0, 1.0)
		glVertex3f(0.0, 0.60, 0.0)
		glEnd();

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

