#include <d3d/rendering/SFMLOGL.h>

#include <iostream>

using namespace d3d;

GLuint d3d::_vaoID = 0;

bool d3d::checkForGLError() {
	GLuint errorCode = glGetError();

	if (errorCode != GL_NO_ERROR) {
		std::cerr << gluErrorString(errorCode) << std::endl;
		abort();

		return true;
	}

	return false;
}

void d3d::sfmloglSetup() {
	glGenVertexArrays(1, &_vaoID);
	glBindVertexArray(_vaoID);

	glEnableVertexAttribArray(D3D_ATTRIB_POSITION);
	glEnableVertexAttribArray(D3D_ATTRIB_NORMAL);
	glEnableVertexAttribArray(D3D_ATTRIB_TEXCOORD);

	glFrontFace(GL_CCW);

	glEnable(GL_CULL_FACE);

	glClearDepth(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
}