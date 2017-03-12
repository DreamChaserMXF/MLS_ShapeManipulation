#include "..\CommonFunc.h"
#include "ContourDetector.h"
#include "EFCHandler.h"
#include "DetectionCallbackFunction.h"

extern const std::string kTrackbarName = "Canny";


ContourDetector::ContourDetector(const std::string& edge_winname, const std::string& contour_winname)
	: edge_winname_(edge_winname), contour_winname_(contour_winname), external_contour_only_(true)
{
	
}

void ContourDetector::findContour(const std::string& img_path)
{
	cv::namedWindow(edge_winname_);
	cv::namedWindow(contour_winname_);

	cv::Mat img = cv::imread(img_path);
	if(NULL == img.data)
	{
		std::cerr << "\ncannot read img file: " << img_path;
		exit(EXIT_FAILURE);
	}
	int width = 512;
	int height = (int)(img.size().height * 512.0 / img.size().width);
	cv::resize(img, resized_img_, cv::Size(width, height));
	cv::resizeWindow(edge_winname_, width, height);
	cv::resizeWindow(contour_winname_, width, height);
	cv::createTrackbar(kTrackbarName, edge_winname_, nullptr, 100, on_trackbar_change, this);
	cv::setTrackbarPos(kTrackbarName, edge_winname_, 20);
	cv::setMouseCallback(contour_winname_, on_mouse, this);

	cv::imshow(edge_winname_, resized_img_);
	cv::waitKey(1);
	//on_trackbar_change(0, this);	// һ��Ҫ����һ�Σ��Գ�ʼ��contour_img_�����֮ǰ���ù�setTrackbarPos���ᴥ��on_trackbar_change�Ļ���������Բ�����
	printf("\n'Enter' ---- auto-extract contour.");
	printf("\n's'     ---- switch contour mode between external only or all contour level.");
	printf("\n'r'     ---- reset contour showing.");
	int key_event = 0;
	while(27 != key_event)	// ����ESC�Ͳ��˳���һֱ�������
	{
		key_event = cv::waitKey();
		if('s' == key_event || 'S' == key_event)
		{
			external_contour_only_ = !external_contour_only_;
			printf(external_contour_only_? "\n External Contour Only" : "\n All Contour Level");
			on_trackbar_change(cv::getTrackbarPos(kTrackbarName, edge_winname_), this);
		}
		else if(13 == key_event)
		{
			printf("\nPress ESC to return with current contour, or move trackbar to continue");
			ExtractContour();
			if(contour_.size() > 0)	// ����������Ļ�
			{
				PCAProcess(center2d_, y_axis_);	// ͨ��PCA�ҵ���������ĵ�
				LocateFeaturePointIdx(smooth_contour_, center2d_);	// �ҵ�ʣ��ļ���������
				RefineYAxis();	// ��������ķ���
				//CalcCenter();
			}
		}
		else if('r' == key_event || 'R' == key_event)
		{
			cv::setTrackbarPos(kTrackbarName, edge_winname_, 20);
			on_trackbar_change(cv::getTrackbarPos(kTrackbarName, edge_winname_), this);
		}
	}
	cv::destroyAllWindows();
	printf("\n");
}

cv::Size ContourDetector::getResizedImgSize() const
{
	return resized_img_.size();
}

void ContourDetector::ExtractContour()
{
	// ���ж��м�������
	if(contours_.size() == 0)
	{
		contour_img_ = 0;
		cv::imshow(contour_winname_, contour_img_);
		fprintf(stderr, "\nno contour found!");
		return;
	}
	int max_len_contour_idx = 0;
	if(contours_.size() > 1)
	{
		// ɸѡ�������Ǹ�
		int max_contour_len = 0;
		for(unsigned int i = 0; i < contours_.size(); ++i)
		{
			int contour_len = contours_[i].size();
			if(contour_len > max_contour_len)
			{
				max_contour_len = contour_len;
				max_len_contour_idx = i;
			}
		}
	}
	// ԭ����ֵ��contour_
	contour_ = contours_[max_len_contour_idx];
	// �ø���Ҷƽ��һ��contour_
	EFCHandler efc_handler;
	efc_handler.SmoothContour(contour_, smooth_contour_, 30);
	// ��ʾƽ���������
	std::vector<std::vector<cv::Point>> smooth_contours(1, std::vector<cv::Point>(smooth_contour_.begin(), smooth_contour_.end()));
	contour_img_.setTo(0);
	cv::polylines(contour_img_, smooth_contours, true, CV_RGB(255, 255, 255));	// �ؼ��ǵ�������ɫ����Ҫ��255����thickness������CV_FILLED
	cv::imshow(contour_winname_, contour_img_);	
}

