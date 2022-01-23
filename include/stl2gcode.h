#ifndef STL2GCODE_H_
#define STL2GCODE_H_

#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>
#include <list>
#include <stdexcept>
#include <triangle.h>
#include <segment.h>
#include <contour.h>
#include <filesystem>
#include <paramsmgr.h>


// Might spin this out into its own TU like the params class
class STLDivider
{
	std::vector<std::pair<char*, int>> slicecontainer;
	std::string _file;
	size_t _size;

public:
	explicit STLDivider(std::string&);
	std::vector<std::pair<char*, int>> ReadFile(const char*);
	void SplitFile(void);
};

class STL2gcode
{
	static const float near_point;
	static const float near_distance;
	float x_min, x_max, y_min, y_max, z_min, z_max;

	std::vector<slicer::Triangle> triangles;
	std::vector<std::vector<slicer::Segment>> segments;
	std::vector<std::vector<Contour>> shells;

	std::vector<std::vector<slicer::Segment>> infill;
	std::set<int> planes;
	std::string file;

	slicer::STL2gcodeParams _params;

	void stl_binary(void);
	void stl_ascii(void);
	bool is_ascii(void);

	void slicing(const float&);
	void contour_construction(const std::vector<slicer::Segment>&, std::vector<Contour>&);
	void filling(const std::vector<Contour>&, std::vector<slicer::Segment>&, const int&, const bool&);

	void gcode(const std::string&);
	void processSTL(const std::string&);

public:
    explicit STL2gcode(const std::string&, const slicer::STL2gcodeParams&);
    void convert(const std::string&);
};

constexpr float STL2gcode::near_point = 0.00002f;
constexpr float STL2gcode::near_distance = 0.03f;

#endif
