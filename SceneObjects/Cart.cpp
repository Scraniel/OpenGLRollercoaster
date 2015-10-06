/*
 * Cart.cpp
 *
 *  Created on: Oct 1, 2015
 *      Author: dglewis
 */

#include "Cart.h"

Cart::Cart() {
	// TODO Auto-generated constructor stub
	minSpeed = 0.01;
	slowFactor = 0.1;
	currentIndex = 0;
}

Cart::Cart(Vec3f currentPosition, Track track)
{
	this->track = track;
	this->currentPosition = currentPosition;
	currentIndex = 0;
	minSpeed = 0.01;
	slowFactor = 0.5;
}

Cart::~Cart() {
	// TODO Auto-generated destructor stub
}

void Cart::move()
{
	float distance = getSpeed();

	int nextIndex = (currentIndex + 1) % track.getVerts().size();
	Vec3f previousPoint = track.getVerts().at(currentIndex);
	Vec3f nextPoint = track.getVerts().at(nextIndex);
	Vec3f toGo, newPosition;
	Vec3f previousToNext = nextPoint - previousPoint;
	float distanceTravelled = (nextPoint - currentPosition).length();

	// We are on the correct line segment
	if( distanceTravelled >= distance)
	{
		toGo = (previousToNext.normalized()) * distance;
		newPosition = currentPosition + toGo;

		// Special case: The cart needs to land EXACTLY on the next point
		if(distanceTravelled == distance)
		{
			currentIndex = nextIndex;
			//std::cout << currentIndex << "\n";
		}
	}
	// We're not on the correct line segment; find it!
	else
	{

		// Look through each segment until we've accumulated enough distance
		// TODO: Talk to Andrew about removing this now that curve is arc
		// 		 length reparameterized
		do
		{
			currentIndex = nextIndex;
			//std::cout << currentIndex << "\n";
			nextIndex = (nextIndex + 1) % track.getVerts().size();
			previousPoint = track.getVerts().at(currentIndex);
			nextPoint = track.getVerts().at(nextIndex);
			previousToNext = nextPoint - previousPoint;

			distanceTravelled +=  previousToNext.length();


		}while(distanceTravelled < distance);

		// This is necessary because we need to know how much distance was traversed
		// on every line segment BUT the one we're on
		distanceTravelled -= previousToNext.length();

		// We've found the right segment!
		// Our new position is some percentage along this segment. To find that percentage,
		// subtract how far we went on every line segment BUT this one from how far we want to go
		// (which gives us how much further we need to go) and divide it by the length of the
		// current segment
		newPosition = previousPoint + (previousToNext.normalized()) * (distance - distanceTravelled);


		toGo = newPosition - currentPosition;
	}

	M = M * TranslateMatrix(toGo.x(), toGo.y(), toGo.z());
	currentPosition = currentPosition + toGo;
}

// Somewhat coupled to cart, not sure if that's a problem
Mat4f Cart::calcModelMatrix()
{

	// Need 3 points to find oscilating circle at this point
	Vec3f previousPoint, currentPoint, nextPoint, tangent, normal, binormal;
	currentPoint = track.getVerts().at(currentIndex);
	nextPoint = track.getVerts().at((currentIndex+1) % track.getVerts().size());
	if(currentIndex == 0)
	{
		previousPoint = track.getVerts().at(track.getVerts().size() - 1);
	}
	else
	{
		previousPoint = track.getVerts().at(currentIndex - 1);
	}

	// The tangent to the curve is ~ the vector to the next point since
	// the arcs are so small
	Vec3f previousToNext = nextPoint - previousPoint;
	tangent = previousToNext.normalized();

	// The normal is pointing straight down
	Vec3f numerator = nextPoint - (currentPoint * 2) + previousPoint;
	normal = numerator.normalized();


	binormal = tangent.crossProduct(normal);

	// Need to change the axes so they're all orthogonal to one another
	tangent = binormal.crossProduct(normal);

	// Accounting for centripital force
	float h = (numerator * 0.5).length();
	float c = (nextPoint - previousPoint).length();
	float r = ((c * c) + (4 * h * h)) / (8 * h);
	float s = getSpeed();
	float force = (s * s)/r;

	// Accounting for gravity

	normal = (normal * force) + Vec3f(0, g, 0);

	Mat4f total(
			{
			tangent.x(), normal.x(), binormal.x(), 0,
			tangent.y(), normal.y(), binormal.y(), 0,
			tangent.z(), normal.z(), binormal.z(), 0,
			0,			 0,			 0,			   1
			}
			);

	std::cout << total << "\n";

	return total;

}

void Cart::setCurrentPosition(Vec3f currentPosition)
{
	this->currentPosition = currentPosition;
}

void Cart::setTrack(Track track)
{
	this->track = track;
}

float Cart::getSpeed()
{
	switch(track.getTrackSegment(currentIndex))
	{
	// We're in the 'ramp up' portion of the track
	case Track::beginning:
		currentSpeed = minSpeed;
		break;
	// We're in the 'physics' portion of the track
	case Track::middle:

		currentSpeed = sqrt(2.0 * g * (track.getMaxHeight() - track.getHeight(currentPosition))) / 100;
		break;
	// We're in the 'slow down' portion of the track
	case Track::end:
		currentSpeed = currentSpeed = fmax(minSpeed, currentSpeed * slowFactor);
		break;
	}

	return currentSpeed;
}