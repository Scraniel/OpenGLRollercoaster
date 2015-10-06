/*
 * Track.h
 *
 *  Created on: Oct 1, 2015
 *      Author: dglewis
 */

#ifndef SCENEOBJECTS_TRACK_H_
#define SCENEOBJECTS_TRACK_H_

#include "Renderable.h"

class Track: public Renderable {
public:
	enum trackSegment
	{
		beginning,
		middle,
		end
	};

	Track();
	virtual ~Track();
	float getLength() const;
	virtual void setVerts(std::vector<Vec3f>);
	trackSegment getTrackSegment(int);
	float getHeight(Vec3f);
	float getMaxHeight();


private:
	void calculateLength();
	void partitionTrack();
	void calculateMinAndMaxPoints();
	float length, maxHeight;
	int beginningIndex, middleIndex, endIndex; // indices of where each segment of the track starts
	Vec3f minPoint, maxPoint;
};

#endif /* SCENEOBJECTS_TRACK_H_ */
