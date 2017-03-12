#ifndef TRACKER_H
#define TRACKER_H

#include "global_variable.h"
#include "CCFilter.h"
#include <opencv.hpp>
#include <vector>

class Tracker
{
public:
	enum TrackerState{TRACKING = 0x01, MISSING = 0x02, LOST = 0x04};

	Tracker(int max_lostcount, int max_corresponding_distance);
	TrackerState getState() const;
	void Init(const std::vector<BoundingBox>& boundingboxes, const cv::Point& selected_point, BoundingBox& selected_boundingbox);
	void Update(const cv::Mat& mask, const std::vector<BoundingBox>& boundingboxes, BoundingBox& selected_boundingbox);
	void DrawTrajectory(cv::Mat& img, cv::Scalar color);
private:
	const int kMaxLostCount_;
	const int kMaxCorrespondingDistanceSquare_;
	TrackerState state_;
	int lostcount_;
	std::vector<cv::Point> trajectory_;
};

void TrackObjectCenter(IplImage *p_foreground_img, 
						const BoundingBox *p_boundingboxes, int bounding_box_count, 
						cv::Point *p_tracked_point, CvScalar color, bool *pIsObservationExist, BoundingBox *pTrackedBoudingBox);

#endif