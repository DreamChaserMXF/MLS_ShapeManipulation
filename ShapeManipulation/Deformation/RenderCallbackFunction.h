#ifndef RENDERING_CALLBACK_FUNCTION_H
#define RENDERING_CALLBACK_FUNCTION_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_btn_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
void fbsize_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
glm::vec2 PixelPos2PointPos(const glm::vec2& pixel_pos);

#endif