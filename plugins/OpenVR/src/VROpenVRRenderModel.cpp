/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Ben Knorlein
 */



#include "VROpenVRRenderModel.h"




namespace MinVR {
	VROpenVRRenderModel::VROpenVRRenderModel(const std::string & sRenderModelName)
		: m_sModelName(sRenderModelName)
	{
		m_glIndexBuffer = 0;
		m_glVertArray = 0;
		m_glVertBuffer = 0;
		m_glTexture = 0;
	}

	VROpenVRRenderModel::~VROpenVRRenderModel()
	{
		Cleanup();
	}


	//-----------------------------------------------------------------------------
	// Purpose: Allocates and populates the GL resources for a render model
	//-----------------------------------------------------------------------------
	bool VROpenVRRenderModel::BInit(const vr::RenderModel_t & vrModel, const vr::RenderModel_TextureMap_t & vrDiffuseTexture)
	{
		// create and bind a VAO to hold state for this model
		glGenVertexArrays(1, &m_glVertArray);
		glBindVertexArray(m_glVertArray);

		// Populate a vertex buffer
		glGenBuffers(1, &m_glVertBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_glVertBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vr::RenderModel_Vertex_t) * vrModel.unVertexCount, vrModel.rVertexData, GL_STATIC_DRAW);

		// Identify the components in the vertex buffer
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vr::RenderModel_Vertex_t), (void *)offsetof(vr::RenderModel_Vertex_t, vPosition));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vr::RenderModel_Vertex_t), (void *)offsetof(vr::RenderModel_Vertex_t, vNormal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vr::RenderModel_Vertex_t), (void *)offsetof(vr::RenderModel_Vertex_t, rfTextureCoord));

		// Create and populate the index buffer
		glGenBuffers(1, &m_glIndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glIndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t)* vrModel.unTriangleCount * 3, vrModel.rIndexData, GL_STATIC_DRAW);

		glBindVertexArray(0);

		// create and populate the texture
		glGenTextures(1, &m_glTexture);
		glBindTexture(GL_TEXTURE_2D, m_glTexture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, vrDiffuseTexture.unWidth, vrDiffuseTexture.unHeight,
			0, GL_RGBA, GL_UNSIGNED_BYTE, vrDiffuseTexture.rubTextureMapData);

		// If this renders black ask McJohn what's wrong.
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		GLfloat fLargest;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);

		glBindTexture(GL_TEXTURE_2D, 0);

		m_unVertexCount = vrModel.unTriangleCount * 3;

		return true;
	}


	//-----------------------------------------------------------------------------
	// Purpose: Frees the GL resources for a render model
	//-----------------------------------------------------------------------------
	void VROpenVRRenderModel::Cleanup()
	{
		if (m_glVertBuffer)
		{
			glDeleteBuffers(1, &m_glIndexBuffer);
			glDeleteVertexArrays(1, &m_glVertArray);
			glDeleteBuffers(1, &m_glVertBuffer);
			m_glIndexBuffer = 0;
			m_glVertArray = 0;
			m_glVertBuffer = 0;
		}
	}


	//-----------------------------------------------------------------------------
	// Purpose: Draws the render model
	//-----------------------------------------------------------------------------
	void VROpenVRRenderModel::Draw()
	{
		glBindVertexArray(m_glVertArray);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_glTexture);

		glDrawElements(GL_TRIANGLES, m_unVertexCount, GL_UNSIGNED_SHORT, 0);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

} /* namespace MinVR */


