#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H
#include <opencv.hpp>
#include <vector>
class BoundingBox		// 包围盒
{
public:
	cv::Point top_left_;		// 左上角
	cv::Point bottom_right_;	// 右下角
	cv::Point object_center_;// 包围盒中前景（目标）的形心

	static bool GetIntersection(const BoundingBox& box1, const BoundingBox& box2, BoundingBox& result_box);
	static void DrawingBoundingBoxes(cv::Mat& output_img, const std::vector<BoundingBox> &bdboxes, cv::Scalar color, int thickness = 1, int line_type = 8);
	static bool GetSelectedBoundingBox(const std::vector<BoundingBox>& boundingboxes, const cv::Point& selected_point, BoundingBox& selected_boundingbox);
	static bool GetClosestBoundingBox(const std::vector<BoundingBox>& boundingboxes, const BoundingBox& original_boundingbox, BoundingBox& closest_boundingbox, int& min_distance);
};

#endif