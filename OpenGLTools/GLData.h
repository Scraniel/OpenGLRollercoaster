/*
 * GLData.h
 *
 *  Created on: Sep 25, 2015
 *      Author: dglewis
 */

#ifndef OPENGLTOOLS_GLDATA_H_
#define OPENGLTOOLS_GLDATA_H_

#include <GL/glew.h> // include BEFORE glut
#include <GL/glut.h>
#include "../MathTools/Mat4f.h"
#include "ShaderTools.h"

// Houses and initializes all data needed for OpenGL and glut.
// Used to take out ugly OpenGL code from main.
class GLData {
public:
	GLData();
	virtual ~GLData();

	// Will most likely move to private, just for incremental testing
	GLuint vaoID;
	GLuint basicProgramID;

	// Could store these two in an array GLuint[]
	GLuint vertBufferID;
	GLuint colorBufferID;

	void generateIDs();
	void deleteIDs();

private:
	// Would like to in here (if possible):
	void setupVAO();
	void loadBuffer(std::vector<Vec3f>, Vec3f);
	void reloadMVPUniform(Mat4f);

};

#endif /* OPENGLTOOLS_GLDATA_H_ */
