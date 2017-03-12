#include "DetectionCallbackFunction.h"
#include "ContourDetector.h"

extern const std::string kTrackbarName;
bool IsIntersect(const std::vector<cv::Point>& contour_points, const cv::Rect& rect);

void on_trackbar_change(int pos, void* userdata)
{
	ContourDetector *pDetector = static_cast<ContourDetector*>(userdata);
	double low_threshold = cv::getTrackbarPos(kTrackbarName, pDetector->edge_winname_);
	double ratio = 3.0;
	// ��Եͼ��
	cv::Mat edge_img, morph_close_img;
	cv::Canny(pDetector->resized_img_, edge_img, low_threshold, ratio * low_threshold);
	// ����+��ʴ
	cv::morphologyEx(edge_img, morph_close_img, cv::MORPH_CLOSE, cv::Mat());
	cv::imshow(pDetector->edge_winname_, morph_close_img);
	// �ҵ�����
	morph_close_img.copyTo(pDetector->contour_img_);
	cv::findContours(pDetector->contour_img_, pDetector->contours_, pDetector->external_contour_only_? CV_RETR_EXTERNAL : CV_RETR_LIST , CV_CHAIN_APPROX_NONE);
	// ����������
	pDetector->contour_img_.setTo(0);
	cv::polylines(pDetector->contour_img_, pDetector->contours_, true, CV_RGB(255, 255, 255));	// �ؼ��ǵ�������ɫ����Ҫ��255
	cv::imshow(pDetector->contour_winname_, pDetector->contour_img_);
}

// Ȧһ�����Σ�ɾ��������������غϵ�����
void on_mouse(int event, int x, int y, int flags, void* param)
{
	static cv::Point left_top, right_bottom;

	ContourDetector *pDetector = static_cast<ContourDetector*>(param);

	if(CV_EVENT_LBUTTONDOWN == event)		// ��갴��
	{
		left_top.x = x;
		left_top.y = y;
	}
	else if(CV_EVENT_MOUSEMOVE == event && CV_EVENT_FLAG_LBUTTON & flags)	// ��껬��
	{
		cv::Mat tmp_img = pDetector->contour_img_.clone();
		cv::rectangle(tmp_img, left_top, cvPoint(x, y), CV_RGB(255, 255, 255));
		cv::imshow(pDetector->contour_winname_, tmp_img);
	}
	else if(CV_EVENT_LBUTTONUP == event)	// ���̧��
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
		// Ĩ����þ����ཻ������
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
		cv::polylines(pDetector->contour_img_, pDetector->contours_, true, CV_RGB(255, 255, 255));	// �ؼ��ǵ�������ɫ����Ҫ��255
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

