#ifndef MODELHANDLER_H
#define MODELHANDLER_H

#include <GL/glew.h>
#include <vector>
#include <string>
#include <time.h>
#include "..\local_glm.h"

class ModelHandler
{
public:
	enum RefreshType{NONE = 0x0000, ModelPointRefresh = 0x0001, ControlPointRefresh = 0x0002};
	enum ControlPointType{CENTER_POINT_IDX = 0, HEAD_POINT_IDX = 1, TAIL_POINT_IDX = 2, LEFT_FIN_IDX = 3, RIGHT_FIN_IDX = 4};

	ModelHandler();
	void					ImportModel(const std::string& file_prefix);
	void					ImportModel(const std::vector<glm::vec2>& vertices, const std::vector<glm::ivec3>& triangles);
	void					ImportModel(const std::vector<glm::vec2>& vertices, 
										const std::vector<glm::ivec3>& triangles,
										const std::vector<glm::vec2>& tex_coords
										);
	double					getScalingFactor() const;
	std::vector<glm::ivec3>&getTriangleIndex();
	std::vector<glm::vec2>& getRigidDeformResult();
	std::vector<glm::vec2>& getControlPoints();
	std::vector<glm::vec2>& getTexCoords();
	void					SetControlPoints(glm::vec2 point_pos);
	void					PickupPoint(glm::vec2 point_pos);
	void					ReleasePoint();
	void					ControlPointMoving(glm::vec2 point_pos);
	int						NeedRefresh() const;
	void					Refresh(int refresh_type);
	void					Reset();

private:
	void					ReadNode(const std::string& filename, std::vector<glm::vec2>& model_points) const;
	void					ReadTriangle(const std::string& filename, std::vector<glm::ivec3>& triangle_index) const;
	double					NormalizeModel();
	double					CalcScalingFactor(const std::vector<glm::vec2>& model_points) const;
	void					PreComputationAtControlPoint();
	void					DeformByControlPoints();
	std::vector<glm::vec2>::const_iterator 
							getAttachedControlPoint(const glm::vec2& point_pos, double influence_box_halflen) const;
	glm::vec2				PixelPos2PointPos(const glm::vec2& pixel_pos) const;

	std::vector<glm::vec2>	original_model_points_;		// 读入的模型数据
	std::vector<glm::ivec3>	triangle_index_;			// 模型的三角形顶点索引
	std::vector<glm::vec2>	tex_coords_;				// 顶点的纹理坐标
	std::vector<glm::vec2>	reference_model_points_;	// 当前变换的参照模型数据
	std::vector<glm::vec2>	rigid_deform_result_;
	std::vector<glm::vec2>	control_points_;
	double					factor_;
	bool					is_picked_;
	int						picked_point_idx_;
	int						need_refresh_;

	double					controlpoint_influence_range_;	// 控制点的响应范围，在范围内的鼠标单击会被这个控制点捕捉到，在ImportModel中，计算出factor_后初始化

	std::vector<std::vector<double>>	weight_;		// 每行代表一个model point，每列代表一个control point
	std::vector<glm::vec2>				p_asterisk_;	// 每行代表一个model point，两列分别代表横纵坐标
	std::vector<glm::vec2>				q_asterisk_;	// 每行代表一个model point，两列分别代表横纵坐标
	std::vector<std::vector<glm::vec2>> p_hat_;			// 每行代表一个model point，每2列代表一个control point
	std::vector<std::vector<glm::vec2>> q_hat_;			// 每行代表一个model point，每2列代表一个control point
	std::vector<std::vector<glm::mat2>> A_i_;			// 预计算的辅助变量（用于刚性变换）
	std::vector<double>					mu_s_;			// 预计算的辅助变量（用于相似变换）
};

//const double kControlBoxHalfLen = 0.8;
//const double kControlBoxHalfLen = 0.01;
//const double kInfluenceBoxHalfLen = kControlBoxHalfLen * 1.2;
const double ALPHA = 0.5;

inline glm::vec2 getPerpendicularVec(const glm::vec2& v)
{
	return glm::vec2(-v.y, v.x);
}

#endif