#include "openGLStatus.h"

/**
 * @title glErrorCheck
 * @description check OpenGL Status
 */
inline bool glErrorCheck()
{
	GLenum error_code(glGetError());

	if (error_code != GL_NO_ERROR)
	{
		std::cerr << "OpenGL error: " << (char*)gluErrorString(error_code) << std::endl;
		return true;
	}

	return false;
}
