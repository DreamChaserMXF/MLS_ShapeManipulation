#include "DataProvider.h"
#include "ContourDetector.h"
#include "FileUtil.hpp"
#include "Meshing.h"

void DataProvider::AcquireModelData(const std::string& img_path)
{
	ContourDetector detector("edge", "contour");
	detector.findContour(img_path);
	std::vector<cv::Point2d> smooth_contour = detector.getSmoothContour();
	if(smooth_contour.size() > 1)
	{
		std::string poly_filename = img_path.substr(0, img_path.find_last_of('.')) + ".poly";
		MakePolyFile(smooth_contour, poly_filename);	// ��������㵽�ļ����Ա������֤
	}
	else
	{
		fprintf(stderr, "\nno contour found!");
		exit(EXIT_FAILURE);
	}
	// ��OpenCV������ϵͳת��OpenGL������ϵͳ��OpenCV���������Ͻ�Ϊԭ�㣬OpenGL�������½�Ϊԭ�㣩
	cv::Size img_size = detector.getResizedImgSize();
	OpenCVCoord2OpenGLCoord(smooth_contour, vertices_, img_size.height);
	cv::Point2d contour_area_center = detector.center2d_;
	cv::Point2d major_axis = detector.y_axis_;
	glm::vec2 center_in_gl = glm::vec2(contour_area_center.x, img_size.height - contour_area_center.y);
	glm::vec2 major_axis_in_gl = glm::vec2(major_axis.x, -major_axis.y);
	// ����
	std::vector<glm::ivec3> triangles;
	Meshing(vertices_, vertices_, triangles_, "pq30zQ");
	// ������������
	CalcTexCoord(vertices_, tex_coord_, img_size);
	// �Զ������ƽ�ƹ�һ������ת��������
	RegularizeVertices(vertices_, center_in_gl, major_axis_in_gl);
	// ��¼ͷβ��������λ��
	head_idx_ = detector.head_idx_;
	tail_idx_ = detector.tail_idx_;
	left_fin_idx_ = detector.left_fin_idx_;
	right_fin_idx_ = detector.right_fin_idx_;
}