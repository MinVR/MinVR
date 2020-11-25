/**
 * HoloPlayContext.cpp
 * Contributors:
 *      * Arthur Sonzogni (author), Looking Glass Factory Inc.
 *		* Benjamin Knorlein (modifications to use for MinVR), Brown University
 * Licence:
 *      * MIT
 */

#include "HoloPlayContext.hpp"
#include "HoloPlayShaders.h"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <iostream>
#include <stdexcept>

#include "Shader.h"
#include "glError.hpp"

using namespace std;

HoloPlayContext *currentApplication = NULL;

HoloPlayContext::HoloPlayContext(float camera_Size, float fov_angle, int devIndex, int quiver_quality, float nearPlane, float farPlane)
	:m_cameraSize{ camera_Size }, m_fov_angle{ fov_angle }, m_devIndex{ devIndex }, m_qs_quality{ quiver_quality }, m_nearPlane{ nearPlane }, m_farPlane{farPlane}
{
  m_projectionMatrix = glm::mat4(1.0);
  m_viewMatrix = glm::mat4(1.0);

  currentApplication = this;

  // get device info via holoplay core
  if (!GetLookingGlassInfo())
  {
    std::cout << "[Info] HoloplayCore Message Pipe tear down" << std::endl;
    // must tear down the message pipe before shut down the app
    hpc_TeardownMessagePipe();
    throw std::runtime_error("Couldn't find looking glass");
  }
  // get the viewcone here, which is used as a const
  m_viewCone = hpc_GetDevicePropertyFloat(m_devIndex, "/calibration/viewCone/value");

  // get the window size / coordinates
  int win_w = hpc_GetDevicePropertyScreenW(m_devIndex);
  int win_h = hpc_GetDevicePropertyScreenH(m_devIndex);
  int win_x = hpc_GetDevicePropertyWinX(m_devIndex);
  int win_y = hpc_GetDevicePropertyWinY(m_devIndex);

  std::cout << "Looking glass window opened at (" << win_x << ", " << win_y << "), size: ("
	  << win_w << ", " << win_h << "). Set Values in the minvr config-file accordingly" << std::endl;

  m_aspectRatio = (float) win_w / (float)win_h;

  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  glCheckError(__FILE__, __LINE__);

  if (err != GLEW_OK)
  {
    std::cout << "terminiated" << std::endl;
    throw std::runtime_error(string("Could initialize GLEW, error = ") +
                             (const char *)glewGetErrorString(err));
  }

  // get OpenGL version info
  const GLubyte *renderer = glGetString(GL_RENDERER);
  const GLubyte *version = glGetString(GL_VERSION);
  std::cout << "Renderer: " << renderer << std::endl;
  std::cout << "[Info] OpenGL version supported " << version << std::endl;

  int opengl_version_major;
  int opengl_version_minor;

  glGetIntegerv(GL_MAJOR_VERSION, &opengl_version_major);
  glGetIntegerv(GL_MINOR_VERSION, &opengl_version_minor);

  opengl_version_header = "#version ";
  opengl_version_header += to_string(opengl_version_major);
  opengl_version_header += to_string(opengl_version_minor);
  opengl_version_header += "0 core\n";

  // opengl configuration
  glEnable(GL_DEPTH_TEST); // enable depth-testing
  glDepthFunc(GL_LESS);    // depth-testing interprets a smaller value as "closer"

  // initialize the holoplay context
  initialize();
}

HoloPlayContext::~HoloPlayContext() {
	release();
	exit();
}

void HoloPlayContext::exit()
{
  std::cout << "[Info] Informing Holoplay Core to close app" << std::endl;
  hpc_CloseApp();
  // release all the objects created for setting up the HoloPlay Context
  release();
}

