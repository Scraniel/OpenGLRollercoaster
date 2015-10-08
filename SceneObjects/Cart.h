/*
 * Cart.h
 *
 *  Created on: Oct 1, 2015
 *      Author: dglewis
 */

#ifndef SCENEOBJECTS_CART_H_
#define SCENEOBJECTS_CART_H_

#include "Renderable.h"
#include "Track.h"
#include "../MathTools/VectorTools.h"

class Cart: public Renderable {
public:
	Cart();
	Cart(Vec3f, Track);
	virtual ~Cart();
	void move();
	void setTrack(Track);
	void setCurrentPosition(Vec3f);

	constexpr static float g = 0.001;
	static Mat4f calcModelMatrix(Vec3f, Track, int, float);
	int currentIndex;
	float getSpeed();
	float getPositionDifference();
private:

	Track track;

	Vec3f currentPosition, lastPolled;
	float minSpeed, slowFactor, currentSpeed;
};

#endif /* SCENEOBJECTS_CART_H_ */
