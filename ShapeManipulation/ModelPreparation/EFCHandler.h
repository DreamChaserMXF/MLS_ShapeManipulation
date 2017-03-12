#ifndef EFCHANDLER_H
#define EFCHANDLER_H

#include <opencv.hpp>
#include <vector>

struct EFC
{
	double A;
	double B;
	double C;
	double D;
};


// Elliptical Fourier Coefficient Handler
class EFCHandler
{
public:
	EFCHandler();
	template<class PointType1, class PointType2>
	void SmoothContour(const std::vector<PointType1>& input_points, std::vector<PointType2>& output_points, int reconstruction_coef_num = 30) const;	// 用默认参数平滑轮廓曲线
	template<class PointType>
	std::vector<EFC> Encode(const std::vector<PointType> &contour_points, unsigned int coef_num) const;				// 将轮廓点编码为coef_num组椭圆傅里叶系数
	template<class PointType>
	std::vector<PointType> Decode(const std::vector<EFC>& coefficients, unsigned int reconstruct_coef_num, unsigned int point_num) const;				// 将椭圆傅里叶系数还原为point_num个轮廓点
	void CalculateAssitVaraible(unsigned int assist_array_length) const;
	static std::vector<double> assist_2npi;
	static std::vector<double> assist_2_nsq_pisq;
};

template<class PointType1, class PointType2>
void EFCHandler::SmoothContour(const std::vector<PointType1>& input_points, std::vector<PointType2>& output_points, int reconstruction_coef_num) const
{
	std::vector<EFC> coef = Encode(input_points, reconstruction_coef_num);
	output_points = Decode<PointType2>(coef, reconstruction_coef_num, input_points.size());
}


