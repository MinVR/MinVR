/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Ben Knorlein
 */



#include "VROpenVRRenderModelHandler.h"
#include "VROpenVRRenderModel.h"
#include "VROpenVRInputDevice.h"
#include <iostream>


#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace MinVR {
	VROpenVRRenderModelHandler::VROpenVRRenderModelHandler(vr::IVRSystem *pHMD, VROpenVRInputDevice* inputDevice) : m_pHMD(pHMD), m_inputDevice(inputDevice), m_unRenderModelProgramID(0)
	{
		memset(m_rTrackedDeviceToRenderModel, 0, sizeof(m_rTrackedDeviceToRenderModel));

		if (!m_pHMD)
			return;

		for (uint32_t unTrackedDevice = vr::k_unTrackedDeviceIndex_Hmd + 1; unTrackedDevice < vr::k_unMaxTrackedDeviceCount; unTrackedDevice++)
		{
			if (!m_pHMD->IsTrackedDeviceConnected(unTrackedDevice))
				continue;

			queueModelForLoading(unTrackedDevice);
		}
	}

	VROpenVRRenderModelHandler::~VROpenVRRenderModelHandler()
	{
		for (std::vector< VROpenVRRenderModel * >::iterator i = m_vecRenderModels.begin(); i != m_vecRenderModels.end(); i++)
		{
			delete (*i);
		}
		m_vecRenderModels.clear();

		if (m_unRenderModelProgramID)
		{
			glDeleteProgram(m_unRenderModelProgramID);
		}
	}

	GLuint CompileGLShader(const char *pchShaderName, const char *pchVertexShader, const char *pchFragmentShader)
	{
		GLuint unProgramID = glCreateProgram();

		GLuint nSceneVertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(nSceneVertexShader, 1, &pchVertexShader, NULL);
		glCompileShader(nSceneVertexShader);

		GLint vShaderCompiled = GL_FALSE;
		glGetShaderiv(nSceneVertexShader, GL_COMPILE_STATUS, &vShaderCompiled);
		if (vShaderCompiled != GL_TRUE)
		{
			//dprintf("%s - Unable to compile vertex shader %d!\n", pchShaderName, nSceneVertexShader);
			glDeleteProgram(unProgramID);
			glDeleteShader(nSceneVertexShader);
			return 0;
		}
		glAttachShader(unProgramID, nSceneVertexShader);
		glDeleteShader(nSceneVertexShader); // the program hangs onto this once it's attached

		GLuint  nSceneFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(nSceneFragmentShader, 1, &pchFragmentShader, NULL);
		glCompileShader(nSceneFragmentShader);

		GLint fShaderCompiled = GL_FALSE;
		glGetShaderiv(nSceneFragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
		if (fShaderCompiled != GL_TRUE)
		{
			//dprintf("%s - Unable to compile fragment shader %d!\n", pchShaderName, nSceneFragmentShader);
			glDeleteProgram(unProgramID);
			glDeleteShader(nSceneFragmentShader);
			return 0;
		}

		glAttachShader(unProgramID, nSceneFragmentShader);
		glDeleteShader(nSceneFragmentShader); // the program hangs onto this once it's attached

		glLinkProgram(unProgramID);

		GLint programSuccess = GL_TRUE;
		glGetProgramiv(unProgramID, GL_LINK_STATUS, &programSuccess);
		if (programSuccess != GL_TRUE)
		{
			//dprintf("%s - Error linking program %d!\n", pchShaderName, unProgramID);
			glDeleteProgram(unProgramID);
			return 0;
		}

		glUseProgram(unProgramID);
		glUseProgram(0);

		return unProgramID;
	}

	void VROpenVRRenderModelHandler::initModels()
	{
		if (!m_unRenderModelProgramID)
		{
			m_unRenderModelProgramID = CompileGLShader(
				"render model",

				// vertex shader
				"#version 410 compatibility\n"
				"layout(location = 0) in vec4 position;\n"
				"layout(location = 1) in vec3 v3NormalIn;\n"
				"layout(location = 2) in vec2 v2TexCoordsIn;\n"
				"out vec2 v2TexCoord;\n"
				"void main()\n"
				"{\n"
				"	v2TexCoord = v2TexCoordsIn;\n"
				"	gl_Position = gl_ModelViewProjectionMatrix * vec4(position.xyz, 1);\n"
				"}\n",

				//fragment shader
				"#version 410 compatibility\n"
				"uniform sampler2D diffuse;\n"
				"in vec2 v2TexCoord;\n"
				"out vec4 outputColor;\n"
				"void main()\n"
				"{\n"
				"   outputColor = texture( diffuse, v2TexCoord);\n"
				"}\n"

				);
		}

		while (!modelLoaderQueue.empty())
		{
			vr::TrackedDeviceIndex_t index = modelLoaderQueue.front();
			modelLoaderQueue.pop();

			if (index >= vr::k_unMaxTrackedDeviceCount)
				continue;

			if (m_rTrackedDeviceToRenderModel[index] == NULL)
				setupRenderModelForTrackedDevice(index);
		}
	}

	void VROpenVRRenderModelHandler::draw()
	{
		glUseProgram(m_unRenderModelProgramID);

		for (uint32_t unTrackedDevice = 0; unTrackedDevice < vr::k_unMaxTrackedDeviceCount; unTrackedDevice++)
		{
			if (!m_rTrackedDeviceToRenderModel[unTrackedDevice])
				continue;

			/*const vr::TrackedDevicePose_t & pose = m_rTrackedDevicePose[unTrackedDevice];
			if (!pose.bPoseIsValid)
				continue;

			const Matrix4 & matDeviceToTracking = m_rmat4DevicePose[unTrackedDevice];
			Matrix4 matMVP = GetCurrentViewProjectionMatrix(nEye) * matDeviceToTracking;
			glUniformMatrix4fv(m_nRenderModelMatrixLocation, 1, GL_FALSE, matMVP.get());*/
			glPushMatrix();
			glMultMatrixf(m_inputDevice->getPose(unTrackedDevice).getArray());
			m_rTrackedDeviceToRenderModel[unTrackedDevice]->Draw();
			glPopMatrix();
		}
		glUseProgram(0);
	}

	//-----------------------------------------------------------------------------
	// Purpose: Helper to get a string from a tracked device property and turn it
	//			into a std::string
	//-----------------------------------------------------------------------------
	std::string getTrackedDeviceString(vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError = NULL)
	{
		uint32_t unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, NULL, 0, peError);
		if (unRequiredBufferLen == 0)
			return "";

		char *pchBuffer = new char[unRequiredBufferLen];
		unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, pchBuffer, unRequiredBufferLen, peError);
		std::string sResult = pchBuffer;
		delete[] pchBuffer;
		return sResult;
	}

	//-----------------------------------------------------------------------------
	// Purpose: Finds a render model we've already loaded or loads a new one
	//-----------------------------------------------------------------------------
	VROpenVRRenderModel* VROpenVRRenderModelHandler::findOrLoadRenderModel(const char *pchRenderModelName)
	{
		std::cerr << "Load model: " << pchRenderModelName << std::endl;
		VROpenVRRenderModel *pRenderModel = NULL;
		for (std::vector< VROpenVRRenderModel * >::iterator i = m_vecRenderModels.begin(); i != m_vecRenderModels.end(); i++)
		{
			if (!stricmp((*i)->GetName().c_str(), pchRenderModelName))
			{
				pRenderModel = *i;
				break;
			}
		}

		// load the model if we didn't find one
		if (!pRenderModel)
		{
			vr::RenderModel_t *pModel;
			vr::EVRRenderModelError error;
			while (1)
			{
				error = vr::VRRenderModels()->LoadRenderModel_Async(pchRenderModelName, &pModel);
				if (error != vr::VRRenderModelError_Loading)
					break;

#ifdef _WIN32
				Sleep(1);
#else
				usleep(1000);
#endif
			}

			if (error != vr::VRRenderModelError_None)
			{
				std::cerr << "Unable to load render model: " << pchRenderModelName << " - " << vr::VRRenderModels()->GetRenderModelErrorNameFromEnum(error) << std::endl;
				//dprintf("Unable to load render model %s - %s\n", pchRenderModelName,);
				return NULL; // move on to the next tracked device
			}

			vr::RenderModel_TextureMap_t *pTexture;
			while (1)
			{
				error = vr::VRRenderModels()->LoadTexture_Async(pModel->diffuseTextureId, &pTexture);
				if (error != vr::VRRenderModelError_Loading)
					break;
#ifdef _WIN32
				Sleep(1);
#else
				usleep(1000);
#endif
			}

			if (error != vr::VRRenderModelError_None)
			{
				std::cerr << "Unable to load render texture id:" << pModel->diffuseTextureId << "for render model" << pchRenderModelName << std::endl;
				vr::VRRenderModels()->FreeRenderModel(pModel);
				return NULL; // move on to the next tracked device
			}

			pRenderModel = new VROpenVRRenderModel(pchRenderModelName);
			if (!pRenderModel->BInit(*pModel, *pTexture))
			{
				std::cerr << "Unable to create GL model from render model " << pchRenderModelName << std::endl;
				delete pRenderModel;
				pRenderModel = NULL;
			}
			else
			{
				m_vecRenderModels.push_back(pRenderModel);
				std::cerr << "Load model completed: " << pchRenderModelName << std::endl;
			}
			vr::VRRenderModels()->FreeRenderModel(pModel);
			vr::VRRenderModels()->FreeTexture(pTexture);
		}

		return pRenderModel;
	}

	//-----------------------------------------------------------------------------
	// Purpose: Create/destroy GL a Render Model for a single tracked device
	//-----------------------------------------------------------------------------
	void VROpenVRRenderModelHandler::setupRenderModelForTrackedDevice(vr::TrackedDeviceIndex_t unTrackedDeviceIndex)
	{
		if (unTrackedDeviceIndex >= vr::k_unMaxTrackedDeviceCount)
			return;

		// try to find a model we've already set up
		std::string sRenderModelName = getTrackedDeviceString(m_pHMD, unTrackedDeviceIndex, vr::Prop_RenderModelName_String);
		VROpenVRRenderModel *pRenderModel = findOrLoadRenderModel(sRenderModelName.c_str());
		if (!pRenderModel)
		{
			std::string sTrackingSystemName = getTrackedDeviceString(m_pHMD, unTrackedDeviceIndex, vr::Prop_TrackingSystemName_String);
			std::cerr << "Unable to load render model for tracked device " << unTrackedDeviceIndex << " (" << sTrackingSystemName.c_str() << "," << sRenderModelName.c_str() << ")" << std::endl;
		}
		else
		{
			m_rTrackedDeviceToRenderModel[unTrackedDeviceIndex] = pRenderModel;
		}
	}

} /* namespace MinVR */


