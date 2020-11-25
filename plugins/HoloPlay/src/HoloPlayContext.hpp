/**
 * HoloPlayContext.hpp
 * Contributors:
 *      * Arthur Sonzogni (author), Looking Glass Factory Inc.
 *		* Benjamin Knorlein (modifications to use for MinVR), Brown University
 * Licence:
 *      * MIT
 */

#ifndef OPENGL_CMAKE_SKELETON_APPLICATION_HPP
#define OPENGL_CMAKE_SKELETON_APPLICATION_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <string>
#include "HoloPlayCore.h"
#include "Shader.h"

struct hpc_Uniforms_t;

class HoloPlayContext
{
public:
    HoloPlayContext(float camera_Size, float fov_angle, int devIndex, int quiver_quality, float nearPlane, float farPlane );

	~HoloPlayContext();

    // exit function
    void exit();

	glm::mat4 GetProjectionMatrixOfCurrentView() { return m_projectionMatrix; }
	glm::mat4 GetViewMatrixOfCurrentView() { return m_viewMatrix; }

	unsigned int getNumberViews() {
		return m_qs_totalViews;
	}
	// create view texture, framebuffer and texture color buffer, render buffer
	// object and will output the bound handlers
	void setupViewTextureAndFrameBuffer(unsigned int &viewTexture,
		unsigned int &framebuffer,
		unsigned int &textureColorbuffer,
		unsigned int &rbo,
		unsigned int win_w,
		unsigned int win_h);


	// render functions
	void setupVirtualCameraForView( // Changes the view matrix and projection
		int currentViewIndex,       // accoriding to the view index and the
									// currentViewMatrix
		glm::mat4 currentViewMatrix);

	void drawLightField();          // Uses the lightfieldShader program,
									// binds the quiltTexture, and draws a fullscreen
									// quad. Call this after all the views have been
									// copied into quiltTexture using copyViewIntoQuilt().
	void copyViewToQuilt(int view); // Copies the currently bound glTexture to
									// the specified view on the quilt

private:
    // storing matrix of each view
    glm::mat4 m_projectionMatrix;
    glm::mat4 m_viewMatrix;

	int m_qs_quality;			// 0: 32 views
								// 1: 45 views, normally used one
								// 2: 45 views for 8k display
	const int m_devIndex;		// the index of device we are rendering on,
								// default is 0, the first Looking Glass detected
	
	float m_cameraSize;			// size of the holoplay camera,
	
	float m_fov_angle; 

	float m_nearPlane;

	float m_farPlane;

	float m_aspectRatio;		//set by using the window width and height of the device

	float m_viewCone ;			// view cone of hardware, always around 40 - loaded from device


	std::string opengl_version_header;

    // quilt settings
    // more info at
    // https://docs.lookingglassfactory.com/HoloPlayCAPI/guides/quilt/
    int m_qs_width;      // Total width of the quilt texture
    int m_qs_height;     // Total height of the quilt texture
    int m_qs_rows;       // Number of columns in the quilt
    int m_qs_columns;    // Number of rows in the quilt
    int m_qs_totalViews; // The total number of views in the quilt.
                       // Note that this number might be lower than rows *
                       // columns
    // qs_viewWidth & qs_viewHeight could be calculated by given numbers

    // shaders:
    Shader *m_lightFieldShader =
        NULL; // The shader program for drawing light field images to the Looking
              // Glass
    Shader *m_blitShader =
        NULL; // The shader program for copying views to the quilt

    // render var
    unsigned int m_quiltTexture; // The texture object used internally to draw quilt,
                      // It is bound and drawn by drawLightfield()
    unsigned int m_VAO; // The vertex array object used internally to blit to the
                      // quilt and screen
    unsigned int m_VBO; // The vertex buffer object used internally to blit to the
                      // quilt and screen
    unsigned int m_FBO; // The frame buffer object used internally to blit views to
                      // the quilt

    // example implementation for rendering 45 views
    // ====================================================================================
    // set up functions
    void initialize(); // calls all the functions necessary to set up the
                       // HoloPlay Context
    void setupQuilt(); // create the quiltTexture, VBO, VAO, and FBO
    void setupQuiltSettings(int preset);	// Set up the quilt settings according to the preset passed
											// 0: 32 views
											// 1: 45 views, normally used one
											// 2: 45 views for 8k display
											// Feel free to customize if you want
    void passQuiltSettingsToShader(); // assign quilt settings to light-field
                                      // shader uniforms
    void loadCalibrationIntoShader(); // assign calibration to light-field shader
                                      // uniforms
    void loadLightFieldShaders();     // create and compile light-field shader
    void loadBlitShaders();           // create and comiple blit shader

    // release function
    void release(); // Destroys / releases all buffers and objects creating
                    // during initialize()

    // holoplay core related helper functions
    bool
    GetLookingGlassInfo(); // get all the information of all connected looking
                           // glass retuyrn false if no looking glass detected
    // some get functions
    unsigned int getQuiltTexture() { return m_quiltTexture; }
    unsigned int getLightfieldShader() { return m_lightFieldShader->getProgram(); }
};

#endif /* end of include guard: OPENGL_CMAKE_SKELETON_APPLICATION_HPP */
