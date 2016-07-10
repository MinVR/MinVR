#!/usr/bin/python

# Use file's current directory and append path to MinVR Module
import sys, os, inspect
fileDir = os.path.dirname(os.path.abspath(inspect.getsourcefile(lambda:0)))
os.chdir(fileDir)
sys.path.append("../../plugins/Python/src/python")

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
	def onVREvent(self, eventName):
		print eventName
		if eventName == "/KbdEsc_Down":
			self.loop = False
		elif eventName == "/KbdRight_Down" or eventName == "/KbdRight_Repeat":
			self.rotateAngle += 0.1
		elif eventName == "/KbdLeft_Down" or eventName == "/KbdLeft_Repeat":
			self.rotateAngle -= 0.1

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
		glRotatef(self.rotateAngle*50.0, 0.0, 0.0, 1.0);
		glBegin(GL_TRIANGLES);
		glColor3f(1, 0, 0);
		glVertex3f(-0.6, -0.4, 0.0);
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0.6, -0.4, 0.0);
		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(0.0, 0.60, 0.0);
		glEnd();

# ----------- Main program ------------------

# Create application
app = App()

# Create VRMain instance passing in vrsetup configuration
config = sys.argv[1]
vrmain = VRMain(config)

# Add event handler and render handler
vrmain.addEventHandler(app)
vrmain.addRenderHandler(app)

# Main loop
while app.loop:
	vrmain.mainloop()

# Shutdown MinVR
vrmain.shutdown()

