/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "MyAppView.h"

MyAppView::MyAppView(const MyAppModel& model, const MyAppSharedContext& sharedContext, const VRGraphicsState &renderState) : model(model), sharedContext(sharedContext) {
	// Init GL
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0, 0, 0, 1);

	// Create vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, sharedContext.getVbo());
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (char*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (char*)0 + sizeof(GLfloat)*model.vertices.size());
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (char*)0 + sizeof(GLfloat)*model.vertices.size() + sizeof(GLfloat)*model.normals.size());

	// Create shader
	std::string vertexShader =
		"#version 330 \n"
		"layout(location = 0) in vec3 position; "
		"layout(location = 1) in vec3 normal; "
		"layout(location = 2) in vec3 color; "
		""
		"uniform mat4 ProjectionMatrix; "
		"uniform mat4 ViewMatrix; "
		"uniform mat4 ModelMatrix; "
		"uniform mat4 NormalMatrix; "
		""
		"out vec3 col;"
		""
		"void main() { "
		"	gl_Position = ProjectionMatrix*ViewMatrix*ModelMatrix*vec4(position, 1.0); "
		"	col = color;"
		"}";
	vshader = compileShader(vertexShader, GL_VERTEX_SHADER);

	std::string fragmentShader =
		"#version 330 \n"
		"in vec3 col;"
		"out vec4 colorOut;"
		""
		"void main() { "
		"	colorOut = vec4(col, 1.0); "
		"}";
	fshader = compileShader(fragmentShader, GL_FRAGMENT_SHADER);

	// Create shader program
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vshader);
	glAttachShader(shaderProgram, fshader);
	linkShaderProgram(shaderProgram);
}

MyAppView::~MyAppView() {
	// Delete graphics objects
	glDeleteVertexArrays(1, &vao);
	glDetachShader(shaderProgram, vshader);
	glDetachShader(shaderProgram, fshader);
	glDeleteShader(vshader);
	glDeleteShader(fshader);
	glDeleteProgram(shaderProgram);
}

void MyAppView::update(const VRGraphicsState &renderState) {
	// Update window specific objects
}

/// onVRRenderScene will run draw calls on each viewport inside a context.
void MyAppView::render(const VRGraphicsState &renderState) {
	// Draw the scene
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Set shader parameters
	glUseProgram(shaderProgram);
	GLint loc = glGetUniformLocation(shaderProgram, "ProjectionMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, renderState.getProjectionMatrix());
	loc = glGetUniformLocation(shaderProgram, "ViewMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, renderState.getViewMatrix());
	loc = glGetUniformLocation(shaderProgram, "ModelMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, model.modelMatrix);

	// Draw cube
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	// reset program
	glUseProgram(0);
}

/// Compiles shader
GLuint MyAppView::compileShader(const std::string& shaderText, GLuint shaderType) {
	const char* source = shaderText.c_str();
	int length = (int)shaderText.size();
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &source, &length);
	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> log(length);
		glGetShaderInfoLog(shader, length, &length, &log[0]);
		std::cerr << &log[0];
	}

	return shader;
}

/// links shader program
void MyAppView::linkShaderProgram(GLuint shaderProgram) {
	glLinkProgram(shaderProgram);
	GLint status;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint length;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> log(length);
		glGetProgramInfoLog(shaderProgram, length, &length, &log[0]);
		std::cerr << "Error compiling program: " << &log[0] << std::endl;
	}
}