// Holoplay Core related functions
// =========================================================
// Register and initialize the app through HoloPlay Core
// And print information about connected Looking Glass devices
bool HoloPlayContext::GetLookingGlassInfo()
{
  hpc_client_error errco =
      hpc_InitializeApp("Holoplay Core Example App", hpc_LICENSE_NONCOMMERCIAL);
  if (errco)
  {
    string errstr;
    switch (errco)
    {
    case hpc_CLIERR_NOSERVICE:
      errstr = "HoloPlay Service not running";
      break;
    case hpc_CLIERR_SERIALIZEERR:
      errstr = "Client message could not be serialized";
      break;
    case hpc_CLIERR_VERSIONERR:
      errstr = "Incompatible version of HoloPlay Service";
      break;
    case hpc_CLIERR_PIPEERROR:
      errstr = "Interprocess pipe broken";
      break;
    case hpc_CLIERR_SENDTIMEOUT:
      errstr = "Interprocess pipe send timeout";
      break;
    case hpc_CLIERR_RECVTIMEOUT:
      errstr = "Interprocess pipe receive timeout";
      break;
    default:
      errstr = "Unknown error";
      break;
    }
    std::cout << "HoloPlay Service access error (code " << errco << "): " << errstr
         << "!" << std::endl;
    return false;
  }
  char buf[1000];
  hpc_GetHoloPlayCoreVersion(buf, 1000);
  std::cout << "HoloPlay Core version " << buf << "." << std::endl;
  hpc_GetHoloPlayServiceVersion(buf, 1000);
  std::cout << "HoloPlay Service version " << buf << "." << std::endl;
  int num_displays = hpc_GetNumDevices();
  std::cout << num_displays << " devices connected." << std::endl;
  if (num_displays < 1)
  {
    return false;
  }
  for (int i = 0; i < num_displays; ++i)
  {
    std::cout << "Device information for display " << i << ":" << std::endl;
    hpc_GetDeviceHDMIName(i, buf, 1000);
    std::cout << "\tDevice name: " << buf << std::endl;
    hpc_GetDeviceType(i, buf, 1000);
    std::cout << "\tDevice type: " << buf << std::endl;
    hpc_GetDeviceType(i, buf, 1000);
    std::cout << "\nWindow parameters for display " << i << ":" << std::endl;
    std::cout << "\tPosition: (" << hpc_GetDevicePropertyWinX(i) << ", "
         << hpc_GetDevicePropertyWinY(i) << ")" << std::endl;
    std::cout << "\tSize: (" << hpc_GetDevicePropertyScreenW(i) << ", "
         << hpc_GetDevicePropertyScreenH(i) << ")" << std::endl;
    std::cout << "\tAspect ratio: " << hpc_GetDevicePropertyDisplayAspect(i) << std::endl;
    std::cout << "\nShader uniforms for display " << i << ":" << std::endl;
    std::cout << "\tPitch: " << hpc_GetDevicePropertyPitch(i) << std::endl;
    std::cout << "\tTilt: " << hpc_GetDevicePropertyTilt(i) << std::endl;
    std::cout << "\tCenter: " << hpc_GetDevicePropertyCenter(i) << std::endl;
    std::cout << "\tSubpixel width: " << hpc_GetDevicePropertySubp(i) << std::endl;
    std::cout << "\tView cone: "
         << hpc_GetDevicePropertyFloat(i, "/calibration/viewCone/value")
         << std::endl;
    std::cout << "\tFringe: " << hpc_GetDevicePropertyFringe(i) << std::endl;
    std::cout << "\tRI: " << hpc_GetDevicePropertyRi(i)
         << "\n\tBI: " << hpc_GetDevicePropertyBi(i)
         << "\n\tinvView: " << hpc_GetDevicePropertyInvView(i) << std::endl;
  }

  return true;
}

