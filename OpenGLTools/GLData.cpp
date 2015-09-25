/*
 * GLData.cpp
 *
 *  Created on: Sep 25, 2015
 *      Author: dglewis
 */

#include "GLData.h"

GLData::GLData()
{
	// TODO Auto-generated constructor stub
	//generateIDs();

}

GLData::~GLData()
{
	// TODO Auto-generated destructor stub
	//deleteIDs();
}

void GLData::generateIDs()
{
	std::string vsSource = loadShaderStringfromFile( "./Shaders/basic_vs.glsl" );
	std::string fsSource = loadShaderStringfromFile( "./Shaders/basic_fs.glsl" );
	basicProgramID = CreateShaderProgram( vsSource, fsSource );

	// load IDs given from OpenGL
	glGenVertexArrays( 1, &vaoID );
	glGenBuffers( 1, &vertBufferID );
	glGenBuffers( 1, &colorBufferID );
}

void GLData::deleteIDs()
{
	glDeleteProgram( basicProgramID );
	glDeleteVertexArrays( 1, &vaoID );
	glDeleteBuffers( 1, &vertBufferID );
	glDeleteBuffers( 1, &colorBufferID );
}


