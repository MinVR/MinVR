# ================================================================================
# 
# This file is part of the MinVR Open Source Project, which is developed and
# maintained by the University of Minnesota's Interactive Visualization Lab.
# 
# File: CMakeLists.txt
# 
# Original Author(s) of this File:
# 	Dan Orban, 2015, University of Minnesota
# 
# Author(s) of Significant Updates/Modifications to the File:
# 	...
# 
# -----------------------------------------------------------------------------------
# Copyright (c) 2015 Regents of the University of Minnesota
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
# 
# * Redistributions in binary form must reproduce the above copyright notice, this
#   list of conditions and the following disclaimer in the documentation and/or
#   other materials provided with the distribution.
# 
# * The name of the University of Minnesota, nor the names of its
#   contributors may be used to endorse or promote products derived from
#   this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# ================================================================================ 

#.SILENT:

ifneq ($(WINDIR),)
  ARCH=WIN32
  ifeq ($(shell uname -o), Cygwin)
    WINCURDIR = $(shell cygpath -wm $(CURDIR))
  else
    WINCURDIR = $(CURDIR)
  endif
else
  UNAME=$(shell uname)
  ifeq ($(UNAME), Linux)
    ARCH=linux
    NPROCS = $(shell nproc)
  else ifeq ($(UNAME), Darwin)
    ARCH=OSX
  else
    $(error The Makefile does not recognize the architecture: $(UNAME))
  endif
endif

DIRNAME=$(shell basename `pwd`)

all: gen

gen:
	mkdir -p ./build
   ifeq ($(ARCH), linux)
	mkdir -p ./build/Release
	mkdir -p ./build/Debug
	mkdir -p ../$(DIRNAME)_eclipse
	cd ./build/Release; cmake -DCMAKE_BUILD_TYPE=Release ../../
	cd ./build/Debug; cmake -DCMAKE_BUILD_TYPE=Debug ../../
	cd ../$(DIRNAME)_eclipse; cmake -DCMAKE_BUILD_TYPE=Debug $(CURDIR) -G "Eclipse CDT4 - Unix Makefiles" -DCMAKE_ECLIPSE_VERSION=4.3
    else ifeq ($(ARCH), WIN32)
	cd ./build; cmake ../ -G "Visual Studio 12 Win64"
    else ifeq ($(ARCH), OSX)
	cd ./build; cmake ../ -G Xcode
    endif

debug:
    ifeq ($(ARCH), linux)
	cd ./build/Debug; make -j$(NPROC)
    else
	@echo "Open the project file to build the project on this architecture."
    endif

release opt:
    ifeq ($(ARCH), linux)
	cd ./build/Release; make -j$(NPROC)
    else
	@echo "Open the project file to build the project on this architecture."
    endif

install:
    ifeq ($(ARCH), linux)
	cd ./build/Debug; make install
	cd ./build/Release; make install
    else
	@echo "Open the project file to run make install on this architecture."
    endif

clean:
    ifeq ($(ARCH), linux)
	cd ./build/Debug; make clean
	cd ./build/Release; make clean
    else
	@echo "Open the project file to run make clean on this architecture."
    endif

clobber:
	rm -rf ./build
	rm -rf ./install
    ifeq ($(ARCH), linux)
	rm -rf ../$(DIRNAME)_eclipse
    endif
