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

#include <string.h>

namespace MinVR {
	VROpenVRRenderModelHandler::VROpenVRRenderModelHandler(vr::IVRSystem *pHMD, VROpenVRInputDevice* inputDevice, bool hide_tracker) : m_pHMD(pHMD), m_inputDevice(inputDevice), m_unRenderModelProgramID(0), m_nRenderModelMatrixLocation(-1), m_nRenderModelState(-1), m_hide_tracker(hide_tracker)
	{
		memset(m_rTrackedDeviceToRenderModel, 0, sizeof(m_rTrackedDeviceToRenderModel));
		memset(hasComponent, 0, sizeof(hasComponent));

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
				"#version 410\n"
				"uniform mat4 matrix;\n"
				"layout(location = 0) in vec4 position;\n"
				"layout(location = 1) in vec3 v3NormalIn;\n"
				"layout(location = 2) in vec2 v2TexCoordsIn;\n"
				"out vec2 v2TexCoord;\n"
				"void main()\n"
				"{\n"
				"	v2TexCoord = v2TexCoordsIn;\n"
				"	gl_Position = matrix * vec4(position.xyz, 1);\n"
				"}\n",

				//fragment shader
				"#version 410 core\n"
				"uniform sampler2D diffuse;\n"
				"uniform uint state;\n"
				"in vec2 v2TexCoord;\n"
				"out vec4 outputColor;\n"
				"void main()\n"
				"{\n"
				"   outputColor = texture( diffuse, v2TexCoord);\n"
				"	if (state == 2) outputColor.r = outputColor.r + 0.3; "
				"   else if (state == 1) outputColor.b = outputColor.b + 0.3;"
				"}\n"

				);

			m_nRenderModelMatrixLocation = glGetUniformLocation(m_unRenderModelProgramID, "matrix");
			if (m_nRenderModelMatrixLocation == -1)
			{
				std::cerr << "Unable to find matrix uniform in render model shader" << std:: endl;
				exit(-1);
			}

			m_nRenderModelState = glGetUniformLocation(m_unRenderModelProgramID, "state");
			if (m_nRenderModelState == -1)
			{
				std::cerr << "Unable to find state uniform in render model shader" << std::endl;
				exit(-1);
			}
		}

		while (!modelLoaderQueue.empty())
		{
			vr::TrackedDeviceIndex_t index = modelLoaderQueue.front();
			modelLoaderQueue.pop();

			if (index >= vr::k_unMaxTrackedDeviceCount)
				continue;

			if (m_rTrackedDeviceToRenderModel[index] == NULL && !hasComponent[index])
				setupRenderModelForTrackedDevice(index);
		}
	}

	void VROpenVRRenderModelHandler::draw(VRMatrix4 projection, VRMatrix4 modelview)
	{
		glUseProgram(m_unRenderModelProgramID);

		for (uint32_t unTrackedDevice = 0; unTrackedDevice < vr::k_unMaxTrackedDeviceCount; unTrackedDevice++)
		{
			if (m_pHMD->GetTrackedDeviceClass(unTrackedDevice) != vr::TrackedDeviceClass_Controller
				&& m_hide_tracker)
				continue;

			if (!hasComponent[unTrackedDevice]){
				if (!m_rTrackedDeviceToRenderModel[unTrackedDevice])
					continue;

				VRMatrix4 MVP = projection * modelview * m_inputDevice->getPose(unTrackedDevice);
				glUniformMatrix4fv(m_nRenderModelMatrixLocation, 1, GL_FALSE, MVP.getArray());
				glUniform1ui(m_nRenderModelState, 0);

				m_rTrackedDeviceToRenderModel[unTrackedDevice]->Draw();
			}
			else
			{
				for (int i = 0; i < m_rTrackedDeviceToRenderModelComponents[unTrackedDevice].size(); i++)
				{
					vr::VRControllerState_t c_state;
					if (m_pHMD->GetControllerState(unTrackedDevice, &c_state, sizeof(c_state))){
						vr::RenderModel_ControllerMode_State_t statemode;
						statemode.bScrollWheelVisible = false;
						vr::RenderModel_ComponentState_t component_state;
						
						if (vr::VRRenderModels()->GetComponentState(m_rDeviceName[unTrackedDevice].c_str(), m_rComponentName[unTrackedDevice][i].c_str(), &c_state, &statemode, &component_state))
						{
							//std::cerr << "Working " << std::endl;// << component_state->mTrackingToComponentLocal.m << std::endl;
							if (component_state.uProperties & vr::VRComponentProperty_IsVisible){
								VRMatrix4 MVP = projection * modelview * m_inputDevice->getPose(unTrackedDevice) * m_inputDevice->poseToMatrix4(component_state.mTrackingToComponentRenderModel.m);
								glUniformMatrix4fv(m_nRenderModelMatrixLocation, 1, GL_FALSE, MVP.getArray());
								unsigned int state = (component_state.uProperties & vr::VRComponentProperty_IsPressed) ? 2 : (component_state.uProperties & vr::VRComponentProperty_IsTouched) ? 1 : 0;
								glUniform1ui(m_nRenderModelState, state);
								m_rTrackedDeviceToRenderModelComponents[unTrackedDevice][i]->Draw();
							}
						}
					}
				}

			}
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
		std::cerr << "Find Or Load model: " << pchRenderModelName << std::endl;
		VROpenVRRenderModel *pRenderModel = NULL;
		for (std::vector< VROpenVRRenderModel * >::iterator i = m_vecRenderModels.begin(); i != m_vecRenderModels.end(); i++)
		{
#ifdef WIN32
			if (!_stricmp((*i)->GetName().c_str(), pchRenderModelName))
#else
                if (!strcasecmp((*i)->GetName().c_str(), pchRenderModelName))
#endif
			{
				pRenderModel = *i;
				break;
			}
		}

		// load the model if we didn't find one
		if (!pRenderModel)
		{
			std::cerr << "Load model: " << pchRenderModelName << std::endl;
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
		unsigned int count = vr::VRRenderModels()->GetComponentCount(sRenderModelName.c_str());

		if (count == 0){
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
		else
		{
			hasComponent[unTrackedDeviceIndex] = true;
			for (unsigned int i = 0; i < count; i++)
			{
				char *pchBuffer = new char[10000];
				unsigned int  name_length = 10000;
				if (vr::VRRenderModels()->GetComponentName(sRenderModelName.c_str(), i, pchBuffer, name_length) > 0){
					std::string componentName = pchBuffer;
					
					if (vr::VRRenderModels()->GetComponentRenderModelName(sRenderModelName.c_str(), componentName.c_str(), pchBuffer, name_length) > 0){
						std::string componentRenderModelName = pchBuffer;
						VROpenVRRenderModel* component = findOrLoadRenderModel(componentRenderModelName.c_str());
						if (!component)
						{
							std::cerr << "Unable to load render model for tracked device " << sRenderModelName.c_str() << " " << unTrackedDeviceIndex << " Component " << componentName.c_str() << std::endl;
						}
						else
						{
							m_rTrackedDeviceToRenderModelComponents[unTrackedDeviceIndex].push_back(component);
							m_rComponentName[unTrackedDeviceIndex].push_back(componentName);
							m_rDeviceName[unTrackedDeviceIndex] = sRenderModelName;
						}
					}
				}
				delete[] pchBuffer;
			}
		}
	}

} /* namespace MinVR */


