#include <stdio.h>
#include <algorithm>
#include "..\CommonFunc.h"
#include "ModelHandler.h"

ModelHandler::ModelHandler() 
	: original_model_points_(), triangle_index_(), reference_model_points_(), rigid_deform_result_(), 
	control_points_(), 
	factor_(1.0), is_picked_(false), picked_point_idx_(-1), need_refresh_(ModelPointRefresh | ControlPointRefresh), 
	weight_(), p_asterisk_(), q_asterisk_(), p_hat_(), q_hat_(),
	controlpoint_influence_range_(0.0)
{}

void ModelHandler::ImportModel(const std::string& file_prefix)
{
	ReadNode(file_prefix + ".node", original_model_points_);
	ReadTriangle(file_prefix + ".ele", triangle_index_);
	reference_model_points_ = original_model_points_;
	rigid_deform_result_ = reference_model_points_;
	factor_ = CalcScalingFactor(original_model_points_);
	controlpoint_influence_range_ = 2.0 / factor_ * 0.01;	// ԼΪģ�͸߶ȵ�1%
}

void ModelHandler::ImportModel(const std::vector<glm::vec2>& vertices, const std::vector<glm::ivec3>& triangles)
{
	original_model_points_ = vertices;
	triangle_index_ = triangles;

	reference_model_points_ = original_model_points_;
	rigid_deform_result_ = reference_model_points_;
	factor_ = CalcScalingFactor(original_model_points_);
	controlpoint_influence_range_ = 2.0 / factor_ * 0.01;	// ԼΪģ�͸߶ȵ�1%
}

void ModelHandler::ImportModel(const std::vector<glm::vec2>& vertices, 
								const std::vector<glm::ivec3>& triangles,
								const std::vector<glm::vec2>& tex_coords)
{
	original_model_points_ = vertices;
	triangle_index_ = triangles;
	tex_coords_ = tex_coords;
	reference_model_points_ = original_model_points_;
	rigid_deform_result_ = reference_model_points_;
	factor_ = CalcScalingFactor(original_model_points_);
	controlpoint_influence_range_ = 2.0 / factor_ * 0.01;	// ԼΪģ�͸߶ȵ�1%
}


std::vector<glm::ivec3>& ModelHandler::getTriangleIndex()
{
	return triangle_index_;
}

std::vector<glm::vec2>& ModelHandler::getRigidDeformResult()
{
	return rigid_deform_result_;
}

std::vector<glm::vec2>& ModelHandler::getControlPoints()
{
	return control_points_;
}

std::vector<glm::vec2>& ModelHandler::getTexCoords()
{
	return tex_coords_;
}

void ModelHandler::SetControlPoints(glm::vec2 point_pos)
{
	// �Ƚ�һ�������п��Ƶ�ľ���
	std::vector<glm::vec2>::const_iterator c_iter = getAttachedControlPoint(point_pos, controlpoint_influence_range_);
	if(c_iter != control_points_.cend())	// �����п��Ƶ���������ɾ���������Ŀ��Ƶ�
	{
		int control_point_idx = std::distance(control_points_.cbegin(), c_iter);
		control_points_.erase(c_iter);
	}
	else	// ���һ���µĿ��Ƶ�����ƺ�
	{
		control_points_.push_back(point_pos);
		//printf("\n%g %g", point_pos.x, point_pos.y);
	}
	// ����ǰ��״����Ϊ�ο���״
	reference_model_points_ = rigid_deform_result_;	
	// Ԥ����
	PreComputationAtControlPoint();
	// ����ˢ��λ
	need_refresh_ |= ControlPointRefresh;
}

void ModelHandler::PickupPoint(glm::vec2 point_pos)
{
	if(control_points_.size() > 0)
	{
		// �Ƚ�һ�������п��Ƶ�ľ���
		std::vector<glm::vec2>::const_iterator c_iter = getAttachedControlPoint(point_pos, controlpoint_influence_range_);
		if(c_iter == control_points_.cend())	// �����п��Ƶ�����Զ
		{
			printf("\ntoo far from existing control points.");
		}
		else	// ���һ���µĿ��Ƶ�
		{
			is_picked_ = true;
			picked_point_idx_ = std::distance(control_points_.cbegin(), c_iter);;
			//printf("\npicked No.%d control point", picked_point_idx_);
		}
	}
	else
	{
		printf("\nplease add control point first.");
	}
}




