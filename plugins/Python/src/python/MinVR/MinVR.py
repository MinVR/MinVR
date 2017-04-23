import sys

from ctypes import cdll
import ctypes
import os.path

libName = 'MinVR_Python'

from sys import platform as _platform
if _platform == "linux" or _platform == "linux2":
    libFilePath = 'lib/lib' + libName
    libExtension = '.so'
elif _platform == "darwin":
    libFilePath = 'lib/lib' + libName
    libExtension = '.dylib'
elif _platform == "win32":
    libFilePath = 'bin/' + libName
    libExtension = '.dll'

lib = None

def openLibrary(minvr_dir):
	global lib
	pluginPath = minvr_dir + '/plugins/' + libName + '/' + libFilePath + libExtension
	if (not os.path.exists(pluginPath)):
		pluginPath = minvr_dir + '/plugins/' + libName + '/' + libFilePath + 'd' + libExtension
	print(pluginPath)
	lib = cdll.LoadLibrary(pluginPath)

eventcallback_type = ctypes.CFUNCTYPE(None, ctypes.c_char_p, ctypes.c_void_p)
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
		self.renderContextCB = self.getRenderContextCallbackFunc()
		self.renderhandler = lib.VRMain_registerRenderCallback(self.obj, self.renderCB, self.renderContextCB)
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
		def func(eventName, event):
			self.handleEvent(eventName, event)
		return eventcallback_type(func)
	def handleEvent(self, eventName, event):
		eName = eventName
		for handler in self.eventHandlers:
			handler.onVREvent(eName, VRDataIndex(event))
	def getRenderCallbackFunc(self):
		def func(renderState):
			self.handleRender(renderState)
		return rendercallback_type(func)
	def getRenderContextCallbackFunc(self):
		def func(renderState):
			self.handleRenderContext(renderState)
		return rendercallback_type(func)
	def handleRender(self, renderState):
		for handler in self.renderHandlers:
			handler.onVRRenderScene(VRDataIndex(renderState))
	def handleRenderContext(self, renderState):
		for handler in self.renderHandlers:
			handler.onVRRenderContext(VRDataIndex(renderState))
	def mainloop(self):
		lib.VRMain_mainloop(self.obj)

class VREventHandler(object):
	def __init__(self):
		pass
	def onVREvent(self, eventName, event):
		print "Event"

class VRRenderHandler(object):
	def __init__(self):
		pass
	def onVRRenderScene(self, renderState):
		print "Rendering Scene"
	def onVRRenderContext(self, renderState):
		print "Rendering Context"

class VRDataIndex(object):
	def __init__(self, index):
		self.getDatumType = lib.VRDataIndex_getType
		self.getDatumType.argtypes = (ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p)
		self.freeValue = lib.VRDataIndex_freeValue
		self.freeValue.argtypes = (ctypes.c_int, ctypes.c_void_p)
		self.getIntValue = lib.VRDataIndex_getIntValue
		self.getIntValue.argtypes = (ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p)
		self.getIntValue.restype = ctypes.c_int
		self.getFloatValue = lib.VRDataIndex_getFloatValue
		self.getFloatValue.argtypes = (ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p)
		self.getFloatValue.restype = ctypes.c_float
		self.getStringValue = lib.VRDataIndex_getStringValue
		self.getStringValue.argtypes = (ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p)
		self.getStringValue.restype = ctypes.c_char_p
		self.getIntArrayValue = lib.VRDataIndex_getIntArrayValue
		self.getIntArrayValue.argtypes = (ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_int))
		self.getIntArrayValue.restype = ctypes.POINTER(ctypes.c_int)
		self.getFloatArrayValue = lib.VRDataIndex_getFloatArrayValue
		self.getFloatArrayValue.argtypes = (ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_int))
		self.getFloatArrayValue.restype = ctypes.POINTER(ctypes.c_float)
		self.index = index
		self.toBeDeleted = []
	def __del__(self):
		for i in self.toBeDeleted:
			self.freeValue(i[0],i[1])
	def getValue(self, valName, nameSpace):
		datumType = self.getDatumType(self.index, valName, nameSpace)
		if datumType == 1:
			return self.getIntValue(self.index, valName, nameSpace)
		if datumType == 2:
			return self.getFloatValue(self.index, valName, nameSpace)
		if datumType == 3:
			self.toBeDeleted.append([datumType, valName]);
			return self.getStringValue(self.index, valName, nameSpace)
		if datumType == 4:
			arrSize = ctypes.c_int()
			a = self.getIntArrayValue(self.index, valName, nameSpace, ctypes.byref(arrSize))
			self.toBeDeleted.append([datumType, a])
			print arrSize
			return list(a[0:arrSize.value])
		if datumType == 5:
			arrSize = ctypes.c_int()
			a = self.getFloatArrayValue(self.index, valName, nameSpace, ctypes.byref(arrSize))
			self.toBeDeleted.append([datumType, a])
			return list(a[0:arrSize.value])
		return None


