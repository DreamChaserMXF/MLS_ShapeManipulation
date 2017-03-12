#ifndef DEFORMFRAMEWORK_H
#define DEFORMFRAMEWORK_H
#include <string>
#include <vector>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include "..\local_glm.h"
#include "RenderManager.h"
#include "TextureManager.h"
#include "ModelHandler.h"

class DeformFramework
{
public:
	static DeformFramework *getInstance()
	{
		static DeformFramework deform_framework;
		return &deform_framework;
	}
	

	void RenderSetUp(const std::string& window_name, int window_width, int window_height, 
		const std::string& vertex_shader_file, const std::string& fragment_shader_file);

	void DataSetUp(
		const std::vector<glm::vec2>& vertices, 
		const std::vector<glm::ivec3>& triangles,
		const std::vector<glm::vec2>& tex_coords,
		const std::string& texture_filename
		);

	void LoopBegin();

	ModelHandler	model_handler_;

	bool			control_points_visible_;

private:
	enum{MODEL_POINT = 0, CONTROL_POINT = 1};

	DeformFramework() : control_points_visible_(true), tid_(0){}

	unsigned int	tid_;
	GLuint			VAOs[2];
	GLuint			VBOs[2];
	GLuint			EBO;

};

#endif