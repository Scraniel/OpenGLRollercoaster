/*
 * FileHelper.h
 *
 *  Created on: Sep 24, 2015
 *      Author: dglewis
 */

#ifndef HELPERS_FILEHELPER_H_
#define HELPERS_FILEHELPER_H_

#include "../OpenGLTools/GLCurve.h"
class FileHelper{
public:
	static void loadCurveFromFile(const char *, GLCurve &);
};

#endif /* HELPERS_FILEHELPER_H_ */
