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
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/json.h>

struct STL2gcodeParams
{
	float layer_height = 0.2f;
	float nozzle_diameter = 0.25f;
	float thread_thickness = 1.75f;

	float top_bottom_thickness = 0.4f;
	float filling_density = 0.15;
	int nozzle_temperature = 205;

	int table_temperature = 55;
	int printing_speed = 35;
	int filling_speed = 40;

	int moving_speed = 90;
	int printer_width = 215;
	int printer_depth = 215;

	int printer_height = 300;
	float near_point = 0.00002f;
	float near_distance = 0.03f;
};

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

class ParamsMgr
{
	std::string _filepath;
	Json::Reader _reader;
	Json::Value _root;
	Json::StyledStreamWriter _writer;

public:
	explicit ParamsMgr(const std::string& path) : _filepath(path)
	{
		Json::Value _root;
		if(!_reader.parse(_filepath, _root)) {
			throw std::runtime_error(_reader.getFormattedErrorMessages());
			return;
		}
	}

	void WriteParams();

	// So we can have arbitraty return types w/one method
	template<class T>
	decltype(g(T())) GetParam()
	{
		return g(T());
	}
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

	STL2gcodeParams _params;

	void stl_binary(void);
	void stl_ascii(void);
	bool is_ascii(void);

	void slicing(const float&);
	void contour_construction(const std::vector<slicer::Segment>&, std::vector<Contour>&);
	void filling(const std::vector<Contour>&, std::vector<slicer::Segment>&, const int&, const bool&);

	void gcode(const std::string&);
	void processSTL(const std::string&);

public:
    explicit STL2gcode(const std::string&, const STL2gcodeParams&);
    void convert(const std::string&);
};

constexpr float STL2gcode::near_point = 0.00002f;
constexpr float STL2gcode::near_distance = 0.03f;

#endif