// ֻ�����������µ�ʱ�򣬿��Ƶ�������ָ���ƶ��������������Ƶ��ƶ�
void ModelHandler::ControlPointMoving(glm::vec2 point_pos)
{
	//printf("\n%g %g", xpos, ypos);
	if(is_picked_)
	{
		//printf("\nMoving");
		need_refresh_ |= ControlPointRefresh;
		need_refresh_ |= ModelPointRefresh;
		// ���Ƶ���ƶ�
		control_points_[picked_point_idx_] = point_pos;
		// ������ı���
		//clock_t t0 = clock();
		DeformByControlPoints();
		//printf(" %dms ", clock() - t0);
	}
	else
	{
		//printf("\nHolding");
	}
}

void ModelHandler::ReleasePoint()
{
	//printf("\nRelease");
	is_picked_ = false;
}


int ModelHandler::NeedRefresh() const
{
	return need_refresh_;
}
void ModelHandler::Refresh(int refresh_type)
{
	need_refresh_ &= (~refresh_type);
}

void ModelHandler::Reset()
{
	// ����ģ�Ͷ���
	reference_model_points_ = original_model_points_;
	rigid_deform_result_ = reference_model_points_;
	// ɾ�����Ƶ㣬���ڶ�ɾ���ˣ��ʲ���Ҫ����Ԥ����
	control_points_.clear();
	// ����ˢ��λ
	need_refresh_ |= ModelPointRefresh;
	need_refresh_ |= ControlPointRefresh;
}

// �ļ���һ�б��� �㼯���� ά�� �������� �߽��־
// ������ÿ��Ϊ��<������> <������꣨x��y���ո�ָ���> [����] [�߽��־]
void ModelHandler::ReadNode(const std::string& filename, std::vector<glm::vec2>& model_points) const
{
	FILE *pfile = fopen(filename.c_str(), "r");
	if(!pfile)
	{
		perror("node File Cannot Be Found!");
		exit(EXIT_FAILURE);
	}
	int point_count;
	int dimension, attribute, boundary_marker;

	fscanf(pfile, "%u %u %u %u", &point_count, &dimension, &attribute, &boundary_marker);
	model_points.resize(point_count);
	const char* format_str = sizeof(glm::vec2::value_type) == 4 ? "%*d %f %f %*d" : "%*d %lf %lf %*d";
	for(int i = 0; i < point_count; ++i)
	{
		fscanf(pfile, format_str, &model_points[i].x, &model_points[i].y);
		model_points[i].y = -model_points[i].y;	// y�ᷭתһ�£����������������������
	}

	fclose(pfile);
}

// �ļ���һ�б��� ���������� ά��(��Ϊ3) ��������
// ������ÿ��Ϊ��<�����ε����> <������ı��> [����] [�߽��־]
// ע�⣺.ele�ļ��е������ζ������Ǵ�1��ʼ�ģ���OpenGL����Ҫ��0��ʼ�ı�ţ�����Ҫ���⴦��һ�¡�
void ModelHandler::ReadTriangle(const std::string& filename, std::vector<glm::ivec3>& triangle_index) const
{
	FILE *pfile = fopen(filename.c_str(), "r");
	if(!pfile)
	{
		perror("ele File Cannot Be Found!");
		exit(EXIT_FAILURE);
	}
	int triangle_count, dimension, attribute;

	fscanf(pfile, "%u %u %u", &triangle_count, &dimension, &attribute);
	triangle_index.resize(triangle_count);
	const char* format_str = "%*d %d %d %d";
	bool zero_idx_exist = false;
	for(int i = 0; i < triangle_count; ++i)
	{
		fscanf(pfile, format_str, &triangle_index[i].x, &triangle_index[i].y, &triangle_index[i].z);
		if(!zero_idx_exist && 
			(0 == triangle_index[i].x || 0 == triangle_index[i].y || 0 == triangle_index[i].z))
		{
			zero_idx_exist = true;
		}
	}
	if(!zero_idx_exist)	// ˵���������Ǵ�1��ʼ��
	{
		for(int i = 0; i < triangle_count; ++i)
		{
			triangle_index[i] -= 1;
		}
	}

	fclose(pfile);
}

