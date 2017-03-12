#pragma once
#include <iostream>
#include "opencv2/opencv.hpp"

//using namespace cv;
//using namespace std;

#define NUM_SAMPLES 20		//ÿ�����ص����������
#define MIN_MATCHES 2		//#��С����ƥ���������������������϶�Ϊ�뱳��ƥ�䣩
#define RADIUS 20			//Sphere�뾶
#define SUBSAMPLE_FACTOR 16	//�Ӳ�������(���������¸���)


class ViBe_BGS_RGB
{
public:
	ViBe_BGS_RGB(void);
	~ViBe_BGS_RGB(void);

	void init(const cv::Mat& _image);   //��ʼ��
	void processFirstFrame(const cv::Mat& _image);
	void testAndUpdate(const cv::Mat& _image);  //����
	cv::Mat getMask(void){return m_mask;};

private:
	cv::Mat m_samples[NUM_SAMPLES];
	cv::Mat m_foregroundMatchCount;
	cv::Mat m_mask;
};