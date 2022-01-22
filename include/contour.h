#ifndef STL2GCODE_CONTOUR_H
#define STL2GCODE_CONTOUR_H

#include <vector>
#include <ostream>
#include <vertex.h>

typedef std::vector<Vertex> Contour;
std::ostream& operator<<(std::ostream& stream, const Contour& contour);

#endif
