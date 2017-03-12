#ifndef DATA_PROVIDER_H
#define DATA_PROVIDER_H

#include "..\local_glm.h"
#include <vector>
#include <string>

class DataProvider
{
public:
	DataProvider() : vertices_(), triangles_(), tex_coord_(), head_idx_(-1), tail_idx_(-1), left_fin_idx_(-1), right_fin_idx_(-1)
	{}

	void AcquireModelData(const std::string& img_path);
	
	std::vector<glm::vec2> vertices_;
	std::vector<glm::ivec3> triangles_;
	std::vector<glm::vec2> tex_coord_;
	int head_idx_;
	int tail_idx_;
	int left_fin_idx_;
	int right_fin_idx_;
	
};

#endif