double ModelHandler::getScalingFactor() const
{
	return factor_;
}

double ModelHandler::CalcScalingFactor(const std::vector<glm::vec2>& model_points) const
{
	std::vector<glm::vec2>::const_iterator max_iter = std::max_element(model_points.begin(), model_points.end(), max_val_less_than);
	if(model_points.end() == max_iter)
	{
		perror("Non Data in Model");
		exit(EXIT_FAILURE);
	}
	double denominator = 1.2 * (abs(max_iter->x) > abs(max_iter->y) ? abs(max_iter->x) : abs(max_iter->y));
	if(denominator < 1e-3f)
	{
		denominator = 1e-3f;
	}
	return 1.0 / denominator;
}

double ModelHandler::NormalizeModel()
{
	std::vector<glm::vec2>::iterator max_iter = std::max_element(reference_model_points_.begin(), reference_model_points_.end(), max_val_less_than);
	if(reference_model_points_.end() == max_iter)
	{
		perror("Non Data in Model");
		exit(EXIT_FAILURE);
	}
	double denominator = 1.2 * (abs(max_iter->x) > abs(max_iter->y) ? abs(max_iter->x) : abs(max_iter->y));
	if(denominator < 1e-3f)
	{
		denominator = 1e-3f;
	}
	for(std::vector<glm::vec2>::iterator iter = reference_model_points_.begin(); iter != reference_model_points_.end(); ++iter)
	{
		*iter /= denominator;
	}
	return 1.0 / denominator;
}


// ���/ɾ�����Ƶ���Ԥ����
void ModelHandler::PreComputationAtControlPoint()
{
	unsigned int model_point_count = reference_model_points_.size();
	unsigned int control_point_count = control_points_.size();
	// ����weight_
	weight_.resize(model_point_count);
	for(unsigned int i = 0; i < model_point_count; ++i)
	{
		weight_[i].resize(control_point_count);
		double sum_of_weight = 0.0;
		for(unsigned int j = 0; j < control_point_count; ++j)
		{
			glm::vec2 subtraction = reference_model_points_[i] - control_points_[j];
			double distance = subtraction.x * subtraction.x + subtraction.y * subtraction.y;	// Ϊɶglm::dotֻ����floating point?
			if(distance < DBL_MIN)	// ����С�ľ���Ĵ���
			{
				distance = DBL_MIN;
			}
			weight_[i][j] = 1.0 / pow(distance, ALPHA);
			sum_of_weight += weight_[i][j];
		}
		// ��һ��
		for(unsigned int j = 0; j < control_point_count; ++j)
		{
			weight_[i][j] /= sum_of_weight;
		}
		// ������СȨ�أ��ٹ�һ��(���ﻹҪ�����ؿ���)
		//for(unsigned int j = 0; j < control_point_count; ++j)
		//{
		//	weight_[i][j] += 1.0 / control_point_count;
		//	weight_[i][j] /= 2.0;
		//}
	}
	// ����p_asterisk_
	p_asterisk_.resize(model_point_count);
	for(unsigned int i = 0; i < model_point_count; ++i)
	{
		p_asterisk_[i] = glm::vec2(0.0, 0.0);
		for(unsigned int j = 0; j < control_point_count; ++j)
		{
			p_asterisk_[i] += (glm::vec2::value_type)weight_[i][j] * control_points_[j];
		}
	}
	// ����p_hat_
	p_hat_.resize(model_point_count);
	for(unsigned int i = 0; i < model_point_count; ++i)
	{
		p_hat_[i].resize(control_point_count);	
		for(unsigned int j = 0; j < control_point_count; ++j)
		{
			p_hat_[i][j] = control_points_[j] - p_asterisk_[i];
		}
	}
	// ����q_asterisk_��С
	q_asterisk_.resize(model_point_count);
	// ����q_hat_��С
	q_hat_.resize(model_point_count);
	for(unsigned int i = 0; i < model_point_count; ++i)
	{
		q_hat_[i].resize(control_point_count);
	}
	// ����A_i_
	A_i_.resize(model_point_count);
	for(unsigned int k = 0; k < model_point_count; ++k)
	{
		A_i_[k].resize(control_point_count);
		for(unsigned int i = 0; i < control_point_count; ++i)
		{
			glm::mat2 pp = glm::transpose(glm::mat2(p_hat_[k][i], -getPerpendicularVec(p_hat_[k][i])));
			glm::mat2 v_minus_p_asterisk(reference_model_points_[k] - p_asterisk_[k], -getPerpendicularVec(reference_model_points_[k] - p_asterisk_[k]));
			A_i_[k][i] = (glm::mat2::value_type)weight_[k][i] * pp * v_minus_p_asterisk;
		}
	}
}

