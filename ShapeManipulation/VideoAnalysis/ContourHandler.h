#ifndef CONTOURHANDLER_H
#define CONTOURHANDLER_H

#include <opencv.hpp>
#include <vector>

class ContourHandler
{
	friend void WriteStandardModel(const ContourHandler& contour_handler);
public:
	enum STATE{INIT = 0, FINDCONTOUR, FINDLABEL, REGULARIZECONTOUR, GENERATECURVATURE};
	enum FISH_DIR{BODY_MIDDLE, BODY_LEFT, BODY_RIGHT};
	enum FEATURE_POINT_TYPE{ALL_FEATURE_POINT, HEAD, TAIL, LEFT_FIN, RIGHT_FIN, CENTER};

	ContourHandler();
	void FindContour(const cv::Mat& mask);
	void FindLabel(const cv::Mat& mask, cv::Point center);		// should call FindContour first
	void ShowDistance() const;	// should call FindLabel first
	void DrawRegularizedContour(cv::Mat& img, const cv::Point& offset) const;
	void DrawMedialAxis(cv::Mat& img, const cv::Point& offset) const;
	void WriteBodyDir(cv::Mat& img, const cv::Point& offset) const;
	void DrawFeaturePoint(cv::Mat& img, const cv::Point& offset, FEATURE_POINT_TYPE point_type) const;
	std::vector<cv::Point2d> GetRegularizedContour() const;
	std::vector<cv::Point> contour_;

private:
	std::vector<cv::Point> FindLongestContour(const std::vector<std::vector<cv::Point>>& contours) const;
	void GetSmoothContour(int contour_len);
	void GetCenterDistance(int contour_len, cv::Point center, int& max_idx, int& min_idx);
	void FindHeadLocation(int& head_idx_, const int& tail_idx);
	void ConfirmHeadTail(int& head_idx, int& tail_idx);
	void ConfirmLeftRight(int& left_idx, int& right_idx, const int tail_idx);
	void FindFinLocation(int& left_idx, int& right_idx);
	void FindFinLocationThoughMedialCurve();
	void AdjustPointSequence();
	void FindMajorAxis(const cv::Mat& mask, cv::Point2d& x_axis, cv::Point2d& y_axis) const;
	//void RegularizeContour();	// should call FindLabel first (to acquire smooth contour point and center point)
	void ContourHandler::RegularizeContour(cv::Point2d x_axis, cv::Point2d y_axis);
	void FindMedialCurve();
	void FindBodyDirection();

	STATE state_;

	//cv::Mat contour_buffer_;
	std::vector<std::vector<cv::Point>> contours_;
	//int goal_contour_idx_;
	std::vector<cv::Point2d> smooth_contour_;
	std::vector<cv::Point2d> regular_contour_;	
	std::vector<double> center_dist_;
	std::vector<double> diff_center_dist_;
	std::vector<double> medial_axis_dist_;
	std::vector<cv::Point2d> medial_curve_;
	std::vector<double> derivative_;		// 一阶导
	std::vector<double> derivative2_;		// 二阶导
	std::vector<double> curvature_;			// 曲率
	cv::Point2d center_;
	int head_idx_;
	int tail_idx_;
	int left_idx_;
	int right_idx_;
	int regularized_head_idx_;
	int regularized_tail_idx_;
	int regularized_left_idx_;
	int regularized_right_idx_;

	FISH_DIR body_dir_;
};

#endif