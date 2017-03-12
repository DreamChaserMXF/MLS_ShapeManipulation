#ifndef CONTOURDETECTOR_H
#define CONTOURDETECTOR_H

#include <string>
#include <vector>
#include <opencv.hpp>
#include "DetectionCallbackFunction.h"

class ContourDetector
{
public:
	ContourDetector(const std::string& edge_winname, const std::string& contour_winname);
	void findContour(const std::string& img_path);
	cv::Size getResizedImgSize() const;
	std::vector<cv::Point> getContour() const
	{
		return contour_;
	}
	std::vector<cv::Point2d> getSmoothContour() const
	{
		return smooth_contour_;
	}
	cv::Point2d center2d_;
	cv::Point2d y_axis_;	// 鱼的主轴设为y_axis
	int head_idx_;
	int tail_idx_;
	int left_fin_idx_;
	int right_fin_idx_;
private:
	
	void ExtractContour();
	void PCAProcess(cv::Point2d& center2d_, cv::Point2d& y_axis_) const;
	void LocateFeaturePointIdx(const std::vector<cv::Point2d>& smooth_contour, cv::Point2d center);
	void RefineYAxis();
	void GetCenterDistance(const std::vector<cv::Point2d>& contour, std::vector<double>& center_dist, cv::Point center, int& max_idx, int& min_idx) const;
	void FindHeadLocation(std::vector<double>& center_dist, int tail_idx, int& head_idx) const;
	void ConfirmHeadTail(const std::vector<cv::Point2d>& contour, int& head_idx, int& tail_idx) const;
	void FindFinLocation(const std::vector<cv::Point2d>& smooth_contour, int head_idx, int tail_idx, int& left_fin_idx, int& right_fin_idx) const;
	void ConfirmLeftRight(const std::vector<cv::Point2d>& smooth_contour, int tail_idx, int& left_idx, int& right_idx) const;
	void CalcCenter();

	const std::string edge_winname_;
	const std::string contour_winname_;
	cv::Mat resized_img_;
	cv::Mat contour_img_;
	std::vector<std::vector<cv::Point>> contours_;
	std::vector<cv::Point> contour_;
	std::vector<cv::Point2d> smooth_contour_;
	bool external_contour_only_;


	friend void on_trackbar_change(int pos, void* userdata);
	friend void on_mouse(int event, int x, int y, int flags, void* param);
};

#endif