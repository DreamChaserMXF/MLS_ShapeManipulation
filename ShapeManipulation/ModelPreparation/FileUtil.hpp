#include <fstream>

template<class T>
void MakePolyFile(const std::vector<T>& contour, const std::string& kPolyFileName)
{
	std::ofstream out(kPolyFileName);
	unsigned int contour_size = contour.size();
	// point
	out << contour_size << " 2 0 0" << std::endl;	// point num, dimension, attribute num, border marker
	for(unsigned int i = 0; i < contour_size; ++i)
	{
		out << i << " " << contour[i].x << " " << contour[i].y << std::endl;
	}
	// edge
	out << contour_size << " 0" << std::endl;	// segment num, attribute num
	for(unsigned int i = 0; i < contour_size; ++i)
	{
		out << i << " " << i << " " << (i + 1) % contour_size << std::endl;
	}
	// hole
	out << "0" << std::endl;
	out.close();
}