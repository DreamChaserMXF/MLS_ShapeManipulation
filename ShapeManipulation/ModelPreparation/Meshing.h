#ifndef MESHING_H
#define MESHING_H

#include <vector>
#include <opencv.hpp>
#include "..\local_glm.h"

void OpenCVCoord2OpenGLCoord(const std::vector<cv::Point2d>& smooth_contour, 
							std::vector<glm::vec2>& vertices, 
							int img_height);

void Meshing(
	const std::vector<glm::vec2>& smooth_contour, 
	std::vector<glm::vec2>& vertices, 
	std::vector<glm::ivec3>& triangles, 
	char* commandLine = "pq30");

void CalcTexCoord(
	const std::vector<glm::vec2>& vertices, 
	std::vector<glm::vec2>& tex_coord, 
	const cv::Size& img_size);

void RegularizeVertices(std::vector<glm::vec2>& vertices, glm::vec2 center, glm::vec2 major_axis);

void TranslateNormalize(std::vector<glm::vec2>& vertices, 
	const glm::vec2& center);

enum RotateDir		// 旋转90°的方向
{		
	NONE, CW, CCW	// 不旋转、顺时针、逆时针
};
void RotateVertices(std::vector<glm::vec2>& vertices, RotateDir dir);

#endif