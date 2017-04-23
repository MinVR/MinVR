# MinVR Python Plugin

This plugin contains Python specific implementations for MinVR

## Getting Started

To begin using the Python plugin of MinVR, start by adding the MinVR module path
and importing MinVR.

  ```python
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
  ```

Then create an application as an VREventHandler and VRRenderHandler

  ```python
  from MinVR import *
  
  # Application class which handles events and rendering
  class App(VREventHandler, VRRenderHandler):

	# Called when an event is passed from MinVR
	def onVREvent(self, eventName):
		# Event handling code
		
	# Renders at the context level
	def onVRRenderContext(self, renderState):
		initRender = renderState.getValue("InitRender","/")
		if initRender:
			print "Initialize context variables"

	# Renders the scene
	def onVRRenderScene(self, renderState):
		# You can retrieve values from the renderState		
		width = renderState.getValue("WindowWidth","/")
		height = renderState.getValue("WindowHeight","/")
		# Render Code
  ```

Finally, you can create the VRMain object by passing in a configuration path and
start the main loop

  ```python
  # Create application
  app = App()

  # Create VRMain instance passing in vrsetup configuration
  config = sys.argv[1]
  vrmain = VRMain(config)

  # Add event handler and render handler
  vrmain.addEventHandler(app)
  vrmain.addRenderHandler(app)

  # Main loop
  while True:
	vrmain.mainloop()

  # Shutdown MinVR
  vrmain.shutdown()
  ```

To execute the program, run the following from the command line:

  ```
  python program.py -d /path/to/MinVR/install_or_build_directory -c desktop-oldopengl
  ```