void ContourDetector::PCAProcess(cv::Point2d& center2d, cv::Point2d& y_axis) const
{
	// �Ȱ���������������������
	std::vector<std::vector<cv::Point>> smooth_contours(1, std::vector<cv::Point>(smooth_contour_.begin(), smooth_contour_.end()));
	cv::Mat mask = contour_img_.clone();
	mask.setTo(0);
	cv::drawContours(mask, smooth_contours, 0, CV_RGB(255, 255, 255), CV_FILLED);	// �ؼ��ǵ�������ɫ����Ҫ��255����thickness������CV_FILLED
	//cv::imshow("mask", mask);	
	// ����pca���ݼ�
	cv::Size mask_size = mask.size();
	int count = cv::countNonZero(mask);
	cv::Mat pcaset(count, 2, CV_64FC1);
	int i = 0;
	for(int r = 0; r < mask_size.height; ++r)
	{
		for(int c = 0; c < mask_size.width; ++c)
		{
			if(mask.at<uchar>(r, c) > 0)
			{
				pcaset.at<double>(i, 0) = c;
				pcaset.at<double>(i, 1) = r;
				++i;
			}
		}
	}
	// ����PCA����
	cv::PCA pca(pcaset, cv::Mat(), CV_PCA_DATA_AS_ROW, 2);
	center2d.x = pca.mean.at<double>(0);
	center2d.y = pca.mean.at<double>(1);
	// ��¼����
	y_axis.x = pca.eigenvectors.at<double>(0, 0);
	y_axis.y = pca.eigenvectors.at<double>(0, 1);
}

void ContourDetector::LocateFeaturePointIdx(const std::vector<cv::Point2d>& smooth_contour, cv::Point2d center)
{
	std::vector<double> center_dist;
	// ��β
	int max_idx, min_idx;
	GetCenterDistance(smooth_contour, center_dist, center, max_idx, min_idx);
	// ��ͷ
	tail_idx_ = max_idx;
	FindHeadLocation(center_dist, tail_idx_, head_idx_);
	// ȷ��ͷβλ�ã�����Ū���ˣ�������о�����
	ConfirmHeadTail(smooth_contour, head_idx_, tail_idx_);
	// ����
	FindFinLocation(smooth_contour, head_idx_, tail_idx_, left_fin_idx_, right_fin_idx_);
	// ȷ������������
	ConfirmLeftRight(smooth_contour, tail_idx_, left_fin_idx_, right_fin_idx_);
}

void ContourDetector::RefineYAxis()
{
	// y_axis_��Ҫ��β��ָ��ͷ��
	cv::Point2d tail2head = smooth_contour_[head_idx_] - smooth_contour_[tail_idx_];
	if(y_axis_.x * tail2head.x + y_axis_.y * tail2head.y < 0)
	{
		y_axis_ = -y_axis_;
	}
}

void ContourDetector::GetCenterDistance(const std::vector<cv::Point2d>& contour, std::vector<double>& center_dist, cv::Point center, int& max_idx, int& min_idx) const
{
	int contour_len = contour.size();
	center_dist.resize(contour_len);
	double max_dist = -DBL_MAX;
	double min_dist = DBL_MAX;
	for(int j = 0; j < contour_len; ++j)
	{
		center_dist[j] = sqrt((double)(contour[j].x - center.x) * (contour[j].x - center.x) + (contour[j].y - center.y) * (contour[j].y - center.y));
		if(center_dist[j] > max_dist)
		{
			max_dist = center_dist[j];
			max_idx = j;
		}
		else if(center_dist[j] < min_dist)
		{
			min_dist = center_dist[j];
			min_idx = j;
		}
	}
}

