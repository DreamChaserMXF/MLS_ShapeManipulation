#include <Windows.h>
#include "DeformFramework.h"
#include "RenderCallbackFunction.h"

void DeformFramework::RenderSetUp(const std::string& window_name, int window_width, int window_height, 
				const std::string& vertex_shader_file, const std::string& fragment_shader_file)
{
	RenderManager *p_render_manager = RenderManager::getInstance();
	/******************************************************************************************/
	/**************** Window *****************************************************************/
	int width = 300, height = 600;
	p_render_manager->CreateContextWindow("Fish Model", width, height, true);
	/******************************************************************************************/
	/**************** Callback Function ******************************************************/
	glfwSetKeyCallback(p_render_manager->window_, key_callback);
	glfwSetMouseButtonCallback(p_render_manager->window_, mouse_btn_callback);
	glfwSetCursorPosCallback(p_render_manager->window_, cursor_pos_callback);
	glfwSetFramebufferSizeCallback(p_render_manager->window_, fbsize_callback);
	glfwSetScrollCallback(p_render_manager->window_, scroll_callback);
	/******************************************************************************************/
	/**************** Glew *******************************************************************/
    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	// 必须要在创建窗口后、初始化shader前，初始化glew
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
	if(GLEW_OK != err)
	{
		std::cerr << "Error: " << glewGetString(err) << std::endl;
		exit(EXIT_FAILURE);
	}
	/******************************************************************************************/
	/**************** Shader *****************************************************************/
	p_render_manager->InitShader(vertex_shader_file.c_str(), fragment_shader_file.c_str());
}