// setup fuctions
// =========================================================
void HoloPlayContext::initialize()
{
  std::cout << "[Info] initializing" << std::endl;

  loadBlitShaders();
  glCheckError(__FILE__, __LINE__);

  loadLightFieldShaders();
  glCheckError(__FILE__, __LINE__);

  loadCalibrationIntoShader();
  glCheckError(__FILE__, __LINE__);

  setupQuiltSettings(m_qs_quality);
  passQuiltSettingsToShader();
  glCheckError(__FILE__, __LINE__);

  setupQuilt();
  glCheckError(__FILE__, __LINE__);
}

// set up the quilt settings
void HoloPlayContext::setupQuiltSettings(int preset)
{
  // there are 3 presets:
  switch (preset)
  {
  case 0: // standard
    m_qs_width = 2048;
    m_qs_height = 2048;
    m_qs_columns = 4;
    m_qs_rows = 8;
    m_qs_totalViews = 32;
    break;
  default:
  case 1: // hires
    m_qs_width = 4096;
    m_qs_height = 4096;
    m_qs_columns = 5;
    m_qs_rows = 9;
    m_qs_totalViews = 45;
    break;
  case 2: // 8k
    m_qs_width = 4096 * 2;
    m_qs_height = 4096 * 2;
    m_qs_columns = 5;
    m_qs_rows = 9;
    m_qs_totalViews = 45;
    break;
  }
}
// pass quilt values to shader
void HoloPlayContext::passQuiltSettingsToShader()
{
  m_lightFieldShader->bindProgram();

  glUniform1i(glGetUniformLocation(m_lightFieldShader->getProgram(), "overscan"), 0);
  glCheckError(__FILE__, __LINE__);

  glUniform3f(glGetUniformLocation(m_lightFieldShader->getProgram(), "tile")
											, m_qs_columns, m_qs_rows, m_qs_totalViews);
  glCheckError(__FILE__, __LINE__);

  int m_qs_viewWidth = m_qs_width / m_qs_columns;
  int m_qs_viewHeight = m_qs_height / m_qs_rows;

  glUniform2f(glGetUniformLocation(m_lightFieldShader->getProgram(), "viewPortion")
										, m_qs_viewWidth * m_qs_columns / (float)m_qs_width
										, m_qs_viewHeight * m_qs_rows / (float)m_qs_height);
  glCheckError(__FILE__, __LINE__);

  m_lightFieldShader->unbindProgram();
}

void HoloPlayContext::setupQuilt()
{
  std::cout << "setting up quilt texture and framebuffer" << std::endl;
  glGenTextures(1, &m_quiltTexture);
  glBindTexture(GL_TEXTURE_2D, m_quiltTexture);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_qs_width, m_qs_height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, NULL);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindTexture(GL_TEXTURE_2D, 0);

  // framebuffer
  glGenFramebuffers(1, &m_FBO);
  glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

  // bind the quilt texture as the color attachment of the framebuffer
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         m_quiltTexture, 0);

  // vbo and vao
  glGenVertexArrays(1, &m_VAO);
  glGenBuffers(1, &m_VBO);

  // set up the vertex array object
  glBindVertexArray(m_VAO);

  // fullscreen quad vertices
  const float fsquadVerts[] = {
      -1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      -1.0f,
  };

  // create vbo
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(fsquadVerts), fsquadVerts,
               GL_STATIC_DRAW);

  // setup the attribute pointers
  // note: using only 2 floats per vert, not 3
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // unbind stuff
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void HoloPlayContext::loadBlitShaders()
{
  std::cout << "loading blit shaders" << std::endl;
  string blitFragmentShaderSource = R"--(
  in vec2 texCoords;
  out vec4 fragColor;
  uniform sampler2D blitTex;
  void main()
  {
      fragColor = texture(blitTex, texCoords.xy);
  }
  )--";
  string blitVertexShaderSource = R"--(
  layout (location = 0)
  in vec2 vertPos_data;
  out vec2 texCoords;
  void main()
  {
      gl_Position = vec4(vertPos_data.xy, 0.0, 1.0);
      texCoords = (vertPos_data.xy + 1.0) * 0.5;
  }
  )--";

  m_blitShader = new Shader(opengl_version_header + blitVertexShaderSource , opengl_version_header + blitFragmentShaderSource, "BlitShader");
}

