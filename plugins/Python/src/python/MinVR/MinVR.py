import sys

from ctypes import cdll
import ctypes
import xml.etree.ElementTree

libName = 'MinVR_Python'

from sys import platform as _platform
if _platform == "linux" or _platform == "linux2":
    libFilePath = 'lib/lib' + libName + '.so'
elif _platform == "darwin":
    libFilePath = 'lib/lib' + libName + '.dylib'
elif _platform == "win32":
    libFilePath = 'bin/' + libName + '.dll'

lib = None

def openLibrary(minvr_dir):
	#e = xml.etree.ElementTree.parse(config).getroot()
	#pluginpath = e.findall('PluginPath')[0].text
	global lib
	lib = cdll.LoadLibrary(minvr_dir + '/plugins/' + libName + '/' + libFilePath)
	print(minvr_dir + '/plugins/' + libName + '/' + libFilePath)

eventcallback_type = ctypes.CFUNCTYPE(None, ctypes.c_char_p)
rendercallback_type = ctypes.CFUNCTYPE(None, ctypes.c_void_p)

class VRMain(object):
	def __init__(self, minvr_dir, argv):
		openLibrary(minvr_dir)
		arr = (ctypes.c_char_p * len(argv))()
		arr[:] = argv;
		self.obj = lib.VRMain_init(minvr_dir + '/plugins', len(argv), arr)
		self.eventHandlers = []
		self.renderHandlers = []
		self.eventCB = self.getEventCallbackFunc()
		self.eventhandler = lib.VRMain_registerEventCallback(self.obj, self.eventCB)
		self.renderCB = self.getRenderCallbackFunc()
		self.renderhandler = lib.VRMain_registerRenderCallback(self.obj, self.renderCB)
		pluginList = ctypes.create_string_buffer(500)
		lib.setPluginList.restype = ctypes.c_bool
		hasPlugins = lib.setPluginList(self.obj, pluginList)
		if (hasPlugins):
			for plugin in pluginList.value.split(","):
				sys.path.append(pluginpath + '/' + plugin + '/python')
				a = __import__(plugin)
				a.registerWithMinVR(self)
	def shutdown(self):
		lib.VRMain_shutdown(self.obj, self.eventhandler, self.renderhandler)
	def addEventHandler(self, handler):
		self.eventHandlers.append(handler)
	def addRenderHandler(self, handler):
		self.renderHandlers.append(handler)
	def getEventCallbackFunc(self):
		def func(eventName):
			self.handleEvent(eventName)
		return eventcallback_type(func)
	def handleEvent(self, eventName):
		eName = eventName
		for handler in self.eventHandlers:
			handler.onVREvent(eName)
	def getRenderCallbackFunc(self):
		def func(renderState):
			self.handleRender(renderState)
		return rendercallback_type(func)
	def handleRender(self, renderState):
		for handler in self.renderHandlers:
			handler.onVRRenderScene(VRDataIndex(renderState))
	def mainloop(self):
		lib.VRMain_mainloop(self.obj)

class VREventHandler(object):
	def __init__(self):
		pass
	def onVREvent(self, eventName):
		print "Event"

class VRRenderHandler(object):
	def __init__(self):
		pass
	def onVRRenderScene(self, renderState):
		print "Rendering Scene"
	def onVRRenderContext(self):
		print "Rendering Context"

class VRDataIndex(object):
	def __init__(self, index):
		self.getDatumType = lib.VRDataIndex_getType
		self.getDatumType.argtypes = (ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p)
		self.getIntValue = lib.VRDataIndex_getIntValue
		self.getIntValue.argtypes = (ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p)
		self.index = index
	def getValue(self, valName, nameSpace):
		datumType = self.getDatumType(self.index, valName, nameSpace)
		if datumType == 1:
			return self.getIntValue(self.index, valName, nameSpace)
		return None


