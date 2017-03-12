#include <opencv.hpp>
#include "..\local_glm.h"
#include "Meshing.h"
#include "triangle.h"
//#ifdef __cplusplus
//extern "C"{
//#define ANSI_DECLARATORS
//#include "triangle.h"
//#endif
//#ifdef __cplusplus
//}
//#endif

void OpenCVCoord2OpenGLCoord(const std::vector<cv::Point2d>& smooth_contour, std::vector<glm::vec2>& vertices, int img_height)
{
	unsigned int point_num = smooth_contour.size();
	vertices.resize(point_num);
	--img_height;	// 为了方便下面的减法运算
	for(unsigned int i = 0; i < point_num; ++i)
	{
		vertices[i].x = (glm::vec2::value_type)smooth_contour[i].x;
		vertices[i].y = (glm::vec2::value_type)(img_height - smooth_contour[i].y);
	}
}

void Meshing(const std::vector<glm::vec2>& smooth_contour, std::vector<glm::vec2>& vertices, std::vector<glm::ivec3>& triangles, char* commandLine)
{
	struct triangulateio in, out;
	/********************* in ************************/
	// points
	in.numberofpoints = smooth_contour.size();
	in.pointlist = (REAL *) malloc(in.numberofpoints * 2 * sizeof(REAL));
	for(int i = 0; i < in.numberofpoints; ++i)
	{
		in.pointlist[2 * i] = smooth_contour[i].x;
		in.pointlist[2 * i + 1] = smooth_contour[i].y;
	}
	in.numberofpointattributes = 0;
	in.pointattributelist = NULL;
	in.pointmarkerlist = NULL;
	// segment
	in.numberofsegments = smooth_contour.size();
	in.segmentlist = (int *) malloc(in.numberofpoints * 2 * sizeof(int));
	for(int i = 0; i < in.numberofpoints; ++i)
	{
		in.segmentlist[2 * i] = i;
		in.segmentlist[2 * i + 1] = (i + 1) % in.numberofpoints;
	}
	in.segmentmarkerlist = NULL;
	// hole
	in.numberofholes = 0;
	in.holelist = NULL;
	// region
	in.numberofregions = 0;
	in.regionlist = NULL;
	// triangle
	in.numberoftriangles = 0;
	in.trianglelist = NULL;
	in.numberoftriangleattributes = 0;
	in.triangleattributelist = NULL;
	in.numberofedges = 0;
	in.edgelist = NULL;
	/********************* out ***********************/
	out.pointlist = NULL;
	out.pointattributelist = NULL;
	out.pointmarkerlist = NULL;
	out.trianglelist = NULL;
	out.triangleattributelist = NULL;
	out.trianglearealist = NULL;
	out.neighborlist = NULL;
	out.segmentlist = NULL;
	out.segmentmarkerlist = NULL;
	out.edgelist = NULL;
	out.edgemarkerlist = NULL;
	/********************* vorout ***********************/
	// there is no need for vorout
	/********************* triangulate call ********************/
	triangulate(commandLine, &in, &out, NULL);
	// construct vertices and triangles
	vertices.resize(out.numberofpoints);
	for(int i = 0; i < out.numberofpoints; ++i)
	{
		vertices[i].x = (glm::vec2::value_type)out.pointlist[2 * i];
		vertices[i].y = (glm::vec2::value_type)out.pointlist[2 * i + 1];
	}
	triangles.resize(out.numberoftriangles);
	for(int i = 0; i < out.numberoftriangles; ++i)
	{
		triangles[i].x = (glm::ivec3::value_type)out.trianglelist[3 * i];
		triangles[i].y = (glm::ivec3::value_type)out.trianglelist[3 * i + 1];
		triangles[i].z = (glm::ivec3::value_type)out.trianglelist[3 * i + 2];
	}
}

// 计算顶点所对应的纹理坐标
// 注意这里的顶点的坐标应该是以图像左下角为原点的
void CalcTexCoord(const std::vector<glm::vec2>& vertices, std::vector<glm::vec2>& tex_coord, const cv::Size& img_size)
{
	unsigned int vertices_size = vertices.size();
	tex_coord.resize(vertices_size);
	for(unsigned int i = 0; i < vertices_size; ++i)
	{
		tex_coord[i].x = (glm::vec2::value_type)(vertices[i].x / img_size.width);
		tex_coord[i].y = (glm::vec2::value_type)(vertices[i].y / img_size.height);
	}
}

void RegularizeVertices(std::vector<glm::vec2>& vertices, glm::vec2 center, glm::vec2 major_axis)
{
	unsigned int vertices_num = vertices.size();
	std::vector<glm::vec2> orig_vertices = vertices;
	glm::vec2 centered_vertex;
	glm::vec2 last_regularized_vertex(0, 0);
	glm::vec2 minor_axis(major_axis.y, -major_axis.x);
	for(unsigned int i = 0; i < vertices_num; ++i)
	{
		centered_vertex = orig_vertices[i] - center;
		//vertices[i].x = centered_vertex.x * minor_axis.x + centered_vertex.y * minor_axis.y;
		//vertices[i].y = centered_vertex.x * major_axis.x + centered_vertex.y * major_axis.y;
		vertices[i] = centered_vertex;
		if(i > 0 && abs(vertices[i].x - last_regularized_vertex.x) + abs(vertices[i].y - last_regularized_vertex.y) < DBL_MIN)
		{
			perror("\nTwo identical contour point occuring, Why?");
			exit(-1);
		}
		last_regularized_vertex = vertices[i];
	}
}

void TranslateNormalize(std::vector<glm::vec2>& vertices, const glm::vec2& center)
{
	unsigned int vertices_size = vertices.size();
	for(unsigned int i = 0; i < vertices_size; ++i)
	{
		vertices[i] -= center;
	}
}

void RotateVertices(std::vector<glm::vec2>& vertices, RotateDir dir)
{
	unsigned vertices_size = vertices.size();
	std::vector<glm::vec2> assist_vec(vertices);
	if(CW == dir)
	{
		for(unsigned int i = 0; i < vertices_size ; ++i)
		{
			vertices[i].x = assist_vec[i].y;
			vertices[i].y = -assist_vec[i].x;
		}
	}
	else if(CCW == dir)
	{
		for(unsigned int i = 0; i < vertices_size ; ++i)
		{
			vertices[i].x = -assist_vec[i].y;
			vertices[i].y = assist_vec[i].x;
		}
	}
}