void HoloPlayContext::loadLightFieldShaders()
{
  std::cout << "loading quilt shader" << std::endl;
  m_lightFieldShader = new Shader(opengl_version_header + hpc_LightfieldVertShaderGLSL, opengl_version_header + hpc_LightfieldFragShaderGLSL, "QuiltShader");
}

void HoloPlayContext::loadCalibrationIntoShader()
{
  std::cout << "begin assigning calibration uniforms" << std::endl;
  m_lightFieldShader->bindProgram();
  glCheckError(__FILE__, __LINE__);

  glUniform1f(glGetUniformLocation(m_lightFieldShader->getProgram(), "pitch"), hpc_GetDevicePropertyPitch(m_devIndex));
  glCheckError(__FILE__, __LINE__);

  glUniform1f(glGetUniformLocation(m_lightFieldShader->getProgram(), "tilt"), hpc_GetDevicePropertyTilt(m_devIndex));
  glCheckError(__FILE__, __LINE__);

  glUniform1f(glGetUniformLocation(m_lightFieldShader->getProgram(), "center"), hpc_GetDevicePropertyCenter(m_devIndex));
  glCheckError(__FILE__, __LINE__);

  glUniform1i(glGetUniformLocation(m_lightFieldShader->getProgram(), "invView"), hpc_GetDevicePropertyInvView(m_devIndex));
  glCheckError(__FILE__, __LINE__);

  glUniform1i(glGetUniformLocation(m_lightFieldShader->getProgram(), "quiltInvert"), 0);
  glCheckError(__FILE__, __LINE__);

  glUniform1f(glGetUniformLocation(m_lightFieldShader->getProgram(), "subp"), hpc_GetDevicePropertySubp(m_devIndex));
  glCheckError(__FILE__, __LINE__);

  glUniform1i(glGetUniformLocation(m_lightFieldShader->getProgram(), "ri"), hpc_GetDevicePropertyRi(m_devIndex));
  glCheckError(__FILE__, __LINE__);

  glUniform1i(glGetUniformLocation(m_lightFieldShader->getProgram(), "bi"), hpc_GetDevicePropertyBi(m_devIndex));
  glCheckError(__FILE__, __LINE__);

  glUniform1f(glGetUniformLocation(m_lightFieldShader->getProgram(), "displayAspect"), hpc_GetDevicePropertyDisplayAspect(m_devIndex));
  glCheckError(__FILE__, __LINE__);
  
  glUniform1f(glGetUniformLocation(m_lightFieldShader->getProgram(), "quiltAspect"), hpc_GetDevicePropertyDisplayAspect(m_devIndex));
  glCheckError(__FILE__, __LINE__);

  m_lightFieldShader->unbindProgram();
  glCheckError(__FILE__, __LINE__);
}

