/*
 * VectorTools.cpp
 *
 *  Created on: Sep 25, 2015
 *      Author: dglewis
 */

#include "VectorTools.h"

/*
 * Creates a subdivision curve (Chaikin) using the 'Chasing Game' algorithm.
 * NOTE: Does not move the first or last points in the curve.
 *
 * points: the points that create the initial curve to subdivide.
 * depth: how many iterations to subdivide. Higher depth = more points, smoother curve.
 *
 * Returns:
 * A vector containing the new points representing the subdivided curve.
 */
std::vector<Vec3f> VectorTools::subdivide(std::vector<Vec3f> points, int depth)
{
	if(depth == 0)
	{
		return points;
	}

	std::vector<Vec3f> newPoints;

	// we don't want the first point to creep up, so we add it as is
	newPoints.push_back(points.at(0));

	// For each pair of points, we create a new point halfway between them
	for(int i = 0; i < points.size() - 1; i++)
	{
		Vec3f firstPoint = points.at(i);
		Vec3f secondPoint = points.at(i+1);

		Vec3f inBetweenPoint = affineCombination(firstPoint, secondPoint, 0.5);

		// Move each point halfway towards its new neighbour
		firstPoint = affineCombination(firstPoint, inBetweenPoint, 0.5);
		inBetweenPoint = affineCombination(inBetweenPoint, secondPoint, 0.5);

		if(i != 0){
			newPoints.push_back(firstPoint);
		}
		newPoints.push_back(inBetweenPoint);
	}

	// We don't want the last point to creep up either, so we add it as is
	newPoints.push_back(points.at(points.size() - 1));

	return subdivide(newPoints, depth - 1);

}

/*
 * Returns and affine combination of the two points given a proportion.
 * ie. Moves firstPoint a proportion of the way towards secondPoint.
 *
 * TODO: Move into one of the math helper classes?
 */
Vec3f VectorTools::affineCombination(Vec3f firstPoint, Vec3f secondPoint, float proportion)
{
	return firstPoint + ((secondPoint - firstPoint) * proportion);
}
