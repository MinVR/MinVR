/* ================================================================================

This file is part of the MinVR Open Source Project, which is developed and
maintained collaboratively by the University of Minnesota's Interactive
Visualization Lab and the Center for Computation and Visualization at Brown University

File: VRPhotonDevice.cpp

Original Author(s) of this File:
	8/4/2020 - Benjamin Knorlein - Brown Universiy

Author(s) of Significant Updates/Modifications to the File:

-----------------------------------------------------------------------------------
Copyright (c) 2020 Brown University
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

* Neither the name of the University of Minnesota, Brown University, nor the names of its
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


// Note: This include ordering is important!
#include "VRPhotonDevice.h"
#include <iostream>
#include <api/VRAnalogEvent.h>

//#define COUNTPACKAGES
//#define WRITEEVENTSTOFILE

namespace MinVR {


VRPhotonDevice::VRPhotonDevice(std::string appName, std::string appID, std::string appVersion, std::string playerName
	, bool receiveOnly, std::vector<std::string> whitelist, std::vector<std::string> replacements, float updateSpeed) : m_receiveOnly{ receiveOnly }, m_lastsend{ 0 }, m_lastreceived{ -1 }, m_photon{ nullptr }, m_output_listener{ nullptr }, isRunning{ true }, m_updateSpeed{updateSpeed}
{
	PhotonLib::gameName = appName.c_str();
	PhotonLib::appID = appID.c_str();
	PhotonLib::appVersion = appVersion.c_str();
	PhotonLib::PLAYER_NAME = playerName.c_str();

	m_output_listener = new StdIO_UIListener();
	m_photon = new PhotonLib(m_output_listener);
	th1 = new std::thread(&VRPhotonDevice::update_thread, this);

	for (auto val : whitelist) {
		m_whitelist.insert(val);
	}

	for (int i = 0; i < replacements.size(); i+=2) {
		m_replacements.insert(std::pair<std::string, std::string>( replacements[i], replacements[i + 1]));
	}
}

void VRPhotonDevice::update_thread() {
	while (isRunning) {
		mtx.lock();
		if (m_photon) {
			m_photon->update();
			if (!sendQueue.empty()) {
				m_photon->sendData(sendQueue.serialize());
				sendQueue.clear();
			}
			m_photon->update();
		}
		mtx.unlock();
		Sleep(m_updateSpeed);
	}
}

VRPhotonDevice::~VRPhotonDevice()
{
	isRunning = false;
	th1->join();
	delete th1;

	delete m_photon;
	delete m_output_listener;
}

void VRPhotonDevice::appendNewInputEventsSinceLastCall(VRDataQueue *inputEvents) {
	if (m_photon && m_photon->isConnected() && !m_receiveOnly) {
		mtx.lock();
			for (VRDataQueue::iterator iter = inputEvents->begin(); iter != inputEvents->end(); iter++) {
				//check if event is blacklisted
				if (m_whitelist.empty() || m_whitelist.find(iter->second.getData().getName()) != m_whitelist.end())
				{
					VRDataIndex tmpidx = iter->second.getData();
					//change name if required
					if (m_replacements.find(tmpidx.getName()) != m_replacements.end()) {
						tmpidx.setName(m_replacements[tmpidx.getName()]);
					}
					VRDataQueueItem item = VRDataQueueItem(tmpidx.serialize());
					sendQueue.push(iter->first.first, item);
				}
			}
			if (m_lastsend >= INT_MAX)
				m_lastsend = 0;
			sendQueue.push(VRAnalogEvent::createValidDataIndex("PhotonLoopFinished", m_lastsend++));
		mtx.unlock();
		
#ifdef WRITEEVENTSTOFILE
		std::ofstream outfile;
		outfile.open("send.txt", std::ios_base::app); // append instead of overwrite
		outfile << sendQueue.serialize() << std::endl;
		outfile.close();
#endif
		
	}
	mtx.lock();
		for (int f = 0; f < m_photon->getPendingEvents().size(); f++) {
			if (m_photon)
				inputEvents->addSerializedQueue(m_photon->getPendingEvents()[f]);
#ifdef COUNTPACKAGES
			VRDataQueue tmpQueue;
			tmpQueue.addSerializedQueue(m_photon->getPendingEvents()[f]);
			for (VRDataQueue::iterator iter = tmpQueue.begin(); iter != tmpQueue.end(); iter++) {
				if (iter->second.getData().getName() == "PhotonLoopFinished" && iter->second.getData().exists("AnalogValue")) {
					int id = iter->second.getData().getValue("AnalogValue");
					if (m_lastreceived == -1) {
						m_lastreceived = id;
						std::cerr << "START " << m_lastreceived << std::endl;
					}
					if (id == m_lastreceived) {
						m_lastreceived++;
						std::cerr << "In order " << id << std::endl;
					}
					else {
						std::cerr << "!!!!!!!!!!!!!!! OUT OF ORDER !!!!!!!!!!!!!!!!" << std::endl;
					}
				}
			}
			tmpQueue.clear();
#endif
#ifdef WRITEEVENTSTOFILE
			std::ofstream outfile;
			outfile.open("receive.txt", std::ios_base::app); // append instead of overwrite
			outfile << m_photon->getPendingEvents()[f] << std::endl;
			outfile.close();
#endif
		}

		m_photon->getPendingEvents().clear();
	mtx.unlock();

#ifdef WRITEEVENTSTOFILE
	if (m_photon->isConnected() && m_receiveOnly) {
		std::ofstream outfile;
		outfile.open("receive_inputs.txt", std::ios_base::app); // append instead of overwrite
		outfile << inputEvents->serialize() << std::endl;
		outfile.close();
	}
#endif
}


VRInputDevice*
VRPhotonDevice::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace) {
	std::string devNameSpace = nameSpace;

	std::string appName = config->getValue("AppName", devNameSpace);
	std::string appID = config->getValue("AppID", devNameSpace);
	std::string appVersion = config->getValue("AppVersion", devNameSpace);
	std::string playerName = config->getValue("PlayerName", devNameSpace);

	bool receiveOnly = false;
	if (config->exists("ReceiveOnly", devNameSpace))
	{
		receiveOnly = true;
	}

	std::vector<std::string> whitelist;
	if (config->exists("Whitelist", devNameSpace))
	{
		whitelist = config->getValue("Whitelist", devNameSpace);
	}
	std::vector<std::string> replaceList;
	if (config->exists("Replacements", devNameSpace))
	{
		replaceList = config->getValue("Replacements", devNameSpace);
	}

	float updateSpeed = 33.0f;
	if (config->exists("PhotonUpdateSpeed", devNameSpace))
	{
		updateSpeed = config->getValue("PhotonUpdateSpeed", devNameSpace);
	}

	VRInputDevice *dev = new VRPhotonDevice(appName, appID, appVersion, playerName, receiveOnly, whitelist, replaceList, updateSpeed);
	return dev;
}


} // end namespace

