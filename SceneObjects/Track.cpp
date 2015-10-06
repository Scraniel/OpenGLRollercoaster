/*
 * Track.cpp
 *
 *  Created on: Oct 1, 2015
 *      Author: dglewis
 */

#include "Track.h"

Track::Track() {
	// TODO Auto-generated constructor stub

}

Track::~Track() {
	// TODO Auto-generated destructor stub
}

void Track::setVerts(std::vector<Vec3f> newVerts)
{
	Renderable::setVerts(newVerts);
	calculateLength();
	partitionTrack();
	calculateMinAndMaxPoints();

	//std::cout << "Max Height: " << maxHeight << "\nMiddle: " << middleIndex << "\nEnd: " << endIndex << "\n";
}

float Track::getLength() const
{
	return length;
}

void Track::calculateLength()
{
	length = 0;
	std::vector<Vec3f> verts = getVerts();
	for(int i = 0; i < verts.size()-1; i++)
	{
		 Vec3f previous = verts.at(i);
		 Vec3f next = verts.at(i+1);

		 length += (next - previous).length();
	}
}

Track::trackSegment Track::getTrackSegment(int index)
{
	if(index > endIndex)
	{
		return end;
	}
	else if(index > middleIndex)
	{
		return middle;
	}

	return end;
}

void Track::partitionTrack()
{
	beginningIndex = 0;
	endIndex = getVerts().size();

}

void Track::calculateMinAndMaxPoints()
{
	middleIndex = 0;
	minPoint = maxPoint = Vec3f(0,0,0);
	Vec3f current;
	for(int i = 0; i < getVerts().size(); i++)
	{
		current = getVerts().at(i);
		if(current.y() < minPoint.y())
		{
			minPoint = current;
		}
		if(current.y() > maxPoint.y())
		{
			maxPoint = current;
			middleIndex = i;
		}
	}


	maxHeight = fabs((maxPoint - minPoint).y());

}

float Track::getHeight(Vec3f position)
{
	return fabs((position - minPoint).y());
}

float Track::getMaxHeight()
{
	return maxHeight;
}
