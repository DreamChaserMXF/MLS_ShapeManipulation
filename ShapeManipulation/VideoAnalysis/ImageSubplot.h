#ifndef IMAGESUBPLOT
#define IMAGESUBPLOT

#include <opencv.hpp>
class ImageSubplot
{
public:
	static void subplot(cv::Mat &figure, const cv::Mat &img, int row, int col, int show_idx);
	static cv::Point RecoverPoint(cv::Size figure_size, cv::Size img_size, int row, int col, int show_idx, cv::Point point);
};

#endif