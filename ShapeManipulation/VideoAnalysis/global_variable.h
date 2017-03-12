#ifndef GLOBAL_VARIABLE_H
#define GLOBAL_VARIABLE_H

#include <stdio.h>
#include <math.h>
#include <cv.h>
#include "ContourHandler.h"

#define OUTPUT_LOC printf(" file:%s No.%d", __FILE__, __LINE__);


//#define FIXED_WIDTH_HEIGHT	// 用于将从视频中获取的图像转换成固定大小的图像
//#define BACKGROUND_SHOW		// 标志是否展示背景模型
//#define EIGHT_CONNECTED		// 八连通的连通域标记
//#define EDGE_DETECTOR
//#define VIDEOWRITING

const CvScalar COLOR_WHITE	= CV_RGB(255, 255, 255);
const CvScalar COLOR_BLACK	= CV_RGB(0, 0, 0);
const CvScalar COLOR_GRAY	= CV_RGB(100, 100, 100);
const CvScalar COLOR_RED	= CV_RGB(255, 0, 0);
const CvScalar COLOR_GREEN	= CV_RGB(0, 255, 0);
const CvScalar COLOR_BLUE	= CV_RGB(0, 0, 255);
const CvScalar COLOR_YELLOW	= CV_RGB(255, 255, 0);

const double PI = 3.141592653589793;

template<class T>
cv::Point_<T> operator / (const cv::Point_<T>& lhs, double denominator)
{
	return cv::Point_<T>((T)(lhs.x / denominator), (T)(lhs.y / denominator));
}

template<class T>
cv::Point_<T> normalize(const cv::Point_<T> vec)
{
	double norm = sqrt(vec.x * vec.x + vec.y * vec.y);
	return cv::Point_<T>(static_cast<T>(vec.x / norm), static_cast<T>(vec.y / norm));
}

template<class T>
T GetPointDist(const cv::Point_<T>& p1, const cv::Point_<T>& p2)
{
	return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

template<class T>
T GetPointDistSquare(const cv::Point_<T>& p1, const cv::Point_<T>& p2)
{
	return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}

int WindowPauseControl();


#endif