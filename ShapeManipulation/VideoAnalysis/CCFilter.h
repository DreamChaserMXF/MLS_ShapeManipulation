// CCFilter.h : Connected Component Analysis 连通域滤波
#ifndef CCFILTER_H
#define CCFILTER_H
#include <opencv.hpp>
#include <vector>
#include "BoundingBox.h"


class CCFilter
{
public:
	enum{BACKGROUND = 0, FOREGROUND = 255};
	CCFilter(int width, int height);
	~CCFilter();
	void filter(const cv::Mat& input_img, cv::Mat& output_img, int background_threashold, int foreground_threshold);
	void detect(const cv::Mat& input_img, std::vector<BoundingBox>& bdboxes);

	static bool GetMaskCenter(const cv::Mat& mask, cv::Point& center);
private:
	int CCGrowing(const cv::Mat& input_img, int queue_length, uchar color);
	int width_;
	int height_;
	cv::Point*	cc_point_buffer_;	// 用于连通域增长时存储点的位置
	bool*		cc_visited_mask_;	// 访问标志

};

#endif