#ifndef STL2GCODE_STL2GCODE_H
#define STL2GCODE_STL2GCODE_H

#include <vector>
#include <string>
#include <map>
#include <set>
#include <stl2gcode_parameters.h>
#include <triangle.h>
#include <segment.h>
#include <contour.h>

class stl2gcode
{
    static const float near_point;
    static const float near_distance;
    std::vector<Triangle> triangles;

    std::vector<vector<Segment>> segments;
    std::vector<vector<Contour>> shells;
    std::vector<vector<Segment>> infill;

    std::set<int> planes;
    std::string file;
    stl2gcode_parameters parameters;

    float x_min, x_max, y_min, y_max, z_min, z_max;

    void stl_binary();
    void stl_ascii();
    bool is_ascii();

    void slicing(const float& dz);
    void contour_construction(const std::vector<Segment>& segments, std::vector<Contour>& contours);
    void filling(const std::vector<Contour>& contours, std::vector<Segment>& fillings, const int& level, const bool& is_plane);

    void gcode(const string& path);

public:
    explicit stl2gcode(const std::string& path, const stl2gcode_parameters& parameters);
    void convert(const std::string& path);
    void debug_file();
};

#endif