// �ƶ����Ƶ�
// ע�⣬����Ҫ���п��Ƶ���ڵ�����µ���
void ModelHandler::DeformByControlPoints()
{
	unsigned int model_point_count = reference_model_points_.size();
	unsigned int control_point_count = control_points_.size();
	if(0 == control_point_count)
	{
		fprintf(stderr, "\nerror: deform without any control point\n");
		exit(EXIT_FAILURE);
	}
	// ����q_asterisk_
	for(unsigned int i = 0; i < model_point_count; ++i)
	{
		q_asterisk_[i] = glm::vec2(0.0, 0.0);
		for(unsigned int j = 0; j < control_point_count; ++j)
		{
			q_asterisk_[i] += (glm::vec2::value_type)weight_[i][j] * control_points_[j];
		}
	}
	// ����q_hat_
	for(unsigned int i = 0; i < model_point_count; ++i)
	{
		for(unsigned int j = 0; j < control_point_count; ++j)
		{
			q_hat_[i][j] = control_points_[j] - q_asterisk_[i];
		}
	}
	// Rigid Deformation
	for(unsigned int k = 0; k < model_point_count; ++k)
	{
		glm::vec2 sum_q_A(0, 0);
		for(unsigned int i = 0; i < control_point_count; ++i)
		{
			sum_q_A += q_hat_[k][i] * A_i_[k][i];
		}
		double norm_square = glm::dot(sum_q_A, sum_q_A);
		if(norm_square < DBL_MIN)	// ƽ�Ƶ����
		{
			rigid_deform_result_[k] = reference_model_points_[k] + q_asterisk_[k] - p_asterisk_[k];
		}
		else
		{
			//rigid_deform_result_[k] = reference_model_points_[k] + q_asterisk_[k] - p_asterisk_[k];
			glm::vec2 v_minus_p_asterisk = reference_model_points_[k] - p_asterisk_[k];
			rigid_deform_result_[k] = (glm::vec2::value_type)sqrt(glm::dot(v_minus_p_asterisk, v_minus_p_asterisk) / norm_square) * sum_q_A  + q_asterisk_[k];
		}
	}
}

// �ڿ��Ƶ���б��У�Ѱ���ĸ����Ƶ�İ�Χ�п��԰���point_pos�����
// ����ָ���һ������point_pos�ĵ�����
// δ�ҵ�ʱ����control_points_.cend()
std::vector<glm::vec2>::const_iterator ModelHandler::getAttachedControlPoint(const glm::vec2& point_pos, double influence_box_halflen) const
{
	std::vector<glm::vec2>::const_iterator c_iter = control_points_.begin();
	while(c_iter != control_points_.end())
	{
		if(abs(c_iter->x - point_pos.x) <= influence_box_halflen && abs(c_iter->y - point_pos.y) <= influence_box_halflen)
		{
			break;
		}
		++c_iter;
	}
	return c_iter;
}