void DeformFramework::DataSetUp(
	const std::vector<glm::vec2>& vertices, 
	const std::vector<glm::ivec3>& triangles,
	const std::vector<glm::vec2>& tex_coords,
	const std::string& texture_filename
	)
{
	RenderManager *p_render_manager = RenderManager::getInstance();
	/******************************************************************************************/
	/**************** MODEL DATA **************************************************************/
	// 读取轮廓数据
	model_handler_.ImportModel(vertices, triangles, tex_coords);
	//model_handler_.ImportModel(vertices, triangles);
	//model_handler_.ImportModel("Model/modelS");
	double scaling_factor = model_handler_.getScalingFactor();
	glUseProgram(p_render_manager->shader_program_);	// 设置uniform变量前必须要使用该shader
	glUniform2f(p_render_manager->data_scaling_factor_loc_, (float)scaling_factor, (float)scaling_factor);	// scaling factor setting
	glUniform2f(p_render_manager->window_scaling_factor_loc, (float)p_render_manager->window_scaling_factor_.x, (float)p_render_manager->window_scaling_factor_.y);	// scaling factor setting
	glUniform2f(p_render_manager->zoom_scaling_factor_loc_, (float)p_render_manager->zoom_scaling_factor_.x, (float)p_render_manager->zoom_scaling_factor_.y);		// scaling factor setting
	/******************************************************************************************/
	/**************** VAO VBO EBO ***********************************************************/
	GLenum type = 4 == sizeof(model_handler_.getRigidDeformResult()[0]) ? GL_FLOAT : GL_DOUBLE;
    glGenVertexArrays(2, VAOs);
    glGenBuffers(2, VBOs);
    glGenBuffers(1, &EBO);
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAOs[MODEL_POINT]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[MODEL_POINT]);
	// 这里把顶点和纹理坐标一起绑定到VBO[MODEL_POINT]上
	// 先是所有的顶点坐标，接着是所有的纹理坐标
	// 在变形时，只需要调用glBufferSubData更改顶点坐标即可
	unsigned int vertices_num = model_handler_.getRigidDeformResult().size();
	std::vector<glm::vec2> vertices_and_texcoords = model_handler_.getRigidDeformResult();
	vertices_and_texcoords.insert(vertices_and_texcoords.cend(), model_handler_.getTexCoords().begin(), model_handler_.getTexCoords().end());
	glBufferData(GL_ARRAY_BUFFER, vertices_and_texcoords.size() * sizeof(vertices_and_texcoords[0]), vertices_and_texcoords.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, type, GL_FALSE, sizeof(vertices_and_texcoords[0]), (GLvoid*)0);
    glVertexAttribPointer(1, 2, type, GL_FALSE, sizeof(vertices_and_texcoords[0]), (GLvoid*)(vertices_num * sizeof(vertices_and_texcoords[0])));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model_handler_.getTriangleIndex().size() * sizeof(model_handler_.getTriangleIndex()[0]), model_handler_.getTriangleIndex().data(), GL_STATIC_DRAW);
    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(VAOs[CONTROL_POINT]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[CONTROL_POINT]);
	glBufferData(GL_ARRAY_BUFFER, model_handler_.getControlPoints().size() * sizeof(model_handler_.getControlPoints()[0]), model_handler_.getControlPoints().data(), GL_DYNAMIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, model_handler_.getControlBoxes().size() * sizeof(model_handler_.getControlBoxes()[0]), model_handler_.getControlBoxes().data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, type, GL_FALSE, sizeof(model_handler_.getControlPoints()[0]), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);
	/******************************************************************************************/
	/**************** TEXTURE **************************************************************/
	TextureManager *p_texture_manage = TextureManager::Inst();
	p_texture_manage->LoadTexture(texture_filename.c_str(), tid_, NULL, NULL, GL_BGR);
	p_texture_manage->BindTexture(tid_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void DeformFramework::LoopBegin()
{
	RenderManager *p_render_manager = RenderManager::getInstance();
	unsigned int vertices_num = model_handler_.getRigidDeformResult().size();

	//glPolygonMode(GL_BACK, GL_FILL);	
 //   glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_FRONT, GL_FILL);	
    glPolygonMode(GL_BACK, GL_LINE);
	glPointSize(8);
	glLineWidth(1);
	int i = 0;
	printf("\nusage:");
	printf("\n'ctrl + left button' -- set control point.");
	printf("\n'scroll up/down' -- zoom in/out.");
	printf("\n'w' -- highten the model.");
	printf("\n's' -- shorten the model.");
	printf("\n'a' -- thin down the model.");
	printf("\n'd' -- flatten the model.");
    while (!glfwWindowShouldClose(p_render_manager->window_))
    {
		++i;
		//printf("\n%d", i);
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // ***********************Start Render****************************

        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
		// Set shader
		glUseProgram(p_render_manager->shader_program_);

		// TODO Draw Picture

        // Draw Model Points
	    glBindVertexArray(VAOs[MODEL_POINT]);
		if(model_handler_.NeedRefresh() & ModelHandler::ModelPointRefresh)
		{
			glBindBuffer(GL_ARRAY_BUFFER, VBOs[MODEL_POINT]);
			//glBufferData(GL_ARRAY_BUFFER, model_handler_.getRigidDeformResult().size() * sizeof(model_handler_.getRigidDeformResult()[0]), model_handler_.getRigidDeformResult().data(), GL_STREAM_DRAW);
			glBufferSubData(GL_ARRAY_BUFFER, 0, model_handler_.getRigidDeformResult().size() * sizeof(model_handler_.getRigidDeformResult()[0]), model_handler_.getRigidDeformResult().data());
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// update state
			model_handler_.Refresh(ModelHandler::ModelPointRefresh);
		}
		//glUniform4f(p_render_manager->color_loc_, 1.0f, 0.5f, 0.2f, 1.0f);	// color setting
		glUniform1i(p_render_manager->texture_enable_loc_, 1);					// enable texture
		glDrawElements(GL_TRIANGLES, model_handler_.getTriangleIndex().size() * 3, GL_UNSIGNED_INT, nullptr);
		//glDrawArrays(GL_POINTS, 0, model_handler_.getRigidDeformResult().size());
		//glDrawArrays(GL_LINE_LOOP, 0, model_handler_.getRigidDeformResult().size());
		//glDrawArrays(GL_POLYGON, 0, model_handler_.getRigidDeformResult().size());

		// Draw Control Points
		if(control_points_visible_)
		{
			glBindVertexArray(VAOs[CONTROL_POINT]);
			if(model_handler_.NeedRefresh() & ModelHandler::ControlPointRefresh)
			{
				glBindBuffer(GL_ARRAY_BUFFER, VBOs[CONTROL_POINT]);
				glBufferData(GL_ARRAY_BUFFER, model_handler_.getControlPoints().size() * sizeof(model_handler_.getControlPoints()[0]), model_handler_.getControlPoints().data(), GL_STREAM_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				//printf("\n%g %g", model_handler_.getControlPoints().back().x, model_handler_.getControlPoints().back().y);
				// update state
				model_handler_.Refresh(ModelHandler::ControlPointRefresh);
			}
			glUniform4f(p_render_manager->color_loc_, 0.0f, 0.5f, 1.0f, 1.0f);	// color setting
			glUniform1i(p_render_manager->texture_enable_loc_, 0);				// disable texture
			glDrawArrays(GL_POINTS, 0, model_handler_.getControlPoints().size());
		}
		//printf("\n visible: %d", control_points_visible_);
		glBindVertexArray(0);

        // Swap the screen buffers
		glfwSwapBuffers(p_render_manager->window_);
		//Sleep(1000);
		//_sleep(1000);
		//printf("a");
    }
	glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);
    glDeleteBuffers(1, &EBO);

	glDeleteProgram(p_render_manager->shader_program_);
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
}