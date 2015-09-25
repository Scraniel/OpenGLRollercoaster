/*
 * GLCurve.cpp
 *
 *  Created on: Sep 23, 2015
 *      Author: dglewis
 */

#include "GLCurve.h"

GLCurve::GLCurve() {
	// TODO Auto-generated constructor stub

}

GLCurve::~GLCurve() {
	// TODO Auto-generated destructor stub
}


std::vector<Vec3f> GLCurve::getVerts()
{
	return verts;
}

void GLCurve::setVerts(std::vector<Vec3f> newVerts)
{
	verts = newVerts;
}

Vec3f GLCurve::getColour()
{
	return colour;
}

void GLCurve::setColour(Vec3f newColour)
{
	colour = newColour;
}
