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
		self.a = 3

	# Called when an event is passed from MinVR
	def onVREvent(self, eventName):
		print eventName
		if eventName == "/KbdEsc_Down":
			self.loop = False

	# Renders the scene
	def onVRRenderScene(self, renderState):
		glClear(GL_COLOR_BUFFER_BIT)
		width = renderState.getValue("WindowWidth","/")
		height = renderState.getValue("WindowHeight","/")
		ratio = width / height;
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45, ((1.0*width)/(1.0*height)), 0.1, 50.0)
		glBegin(GL_TRIANGLES);
		glColor3f(1, 0, 0);
		glVertex3f(-0.6, -0.4, 0.0);
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0.6, -0.4, 0.0);
		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(0.0, 0.60, 0.0);
		glEnd();
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                glMatrixMode(GL_MODELVIEW);
                glTranslatef(0.0,0.0, -5)
                glRotatef(self.a/3, self.a, self.a/3, self.a/3)
                Cube()

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
        app.a += 3
	vrmain.mainloop()

# Shutdown MinVR
vrmain.shutdown()


