#ifndef STL2GCODE_H
#define STL2GCODE_H

#include <vector>
#include <string>
#include <map>
#include <set>
#include <stl2gcode_parameters.h>
#include <triangle.h>
#include <segment.h>
#include <contour.h>
#include <filesystem>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/json.h>

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
    std::vector<slicer::Triangle> triangles;

    std::vector<std::vector<slicer::Segment>> segments;
    std::vector<std::vector<Contour>> shells;
    std::vector<std::vector<slicer::Segment>> infill;

    std::set<int> planes;
    std::string file;
    STL2gcodeParams params;

    float x_min, x_max, y_min, y_max, z_min, z_max;

    void stl_binary(void);
    void stl_ascii(void);
    bool is_ascii(void);

    void slicing(const float& dz);
    void contour_construction(const std::vector<slicer::Segment>& segments, std::vector<Contour>& contours);
    void filling(const std::vector<Contour>& contours, std::vector<slicer::Segment>& fillings, const int& level, const bool& is_plane);

    void gcode(const std::string& path);

public:
    explicit STL2gcode(const std::string& path, const STL2gcodeParams& params);
    void convert(const std::string& path);
    //void debug_file(void);
};

#endif
