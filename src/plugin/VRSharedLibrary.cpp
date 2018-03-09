/* ================================================================================

This file is part of the MinVR Open Source Project.

File: extend/SharedLibrary.h

Original Author(s) of this File:
	Dan Orban, 2015, University of Minnesota

Author(s) of Significant Updates/Modifications to the File:
	...

-----------------------------------------------------------------------------------
Copyright (c) 2015 Regents of the University of Minnesota
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

* The name of the University of Minnesota, nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
================================================================================ */

#include "VRSharedLibrary.h"
#include <iostream>

namespace MinVR {

VRSharedLibrary::VRSharedLibrary(const std::string &filePath, bool autoLoad) : _filePath(filePath), _isLoaded(false) {
	if (autoLoad)
	{
		load();
	}
}

VRSharedLibrary::~VRSharedLibrary() {
	unload();
}

void VRSharedLibrary::load() {
	if (!_isLoaded) {

		const char* error = "";

#if defined(WIN32)
		_lib = LoadLibraryA(_filePath.c_str());
		if (_lib==NULL) {

			LPVOID lpMsgBuf;
			DWORD dw = GetLastError();

			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				dw,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)&lpMsgBuf,
				0, NULL);

			error = static_cast<const char *>(lpMsgBuf);
		}

#else
		dlerror();
		_lib = dlopen(_filePath.c_str(), RTLD_NOW);//RTLD_LAZY);
		error = dlerror();
#endif

		if (!_lib) {

#ifdef MinVR_DEBUG
			VRWARNING("Could not load library: " + _filePath + " - " + error,
                "This is a harmless warning, unless the library can't be found anywhere else.");
#endif
			return;

		}

    _isLoaded = true;

    DEBUGMSG("Loaded library: " + _filePath)
	}
}

void VRSharedLibrary::unload() {
	if (_isLoaded)
	{
		const char* error = "";
#if defined(WIN32)
		BOOL result = FreeLibrary(_lib);

		if (result != 0) {

			LPVOID lpMsgBuf;
			DWORD dw = GetLastError();

			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				dw,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)&lpMsgBuf,
				0, NULL);

			error = static_cast<const char *>(lpMsgBuf);
		}
#else
		dlerror();
		int result = dlclose(_lib);
		error = dlerror();
#endif
		if(result != 0) {
      VRWARNINGNOADV("Could not unload library: " + _filePath + " - " + error);
			return;
		}

		_isLoaded = false;
	}
}

void* VRSharedLibrary::loadSymbolInternal(const std::string &functionName) {
	if (_isLoaded)
	{
#if defined(WIN32)
		FARPROC symbol =GetProcAddress(_lib, functionName.c_str());
		if (!symbol) {
			//MinVR::Logger::getInstance().assertMessage(false, "Cannot load symbol: " + functionName + " - " + "");

			return NULL;
		}

		return symbol;
#else
		void* symbol = (void*) dlsym(_lib, functionName.c_str());
		const char* dlsym_error = dlerror();
		if (dlsym_error) {
			//MinVR::Logger::getInstance().assertMessage(false, "Cannot load symbol: " + functionName + " - " + dlsym_error);
			dlerror();

			return NULL;
		}

		return symbol;
#endif

	}

	return NULL;
}

} /* namespace extend */

