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
	for(unsigned int i = 0; i < verts.size()-1; i++)
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

	return beginning;
}

void Track::partitionTrack()
{
	beginningIndex = 0;
	endIndex = getVerts().size()*(8./9.);

}

void Track::calculateMinAndMaxPoints()
{
	middleIndex = 0;
	minPoint = maxPoint = getVerts().at(0);
	Vec3f current;
	for(unsigned int i = 0; i < getVerts().size(); i++)
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

std::vector<float> Track::getColours()
{
	std::vector<float> colours;
	for(int i = 0; i < middleIndex; i++){
		colours.push_back(0);
		colours.push_back(0);
		colours.push_back(1);
	}
	for(unsigned int i = middleIndex; i < endIndex; i++){
		colours.push_back(getColour().x());
		colours.push_back(getColour().y());
		colours.push_back(getColour().z());
	}
	for(unsigned int i = endIndex; i < getVerts().size(); i++){
		colours.push_back(0);
		colours.push_back(1);
		colours.push_back(1);
	}


	return colours;
}