template<class PointType>
std::vector<EFC> EFCHandler::Encode(const std::vector<PointType> &contour_points, unsigned int coef_num) const
{
	std::vector<EFC> coef(coef_num);
	coef[0].B = coef[0].D = 0.0;
	// 异常情况
	if(0 == contour_points.size())
	{
		return coef;
	}
	// 计算辅助变量
	CalculateAssitVaraible(coef_num);
	// 计算deltaX, deltaY, deltaT
	unsigned int point_num = contour_points.size();
	unsigned int seg_num = point_num - 1;
	std::vector<double> deltaX(seg_num, 0.0);
	std::vector<double> deltaY(seg_num, 0.0);
	std::vector<double> deltaT(seg_num, 0.0);
	for(unsigned int i = 0; i < seg_num; ++i)
	{
		deltaX[i] = contour_points[i+1].x - contour_points[i].x;
		deltaY[i] = contour_points[i+1].y - contour_points[i].y;
		deltaT[i] = sqrt(deltaX[i] * deltaX[i] + deltaY[i] * deltaY[i]);
	}
	// 去除重复点
	for(unsigned int i = 0; i < deltaT.size(); ++i)
	{
		if(deltaT[i] < DBL_MIN)
		{
			deltaX.erase(deltaX.cbegin() + i);
			deltaY.erase(deltaY.cbegin() + i);
			deltaT.erase(deltaT.cbegin() + i);
		}
	}
	seg_num = deltaT.size();
	point_num = seg_num + 1;
	// 计算time_stamp
	std::vector<double> time_stamp(point_num, 0.0);
	time_stamp[0] = 0.0;
	for(unsigned int i = 1; i < point_num; ++i)
	{
		time_stamp[i] = time_stamp[i - 1] + deltaT[i - 1];
	}
	double period = time_stamp.back();
	// 计算A0
	double ellipse_dcX = 0.0;
	for(unsigned int i = 0; i < seg_num; ++i)
	{
		double epsilon = 0.0;
		for(unsigned int j = 0; j < i; ++j)
		{
			epsilon += deltaX[j];
		}
		epsilon -= deltaX[i] / deltaT[i] * time_stamp[i];
		double single_item = deltaX[i] / (2.0 * deltaT[i]) * (time_stamp[i+1] * time_stamp[i+1] - time_stamp[i] * time_stamp[i]) + epsilon * (time_stamp[i+1] - time_stamp[i]);
		ellipse_dcX += single_item;
	}
	coef[0].A = ellipse_dcX / period + contour_points.front().x;
	// 计算C0
	double ellipse_dcY = 0.0;
	for(unsigned int i = 0; i < seg_num; ++i)
	{
		double epsilon = 0.0;
		for(unsigned int j = 0; j < i; ++j)
		{
			epsilon += deltaY[j];
		}
		epsilon -= deltaY[i] / deltaT[i] * time_stamp[i];
		double single_item = deltaY[i] / (2.0 * deltaT[i]) * (time_stamp[i+1] * time_stamp[i+1] - time_stamp[i] * time_stamp[i]) + epsilon * (time_stamp[i+1] - time_stamp[i]);
		ellipse_dcY += single_item;
	}
	coef[0].C = ellipse_dcY / period + contour_points.front().y;
	// 计算a_n
	for(unsigned int i = 1; i < coef_num; ++i)
	{
		double accum = 0.0;
		for(unsigned int j = 0; j < seg_num; ++j)
		{
			accum += (deltaX[j] / deltaT[j] * (cos(assist_2npi[i] * time_stamp[j + 1] / period) - cos(assist_2npi[i] * time_stamp[j] / period)));
		}
		coef[i].A = accum * period / assist_2_nsq_pisq[i];
	}
	// 计算b_n
	for(unsigned int i = 1; i < coef_num; ++i)
	{
		double accum = 0.0;
		for(unsigned int j = 0; j < seg_num; ++j)
		{
			accum += (deltaX[j] / deltaT[j] * (sin(assist_2npi[i] * time_stamp[j + 1] / period) - sin(assist_2npi[i] * time_stamp[j] / period)));
		}
		coef[i].B = accum * period / assist_2_nsq_pisq[i];
	}
	// 计算c_n
	for(unsigned int i = 1; i < coef_num; ++i)
	{
		double accum = 0.0;
		for(unsigned int j = 0; j < seg_num; ++j)
		{
			accum += (deltaY[j] / deltaT[j] * (cos(assist_2npi[i] * time_stamp[j + 1] / period) - cos(assist_2npi[i] * time_stamp[j] / period)));
		}
		coef[i].C = accum * period / assist_2_nsq_pisq[i];
	}
	// 计算d_n
	for(unsigned int i = 1; i < coef_num; ++i)
	{
		double accum = 0.0;
		for(unsigned int j = 0; j < seg_num; ++j)
		{
			accum += (deltaY[j] / deltaT[j] * (sin(assist_2npi[i] * time_stamp[j + 1] / period) - sin(assist_2npi[i] * time_stamp[j] / period)));
		}
		coef[i].D = accum * period / assist_2_nsq_pisq[i];
	}

	return coef;
}

template<class PointType>
std::vector<PointType> EFCHandler::Decode(const std::vector<EFC>& coefficients, unsigned int reconstruct_coef_num, unsigned int point_num) const
{
	std::vector<PointType> contour(point_num);
	if(reconstruct_coef_num > coefficients.size())
	{
		return contour;
	}
	// 计算辅助变量
	CalculateAssitVaraible(reconstruct_coef_num);
	// 重建
	for(unsigned int i = 0; i < point_num; ++i)
	{
		double accumX = 0.0;
		double accumY = 0.0;
		for(unsigned int j = 1; j < reconstruct_coef_num; ++j)
		{
			accumX += coefficients[j].A * cos(assist_2npi[j] * (i + 1) / point_num) + coefficients[j].B * sin(assist_2npi[j] * (i + 1) / point_num);
			accumY += coefficients[j].C * cos(assist_2npi[j] * (i + 1) / point_num) + coefficients[j].D * sin(assist_2npi[j] * (i + 1) / point_num);
		}
		contour[i].x = (float)(coefficients[0].A + accumX);
		contour[i].y = (float)(coefficients[0].C + accumY);
	}

	return contour;
}


#endif