void HoloPlayContext::setupViewTextureAndFrameBuffer(
    unsigned int &viewTexture,
    unsigned int &framebuffer,
    unsigned int &textureColorbuffer,
    unsigned int &rbo,
	unsigned int win_w,
	unsigned int win_h)
{
  glGenTextures(1, &viewTexture);
  glBindTexture(GL_TEXTURE_2D, viewTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, win_w, win_h, 0, GL_RGB,
               GL_UNSIGNED_BYTE, NULL);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, win_w, win_h, 0,
               GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
  glCheckError(__FILE__, __LINE__);

  // generate framebuffer
  glGenFramebuffers(1, &framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

  // create a color attachment texture
  glGenTextures(1, &textureColorbuffer);
  glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, win_w, win_h, 0, GL_RGB,
               GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  // attach it to currently bound framebuffer object
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         textureColorbuffer, 0);

  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, win_w,
                        win_h); // use a single renderbuffer object for both a
                                // depth AND stencil buffer.
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  // attach the renderbuffer object to the depth and stencil attachment of the
  // framebuffer
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, rbo); // now actually attach it

  // now that we actually created the framebuffer and added all attachments we
  // want to check if it is actually complete 
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "[ERROR] Framebuffer is not complete!" << std::endl;
  }
  else
  {
    // attach texture to the framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                           GL_TEXTURE_2D, viewTexture, 0);
    glCheckError(__FILE__, __LINE__);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// release function
// =========================================================
void HoloPlayContext::release()
{
  std::cout << "[Info] HoloPlay Context releasing" << std::endl;
  glDeleteVertexArrays(1, &m_VAO);
  glDeleteBuffers(1, &m_VBO);
  glDeleteFramebuffers(1, &m_FBO);
  glDeleteTextures(1, &m_quiltTexture);
  delete m_lightFieldShader;
  delete m_blitShader;
}

// render functions
// =========================================================
// set up the camera for each view and the shader of the rendering object
void HoloPlayContext::setupVirtualCameraForView(int currentViewIndex,
                                                glm::mat4 currentViewMatrix)
{
	// The standard model Looking Glass screen is roughly 4.75" vertically. If we
	// assume the average viewing distance for a user sitting at their desk is
	// about 36", our field of view should be about 14°. There is no correct
	// answer, as it all depends on your expected user's distance from the Looking
	// Glass, but we've found the most success using this figure.
	const float fov = glm::radians(m_fov_angle);
	float cameraDistance = -m_cameraSize / tan(fov / 2.0f);

	float offsetAngle =
		(currentViewIndex / (m_qs_totalViews - 1.0f) - 0.5f) *
		glm::radians(m_viewCone); // start at -viewCone * 0.5 and go up to viewCone * 0.5

	float offset =
		cameraDistance *
		tan(offsetAngle); // calculate the offset that the camera should move

	// modify the view matrix (position)
	// determine the local direction of the offset using currentViewMatrix and translate
	glm::vec3 offsetLocal = glm::vec3(currentViewMatrix * glm::vec4(offset, 0.0f, cameraDistance, 1.0f));
	m_viewMatrix = glm::translate(currentViewMatrix, offsetLocal);

	m_projectionMatrix = glm::perspective(fov, m_aspectRatio, m_nearPlane, m_farPlane);
	// modify the projection matrix, relative to the camera size and aspect ratio
	m_projectionMatrix[2][0] += offset / (m_cameraSize * m_aspectRatio);
}

// copy view to quilt
void HoloPlayContext::copyViewToQuilt(int view)
{
  // framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

  // save the viewport
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  int m_qs_viewWidth = m_qs_width / m_qs_columns;
  int m_qs_viewHeight = m_qs_height / m_qs_rows;
  int x = (view % m_qs_columns) * m_qs_viewWidth;
  int y = (view / m_qs_columns) * m_qs_viewHeight;

  glViewport(x, y, m_qs_viewWidth, m_qs_viewHeight);
  glScissor(x, y, m_qs_viewWidth, m_qs_viewHeight);

  // vao
  glBindVertexArray(m_VAO);

  // use the shader and draw
  m_blitShader->bindProgram();
  glDrawArrays(GL_TRIANGLES, 0, 6);

  // reset framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // reset viewport
  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
  glScissor(viewport[0], viewport[1], viewport[2], viewport[3]);
  glBindVertexArray(0);
  m_blitShader->unbindProgram();
}

void HoloPlayContext::drawLightField()
{
  // bind quilt texture
  glBindTexture(GL_TEXTURE_2D, m_quiltTexture);

  // bind vao
  glBindVertexArray(m_VAO);

  // use the shader and draw
  m_lightFieldShader->bindProgram();
  glDrawArrays(GL_TRIANGLES, 0, 6);

  // clean up
  glBindVertexArray(0);
  m_lightFieldShader->unbindProgram();
}
