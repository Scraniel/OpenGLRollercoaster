/*
 * GLCurve.h
 *
 *  Created on: Sep 23, 2015
 *      Author: dglewis
 */

#ifndef GLCURVE_H_
#define GLCURVE_H_

#include "../MathTools/Vec3f.h"

class GLCurve {
public:
	GLCurve();
	virtual ~GLCurve();
	std::vector<Vec3f> getVerts();
	void setVerts(std::vector<Vec3f>);
	Vec3f getColour();
	void setColour(Vec3f);
private:
	// The vertices that make up the curve
	std::vector<Vec3f> verts;
	// The colour to draw the curve as
	Vec3f colour;
};

#endif /* GLCURVE_H_ */
