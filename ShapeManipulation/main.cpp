#include <opencv.hpp>
#include <string>
#include <fstream>
#include "local_glm.h"
#include "ModelPreparation\DataProvider.h"
#include "ModelPreparation\EFCHandler.h"
#include "ModelPreparation\ContourDetector.h"
#include "ModelPreparation\FileUtil.hpp"
#include "ModelPreparation\Meshing.h"
#include "Deformation\DeformFramework.h"

#ifdef NDEBUG
#pragma comment(linker, "/NODEFAULTLIB:msvcrtd")
#endif

//const std::string kImgPath = "1.jpg";
//const std::string kImgPath = "2.jpg";
const std::string kImgPath = "Image/14.jpg";
//const std::string kPolyFileName = "3.poly";

int main()
{
	// ��ͼ���л�ȡ���ݣ����㡢�����������ꡢ������λ�ã�
	DataProvider data_provider;
	data_provider.AcquireModelData(kImgPath);
	// OpenGL����ģ������
	DeformFramework *p_deform_framework = DeformFramework::getInstance();
	p_deform_framework->RenderSetUp("deformation", 400, 600, "shader/vertex.glsl", "shader/fragment.glsl");
	int feature_point_loc[4] = {data_provider.head_idx_, data_provider.tail_idx_, data_provider.left_fin_idx_, data_provider.right_fin_idx_};
	p_deform_framework->DataSetUp(data_provider.vertices_, data_provider.triangles_, data_provider.tex_coord_, kImgPath);
	// ��ʼ����
	p_deform_framework->LoopBegin();
	return 0;
}