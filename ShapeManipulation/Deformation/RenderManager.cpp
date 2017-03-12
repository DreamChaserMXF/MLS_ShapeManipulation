#include "RenderManager.h"

// 局部函数
static GLuint initShader(const GLenum shaderType, const char *shaderPath);
static GLuint initProgram(GLuint vertexShader, GLuint fragmentShader);
static void shaderErrors(const GLuint shader);
static void programErrors(const GLuint program);
static char* readFile(const char *fileName);

void RenderManager::CreateContextWindow(const char* const window_title, GLuint window_width, GLuint orig_window_height, int resizable)
{
	orig_window_width_ = window_width;
	orig_window_height_ = orig_window_height;
	window_scaling_factor_.x = (double)orig_window_height_ / orig_window_width_;
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);	// 可以同时使用固定管线与可编程管线
	glfwWindowHint(GLFW_RESIZABLE, resizable);
	// Create window
	window_ = glfwCreateWindow(orig_window_width_, orig_window_height_, window_title, nullptr, nullptr);
	glfwMakeContextCurrent(window_);
	// Define the viewport configuration
	int width, height;
	glfwGetFramebufferSize(window_, &width, &height);  
	glViewport(0, 0, width, height);
	// 记录与窗口相关的参数
	current_window_width_ = width;
	current_window_height_ = height;
	window_scaling_factor_.x = (double)height / width;
}

GLuint RenderManager::InitShader(const char* const vertex_shader_filename, const char* const fragment_shader_filename)
{
	GLuint vertexShader = initShader(GL_VERTEX_SHADER, vertex_shader_filename);
	GLuint fragmentShader = initShader(GL_FRAGMENT_SHADER, fragment_shader_filename);
    shader_program_ = initProgram(vertexShader, fragmentShader);
	glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
	// 获取Uniform变量位置
	data_scaling_factor_loc_ = glGetUniformLocation(shader_program_, "dataScalingFactor");
	window_scaling_factor_loc = glGetUniformLocation(shader_program_, "windowScalingFactor");
	zoom_scaling_factor_loc_ = glGetUniformLocation(shader_program_, "zoomScalingFactor");
	color_loc_ = glGetUniformLocation(shader_program_, "myColor");
	texture_enable_loc_ = glGetUniformLocation(shader_program_, "enableTexture");
	return shader_program_;
}


// print shader error info log
static void shaderErrors(const GLuint shader)
{
	GLint length = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
	char *info = new char[length + 1];
	glGetShaderInfoLog(shader, length * sizeof(GLchar), &length, info);
	std::cerr << "error info: " << info << std::endl;
	delete info;	
}

// print program error info log
static void programErrors(const GLuint program)
{
	GLint length = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
	char *info = new char[length + 1];
	glGetProgramInfoLog(program, length * sizeof(GLchar), &length, info);
	std::cerr << "error info: " << info << std::endl;
	delete info;
}

// return pointer for the read string
// NULL for file opening error
static char* readFile(const char *fileName)
{
	FILE *pfile = fopen(fileName, "rb");
	if(!pfile)
	{
		return NULL;
	}

	fseek(pfile, 0L, SEEK_END);
	int len = ftell(pfile);
	char *str = new char[len + 1];

	fseek(pfile, 0L, SEEK_SET);
	fread(str, len, 1, pfile);
	str[len] = '\0';

	return str;
}

// throw 1 for error
// positive number for shader pointer
static GLuint initShader(const GLenum shaderType, const char *shaderPath)
{
	GLuint shader = glCreateShader(shaderType);
	const char *source = readFile(shaderPath);
	if(!source)
	{
		std::cerr << "cannot read the shader source from: \"" << shaderPath << "\"" << std::endl;
		throw 1;
	}
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);
	GLint compiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if(!compiled)
	{
		std::cerr << "cannot compile the shader \"" << shaderPath << "\"" << std::endl;
		shaderErrors(shader);
		glDeleteShader(shader);
		throw 1;
	}
	return shader;
}

// throw 2 for error
// positive number for program pointer
static GLuint initProgram(GLuint vertexShader, GLuint fragmentShader)
{
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	// check for link
	GLint linked = 0;
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if(!linked)
	{
		std::cerr << "cannot link shader " << std::endl;
		programErrors(program);
		glDeleteProgram(program);
		throw 2;
	}
	// check for validate
	GLint validated;
	glValidateProgram(program);
	glGetProgramiv(program, GL_VALIDATE_STATUS, &validated);
	if(!validated)
	{
		std::cerr << "cannot validate program" << std::endl;
		programErrors(program);
		glDeleteProgram(program);
		throw 2;
	}
	//glUseProgram(program);
	return program;
}