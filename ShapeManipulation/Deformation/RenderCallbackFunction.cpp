#include "..\CommonFunc.h"
#include "RenderCallbackFunction.h"
#include "ModelHandler.h"
#include "RenderManager.h"
#include "DeformFramework.h"

RenderManager *p_render_manager = RenderManager::getInstance();
DeformFramework *p_deform_framework = DeformFramework::getInstance();

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//printf("\nkey:%d, scancode:%d, action:%d, mode:%d", key, scancode, action, mode);
	if(action == GLFW_PRESS || action == GLFW_REPEAT)	// 按下，或长按
	{
		if (GLFW_KEY_ESCAPE == key)	// esc
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		else if(GLFW_KEY_R == key)	// reset
		{
			// 还原模型点与控制点
			p_deform_framework->model_handler_.Reset();
			// 还原滚轮放缩的影响
			p_render_manager->zoom_scaling_factor_ = glm::vec2(1.0, 1.0);
			glUniform2f(p_render_manager->zoom_scaling_factor_loc_, 1.0f, 1.0f);
		}
		else if(GLFW_KEY_W == key)
		{
			p_render_manager->zoom_scaling_factor_.y += (glm::vec2::value_type)0.02;
			glUniform2f(p_render_manager->zoom_scaling_factor_loc_, (float)p_render_manager->zoom_scaling_factor_.x, (float)p_render_manager->zoom_scaling_factor_.y);
		}
		else if(GLFW_KEY_S == key)
		{
			p_render_manager->zoom_scaling_factor_.y -= (glm::vec2::value_type)0.02;
			glUniform2f(p_render_manager->zoom_scaling_factor_loc_, (float)p_render_manager->zoom_scaling_factor_.x, (float)p_render_manager->zoom_scaling_factor_.y);
		}
		else if(GLFW_KEY_A == key)
		{
			p_render_manager->zoom_scaling_factor_.x -= (glm::vec2::value_type)0.02;
			glUniform2f(p_render_manager->zoom_scaling_factor_loc_, (float)p_render_manager->zoom_scaling_factor_.x, (float)p_render_manager->zoom_scaling_factor_.y);
		}
		else if(GLFW_KEY_D == key)
		{
			p_render_manager->zoom_scaling_factor_.x += (glm::vec2::value_type)0.02;
			glUniform2f(p_render_manager->zoom_scaling_factor_loc_, (float)p_render_manager->zoom_scaling_factor_.x, (float)p_render_manager->zoom_scaling_factor_.y);
		}
		else if(GLFW_KEY_V == key)
		{
			// 设置控制点是否显示
			p_deform_framework->control_points_visible_ = !p_deform_framework->control_points_visible_;
		}
	}

}

void mouse_btn_callback(GLFWwindow* window, int button, int action, int mods)
{
	if(GLFW_MOUSE_BUTTON_LEFT == button)
	{
		if(mods & GLFW_MOD_CONTROL)		// 按下Ctrl键，操作控制点
		{
			// 按下时，添加或删除控制点。行为取决于按下的位置。
			if(GLFW_PRESS == action)
			{
				// ctrl + left btn
				double pixel_xpos, pixel_ypos;
				glfwGetCursorPos(window, &pixel_xpos, &pixel_ypos);
				//printf("\n%g %g", pixel_xpos, pixel_ypos);
				glm::vec2 control_point_pos = PixelPos2PointPos(glm::vec2(pixel_xpos, pixel_ypos));
				//printf("\n%g %g", control_point_pos.x, control_point_pos.y);
				p_deform_framework->model_handler_.SetControlPoints(control_point_pos);
			}
		}
		else
		{
			if(GLFW_PRESS == action)
			{
				// left btn press
				double pixel_xpos, pixel_ypos;
				glfwGetCursorPos(window, &pixel_xpos, &pixel_ypos);
				glm::vec2 control_point_pos = PixelPos2PointPos(glm::vec2(pixel_xpos, pixel_ypos));
				p_deform_framework->model_handler_.PickupPoint(control_point_pos);
			}
			else
			{
				// left btn release
				p_deform_framework->model_handler_.ReleasePoint();
			}
		}
	}
}

void cursor_pos_callback(GLFWwindow* window, double pixel_xpos, double pixel_ypos)
{
	glm::vec2 control_point_pos = PixelPos2PointPos(glm::vec2(pixel_xpos, pixel_ypos));
	p_deform_framework->model_handler_.ControlPointMoving(control_point_pos);
}

void fbsize_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height); 
	
	p_render_manager->current_window_width_ = width;
	p_render_manager->current_window_height_ = height;
	
	//p_render_manager->window_scaling_factor_.x = p_render_manager->orig_window_width_ / (double)width;	// 横向按照原始窗口的宽度为单位1进行调整
	p_render_manager->window_scaling_factor_.x = (glm::vec2::value_type)(p_render_manager->orig_window_height_ / (double)width);	// 无论横向还是纵向，都按照原始窗口的高度为单位1进行调整
	p_render_manager->window_scaling_factor_.y = (glm::vec2::value_type)(p_render_manager->orig_window_height_ / (double)height);
	glUniform2f(p_render_manager->window_scaling_factor_loc, (float)p_render_manager->window_scaling_factor_.x, (float)p_render_manager->window_scaling_factor_.y);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//printf("\n%g %g ", xoffset, yoffset);	// 鼠标滚轮只有y有值，向上滚为正。滚一小下是1，幅度加大后是2、3，很难达到4
	//zoom_scaling_factor_ += 0.1 * yoffset;
	//if(zoom_scaling_factor_.x < 0)
	//{
	//	zoom_scaling_factor_.x = 0.0;
	//	zoom_scaling_factor_.y = 0.0;
	//}
	p_render_manager->zoom_scaling_factor_ *= 1.0 + 0.1 * yoffset;	// 这种体验更好，尤其在放大的时候
	glUniform2f(p_render_manager->zoom_scaling_factor_loc_, (float)p_render_manager->zoom_scaling_factor_.x, (float)p_render_manager->zoom_scaling_factor_.y);
}

glm::vec2 PixelPos2PointPos(const glm::vec2& pixel_pos)
{
	// 先从像素坐标转换为位于[-1, 1]区间内的窗口坐标
	glm::vec2 screen_pos(2.0 * pixel_pos.x / p_render_manager->current_window_width_ - 1.0, 1.0 - 2.0 * pixel_pos.y / p_render_manager->current_window_height_);
	// 由于点的窗口坐标的转换过程是：窗口坐标 = 原始位置 * 数据缩放系数 * 窗口缩放系数 * 滚轮缩放系数
	// 故现在逆回去，根据窗口坐标逆推点的原始坐标
	glm::vec2 point_pos = screen_pos / ((glm::vec2::value_type)p_deform_framework->model_handler_.getScalingFactor() * p_render_manager->window_scaling_factor_ * p_render_manager->zoom_scaling_factor_);
	return point_pos;
	//return screen_pos;
}

