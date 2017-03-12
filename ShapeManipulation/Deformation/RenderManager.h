#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H
#include <iostream>
// GLEW
//#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// GLM
#include "..\local_glm.h"

class RenderManager
{
public:
	GLFWwindow *window_;
	GLuint orig_window_width_;
	GLuint orig_window_height_;
	int current_window_width_;
	int current_window_height_;

	GLuint shader_program_;
	GLint data_scaling_factor_loc_;
	GLint window_scaling_factor_loc;
	GLint zoom_scaling_factor_loc_;
	GLint color_loc_;
	GLint texture_enable_loc_;
	glm::vec2 window_scaling_factor_;
	glm::vec2 zoom_scaling_factor_;

	//double zoom_interval_;	// 表示每次滚动滚轮，或按下WSAD按钮时，放缩比例的增减量

	static RenderManager* getInstance()
	{
		static RenderManager interactive_manager;
		return &interactive_manager;
	}

	void CreateContextWindow(const char* const window_title, GLuint window_width, GLuint orig_window_height, int resizable);
	GLuint InitShader(const char* const vertex_shader_filename, const char* const fragment_shader_filename);

private:
	RenderManager()
		: window_(nullptr), orig_window_width_(0), orig_window_height_(0),
		shader_program_(0),
		data_scaling_factor_loc_(0), window_scaling_factor_loc(0), zoom_scaling_factor_loc_(0),
		window_scaling_factor_(1.0), zoom_scaling_factor_(1.0)
		//,zoom_interval_(0.1)
	{
		// Init GLFW
		glfwInit();
	}

};



#endif