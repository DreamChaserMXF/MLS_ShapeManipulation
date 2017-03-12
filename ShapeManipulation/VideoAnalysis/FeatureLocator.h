#ifndef FEATURELOCATOR_H
#define FEATURELOCATOR_H

#include <opencv.hpp>
#include "HistHandler.h"
#include "ContourHandler.h"
#include "CCFilter.h"

class FeatureLocator
{
public:
	void Init(const cv::Mat& img, const cv::Mat& mask, cv::Point center);
	cv::Mat GetMask() const;
	void DrawAllFeature(cv::Mat& img, cv::Point offset) const;
private:
	//cv::Mat original_img_;
	//cv::Mat original_mask_;
	HistHandler hist_handler_;
	ContourHandler contour_handler_;
	cv::Mat mask_;
	//ContourHandler contour_handler_s;	// 最小的轮廓
	//ContourHandler contour_handler_m;	// 中等的轮廓
	//ContourHandler contour_handler_l;	// 最大的轮廓
};

#endif