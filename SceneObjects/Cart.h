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

class Cart: public Renderable {
public:
	Cart();
	Cart(Vec3f, Track);
	virtual ~Cart();
	void move();
	void setTrack(Track);
	void setCurrentPosition(Vec3f);
private:
	float getSpeed();
	Mat4f calcModelMatrix();
	const float g = 9.81;
	Track track;
	int currentIndex;
	Vec3f currentPosition;
	float minSpeed, slowFactor, currentSpeed;
};

#endif /* SCENEOBJECTS_CART_H_ */
