#pragma once
#include <iostream>
#include "opencv2/opencv.hpp"

//using namespace cv;
//using namespace std;

#define NUM_SAMPLES 20		//每个像素点的样本个数
#define MIN_MATCHES 2		//#最小单个匹配个数（超过这个数量即认定为与背景匹配）
#define RADIUS 20			//Sphere半径
#define SUBSAMPLE_FACTOR 16	//子采样概率(即背景更新概率)


class ViBe_BGS_RGB
{
public:
	ViBe_BGS_RGB(void);
	~ViBe_BGS_RGB(void);

	void init(const cv::Mat& _image);   //初始化
	void processFirstFrame(const cv::Mat& _image);
	void testAndUpdate(const cv::Mat& _image);  //更新
	cv::Mat getMask(void){return m_mask;};

private:
	cv::Mat m_samples[NUM_SAMPLES];
	cv::Mat m_foregroundMatchCount;
	cv::Mat m_mask;
};