// Ѱ����ͷ��λ��
void ContourDetector::FindHeadLocation(std::vector<double>& center_dist, int tail_idx, int& head_idx) const
{
	int contour_len = center_dist.size();
	int quarter_len = contour_len / 4;
	int lower_bound = (tail_idx + quarter_len) % contour_len;
	int upper_bound = (tail_idx + contour_len - quarter_len) % contour_len;
	double max_dist = -DBL_MAX;
	int max_idx = -1;
	for(int j = lower_bound; j != upper_bound; j = (j + 1) % contour_len)
	{
		if(center_dist[j] > max_dist)
		{
			max_dist = center_dist[j];
			max_idx = j;
		}
	}
	if(-1 == max_idx)
	{
		std::cout << " wrong max_idx, maybe the contour size is 0. " << contour_len << " " << center_dist.size() << " " << lower_bound << " " << upper_bound;
		exit(-1);
	}
	head_idx = max_idx;
}

// ͷβҪ��ȷ��һ�£���ҪŪ�ߵ�
// ȷ�Ϸ����ǣ���ͷ����������������������ڵ�һ�������ĵ㣬��������֮��ľ���
// �ٴ�β������ͬ����һ�������ĵ㣬�������
// ������Ϊͷ
void ContourDetector::ConfirmHeadTail(const std::vector<cv::Point2d>& contour, int& head_idx, int& tail_idx) const
{
	int contour_len = contour.size();
	double head_sum_dist_square = 0;
	double tail_sum_dist_square = 0;
	int quater_len = contour_len / 10;
	for(int i = 1; i <= quater_len; ++i)
	{
		head_sum_dist_square += GetPointDistSquare(contour[(head_idx + contour_len - i) % contour_len], contour[(head_idx + i) % contour_len]);
		tail_sum_dist_square += GetPointDistSquare(contour[(tail_idx + contour_len - i) % contour_len], contour[(tail_idx + i) % contour_len]);
	}
	if(head_sum_dist_square < tail_sum_dist_square)
	{
		std::swap(head_idx, tail_idx);
	}
}

void ContourDetector::FindFinLocation(const std::vector<cv::Point2d>& smooth_contour, int head_idx, int tail_idx, int& left_fin_idx, int& right_fin_idx) const
{
	int contour_len = smooth_contour.size();
	cv::Point2d medial_axis = normalize(smooth_contour[head_idx] - smooth_contour[tail_idx]);
	cv::Point2d assist_vec;
	double max_dist = 1.0;
	int max_idx = -1;
	double min_dist = DBL_MAX;
	int min_idx = -1;
	for(int i = 0; i < contour_len; ++i)
	{
		assist_vec = smooth_contour_[i] - smooth_contour_[tail_idx_];
		double medial_axis_dist = assist_vec.x * medial_axis.y - assist_vec.y * medial_axis.x;
		if(medial_axis_dist > max_dist)
		{
			max_dist = medial_axis_dist;
			max_idx = i;
		}
		else if(medial_axis_dist < min_dist)
		{
			min_dist = medial_axis_dist;
			min_idx = i;
		}
	}
	left_fin_idx = max_idx;
	right_fin_idx = min_idx;
}
void ContourDetector::ConfirmLeftRight(const std::vector<cv::Point2d>& smooth_contour, int tail_idx, int& left_idx, int& right_idx) const
{
	cv::Point tail2left = smooth_contour[left_idx] - smooth_contour[tail_idx];
	cv::Point tail2right = smooth_contour[right_idx] - smooth_contour[tail_idx];
	int z = tail2right.x * tail2left.y - tail2right.y * tail2left.x;	// ��˺��ж�Z����
	if(z > 0)	// ˵�����ҷ���
	{
		std::swap(left_idx, right_idx);
	}
}
void ContourDetector::CalcCenter()
{
	cv::Mat img = contour_img_.clone();
	img.setTo(0);
	std::vector<std::vector<cv::Point>> smooth_contours(1, std::vector<cv::Point>(smooth_contour_.begin(), smooth_contour_.end()));
	cv::drawContours(img, smooth_contours, 0, CV_RGB(255, 255, 255), CV_FILLED);
	// ��ʾһ�¿���
	//cv::imshow("filled contour", img);
	// ��������
	int sumX = 0, sumY = 0;
	double weighted_sumX = 0.0, weighted_sumY = 0.0;
	for(int r = 0; r < img.size().height; ++r)
	{
		for(int c = 0; c < img.size().width; ++c)
		{
			if(img.at<uchar>(r, c) > 0U)
			{
				weighted_sumX += c;
				weighted_sumY += r;
				++sumX;
				++sumY;
			}
		}
	}
	center2d_.x = (float)(weighted_sumX / sumX);
	center2d_.y = (float)(weighted_sumY / sumY);
}

