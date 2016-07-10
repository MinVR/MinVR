# MinVR Python Plugin

This plugin contains Python specific implementations for MinVR

## Getting Started

To begin using the Python plugin of MinVR, start by adding the MinVR module path
and importing MinVR.

  ```python
  sys.path.append("../../plugins/Python/src/python")
  from MinVR import *
  ```

Then create an application as an VREventHandler and VRRenderHandler

  ```python
  # Application class which handles events and rendering
  class App(VREventHandler, VRRenderHandler):

	# Called when an event is passed from MinVR
	def onVREvent(self, eventName):
		# Event handling code

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
  python example.py desktop.xml
  ```

