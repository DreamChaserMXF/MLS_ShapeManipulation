#include "DetectionCallbackFunction.h"
#include "ContourDetector.h"

extern const std::string kTrackbarName;
bool IsIntersect(const std::vector<cv::Point>& contour_points, const cv::Rect& rect);

void on_trackbar_change(int pos, void* userdata)
{
	ContourDetector *pDetector = static_cast<ContourDetector*>(userdata);
	double low_threshold = cv::getTrackbarPos(kTrackbarName, pDetector->edge_winname_);
	double ratio = 3.0;
	// 边缘图像
	cv::Mat edge_img, morph_close_img;
	cv::Canny(pDetector->resized_img_, edge_img, low_threshold, ratio * low_threshold);
	// 膨胀+腐蚀
	cv::morphologyEx(edge_img, morph_close_img, cv::MORPH_CLOSE, cv::Mat());
	cv::imshow(pDetector->edge_winname_, morph_close_img);
	// 找到轮廓
	morph_close_img.copyTo(pDetector->contour_img_);
	cv::findContours(pDetector->contour_img_, pDetector->contours_, pDetector->external_contour_only_? CV_RETR_EXTERNAL : CV_RETR_LIST , CV_CHAIN_APPROX_NONE);
	// 画出来看看
	pDetector->contour_img_.setTo(0);
	cv::polylines(pDetector->contour_img_, pDetector->contours_, true, CV_RGB(255, 255, 255));	// 关键是第三个颜色分量要是255
	cv::imshow(pDetector->contour_winname_, pDetector->contour_img_);
}

// 圈一个矩形，删掉所有与矩形有重合的轮廓
void on_mouse(int event, int x, int y, int flags, void* param)
{
	static cv::Point left_top, right_bottom;

	ContourDetector *pDetector = static_cast<ContourDetector*>(param);

	if(CV_EVENT_LBUTTONDOWN == event)		// 鼠标按下
	{
		left_top.x = x;
		left_top.y = y;
	}
	else if(CV_EVENT_MOUSEMOVE == event && CV_EVENT_FLAG_LBUTTON & flags)	// 鼠标滑动
	{
		cv::Mat tmp_img = pDetector->contour_img_.clone();
		cv::rectangle(tmp_img, left_top, cvPoint(x, y), CV_RGB(255, 255, 255));
		cv::imshow(pDetector->contour_winname_, tmp_img);
	}
	else if(CV_EVENT_LBUTTONUP == event)	// 鼠标抬起
	{
		right_bottom.x = x;
		right_bottom.y = y;
		if(right_bottom.x < left_top.x)
		{
			std::swap(left_top.x, right_bottom.x);
		}
		if(right_bottom.y < left_top.y)
		{
			std::swap(left_top.y, right_bottom.y);
		}
		// 抹除与该矩形相交的轮廓
		cv::Rect rect(left_top, right_bottom);
		std::vector<std::vector<cv::Point>>::const_iterator c_iter = pDetector->contours_.cbegin();
		while(c_iter != pDetector->contours_.end())
		{
			if(IsIntersect(*c_iter, rect))
			{
				c_iter = pDetector->contours_.erase(c_iter);
			}
			else
			{
				++c_iter;
			}
		}
		pDetector->contour_img_.setTo(0);
		cv::polylines(pDetector->contour_img_, pDetector->contours_, true, CV_RGB(255, 255, 255));	// 关键是第三个颜色分量要是255
		cv::imshow(pDetector->contour_winname_, pDetector->contour_img_);
	}
}

bool IsIntersect(const std::vector<cv::Point>& contour_points, const cv::Rect& rect)
{
	for(std::vector<cv::Point>::const_iterator c_iter = contour_points.cbegin(); c_iter != contour_points.cend(); ++c_iter)
	{
		if(c_iter->x >= rect.x && c_iter->x <= rect.x + rect.width
			&& c_iter->y >= rect.y && c_iter->y <= rect.y + rect.height)
		{
			return true;
		}
	}
	return false;
}

