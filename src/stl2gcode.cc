#include <stl2gcode.h>

using namespace slicer;

STLDivider::STLDivider(std::string& stlfile) : _file(stlfile)
{
	std::ifstream in(_file.c_str(), std::ifstream::ate | std::ifstream::binary);
	_size = in.tellg();
}

STL2gcode::STL2gcode(const std::string& path, const STL2gcodeParams& params)
{
#ifdef _DEBUG
	std::cout << path.c_str() << std::endl;
#endif
	this->file = path;
	this->_params = params;

	std::fstream f(file);
	if (!f.is_open())
		throw std::invalid_argument("File doesn't exist.");

	f.close();

	if (is_ascii())
		stl_ascii();
	else
		stl_binary();

	processSTL(path);
}

void
STL2gcode::stl_binary()
{
	struct Face
	{
		Vertex normal;
		Triangle triangle;
		unsigned short attribute;
	} face;

	std::ifstream f(file, std::ios::binary);
	char temp[80];
	f.read(temp, 80);

	unsigned int number;
	f.read((char*)& number, 4);

	for(unsigned i = 0; i < number; ++i) {
		f.read((char*)&face, 50);
		triangles.push_back(face.triangle);
	}
	f.close();
}

void
STL2gcode::stl_ascii()
{
	std::ifstream f(file);
	std::string line;

	while (!f.eof()) {
		f >> line;
		if (line == "loop") {
			Vertex v1, v2, v3;
			f >> line;
			f >> line;
			v1.x = stod(line);
			f >> line;
			v1.y = stod(line);
			f >> line;
			v1.z = stod(line);

			f >> line;
			f >> line;
			v2.x = stod(line);
			f >> line;
			v2.y = stod(line);
			f >> line;
			v2.z = stod(line);

			f >> line;
			f >> line;
			v3.x = stod(line);
			f >> line;
			v3.y = stod(line);
			f >> line;
			v3.z = stod(line);

			Triangle triangle(v1, v2, v3);
			triangles.push_back(triangle);
		}
	}
	f.close();
}

bool
STL2gcode::is_ascii()
{
    std::ifstream f(file, std::ios::binary);
    char str[6];
    f.get(str, 6);

    f.close();

    return std::string(str) == "solid";
}

void
STL2gcode::convert(const std::string& path)
{
	x_min = triangles.front().x_min();
	x_max = triangles.front().x_max();
	y_min = triangles.front().y_min();

	y_max = triangles.front().y_max();
	z_min = triangles.front().z_min();
	z_max = triangles.front().z_max();

	for(auto& triangle : triangles) {
		if (x_min > triangle.x_min())
			x_min = triangle.x_min();

		if (x_max < triangle.x_max())
			x_max = triangle.x_max();

		if (y_min > triangle.y_min())
			y_min = triangle.y_min();

		if (y_max < triangle.y_max())
			y_max = triangle.y_max();

		if (z_min > triangle.z_min())
			z_min = triangle.z_min();

		if (z_max < triangle.z_max())
			z_max = triangle.z_max();
	}

	Vertex shift;
	shift.x = _params.printer_width / 2.0f - x_min - (x_max - x_min) / 2.0f;
	shift.y = _params.printer_depth / 2.0f - y_min - (y_max - y_min) / 2.0f;
	shift.z = -z_min;

	for (auto& triangle: triangles)
		triangle += shift;

	x_min += shift.x;   x_max += shift.x;
	y_min += shift.y;   y_max += shift.y;
	z_min += shift.z;   z_max += shift.z;

	slicing(_params.layer_height);
	triangles.clear();

	while (segments[segments.size() - 1].empty()) {
		segments.pop_back();
		shells.pop_back();
	}

	for(unsigned i = 0; i < segments.size(); ++i)
		contour_construction(segments[i], shells[i]);

	segments.clear();
	unsigned plane_levels_count = static_cast<int>(floorf(_params.top_bottom_thickness / _params.layer_height));

	for(unsigned i = 0; i < shells.size() - 1; ++i) {
		bool is_plane;

		if(i < plane_levels_count || planes.count(i) == 1)
			is_plane = true;
		else {
			auto plane = planes.lower_bound(i);
			is_plane = *plane - i < plane_levels_count;
		}

		if(infill.size() > shells.size()) break;
			filling(shells[i], infill[i], i, is_plane);
	}
	gcode(path);
}

void
STL2gcode::slicing(const float& dz)
{
	auto p_size = static_cast<unsigned int>(ceilf((z_max - z_min) / dz) + 1);

	typedef std::vector<Triangle>::iterator Triangle_;
	std::vector<std::vector<Triangle_>> levels;
	levels.resize(p_size);

	for (auto triangle = triangles.begin(); triangle != triangles.end(); ++triangle) {
		float i = ((triangle->z_min() - z_min) / dz);
		int ii = static_cast<int>(floorf(i) + (floorf(i) == i ? 0 : 1));
		levels[ii].push_back(triangle);
	}

    segments.resize(p_size);
    std::list<Triangle_> current;

    for(unsigned i = 0; i < p_size; ++i) {
		float plane_z = z_min + dz * i;

		current.remove_if([&plane_z] (const Triangle_ &t) -> bool {
			return t->z_max() < plane_z;
		});

		current.insert(current.end(), levels[i].begin(), levels[i].end());
		for(auto& t : current) {
            if (t->belong_to_plane_z() && plane_z - dz < t->v1.z && t->v1.z <= plane_z + dz)
                planes.insert(i);
				else {
				 auto points = t->intersect(plane_z);
                if (points.size() == 2) {
                    if (t->z_max() != plane_z || plane_z == z_max) {
                        segments[i].emplace_back(points[0], points[1]);
                    }
                }
            }
        }
    }

	shells.resize(p_size);
	infill.resize(p_size);
}

void
STL2gcode::contour_construction(const std::vector<Segment>& _segments, std::vector<Contour>& contours)
{
	std::vector<Segment> segments(_segments.begin(), _segments.end());

	while(!segments.empty()) {
		Vertex v1 = segments.back().v0;
		Vertex v2 = segments.back().v1;

		segments.pop_back();
		Contour contour;
		contour.push_back(v1);

		contour.push_back(v2);

		while(contour.back().distance(contour.front()) > near_point && !segments.empty()) {
			auto& last = contour.back();
			auto segment = min_element(segments.begin(), segments.end(), [&last] (const Segment& s1, const Segment& s2) -> bool {
				return std::min(last.distance(s1.v0), last.distance(s1.v1)) < std::min(last.distance(s2.v0), last.distance(s2.v1));
			});

			if(segment != segments.end() && std::min(last.distance(segment->v0), last.distance(segment->v1)) <= near_point) {
				if(last.distance(segment->v0) < last.distance(segment->v1))
					contour.push_back(segment->v1);
				else
					contour.push_back(segment->v0);

				segments.erase(segment);
			} else
				break;
		}

		if(contour.back() != contour.front() && contour.back().distance(contour.front()) <= near_point)
			contour.back() = contour.front();

		for(unsigned i = 1; i < contour.size() - 1; ++i) {
			if(contour[i - 1] != contour[i + 1] && Segment(contour[i - 1], contour[i + 1]).distance(contour[i]) <= near_distance) {
				contour.erase(contour.begin() + i);
				--i;
			}
		}

		if(contour.front() == contour.back() && contour.size() > 3) {
			if(Segment(*(contour.begin() + 1), *(contour.end() - 2)).distance(contour.front()) <= near_distance) {
				contour.erase(contour.begin());
				contour.erase(contour.end() - 1);
				contour.push_back(contour.front());
			}
		}
		contours.push_back(contour);
	}
}

void
STL2gcode::filling(const std::vector<Contour>& contours, std::vector<Segment>& fillings, const int& level, const bool& is_plane)
{
#ifdef _DEBUG
	std::cout << contours.size() << std::endl;
#endif

	float x_min = contours.front().front().x;
	float x_max = contours.front().front().x;
	float y_min = contours.front().front().y;

	float y_max = contours.front().front().y;
	float z = contours.front().front().z;

	for(auto& contour: contours) {
		for(auto& vertex: contour) {

			if(vertex.x < x_min)
				x_min = vertex.x;
			else if(x_max < vertex.x)
				x_max = vertex.x;

			if(vertex.y < y_min)
				y_min = vertex.y;
			else if(y_max < vertex.y)
				y_max = vertex.y;
		}
	}

	float dt = std::max(x_max - x_min, y_max - y_min);
	if(is_plane)
		dt = _params.nozzle_diameter;
	else if (_params.filling_density != 0.0f) {
		float filling_square =_params.filling_density * (x_max - x_min) * (y_max - y_min);
		float n = filling_square / (2 * _params.nozzle_diameter * hypot(x_max - x_min, y_max - y_min));
		dt /= n;
	}

	std::vector<Segment> infill_lines;
	if(!is_plane || level % 2 == 0) {
		for (float y = y_min; y < y_max; y += dt) {

			float b = y - x_min;
			float x = y_max - b;
			infill_lines.emplace_back(Vertex(x_min, y, z), Vertex(x, y_max, z));
		}

		for (float x = x_min + dt; x < x_max; x += dt) {
			float b = y_min - x;
			float y = x_max + b;
			infill_lines.emplace_back(Vertex(x, y_min, z), Vertex(x_max, y, z));
		}
	}

	if (!is_plane || level % 2 == 1) {
		for (float x = x_min; x < x_max; x += dt) {
			float b = y_max + x;
			float y = b - x_max;
			infill_lines.emplace_back(Vertex(x, y_max, z), Vertex(x_max, y, z));
		}

		for(float y = y_min + dt; y < y_max; y += dt) {
			float b = y + x_min;
			float x = b - y_min;
			infill_lines.emplace_back(Vertex(x_min, y, z), Vertex(x, y_min, z));
		}
	}

	for(auto& infill_line : infill_lines) {
		std::vector<Vertex> intersections;
		for(auto& contour: contours) {

			for(unsigned i = 0; i < contour.size() - 1; ++i) {
				Vertex intersection;
				if(infill_line.intersect_with_segment(Segment(contour[i], contour[i + 1]), intersection))
					intersections.push_back(intersection);
			}
		}

		std::sort(intersections.begin(), intersections.end(), [] (const Vertex& v1, const Vertex& v2) -> bool {
			return v1.y < v2.y;
		});

		intersections.erase(unique(intersections.begin(), intersections.end(), [](const Vertex& v1, const Vertex& v2) -> bool {
			return v1.distance(v2) <= 3 * near_point;
		}), intersections.end());

		if(intersections.size() > 1) {
			for(unsigned i = 0; i < intersections.size() - 1; i += 2) {
				Segment segment(intersections[i], intersections[i + 1]);

				if(segment.length() >= 3 * _params.nozzle_diameter) {
					segment.shorten_by(_params.nozzle_diameter);
					fillings.push_back(segment);
				}
			}
		}
	}
}

void
STL2gcode::gcode(const std::string& path)
{
	_params.moving_speed = 60 * _params.moving_speed;
	_params.filling_speed = 60 * _params.filling_speed;
	_params.printing_speed = 60 * _params.printing_speed;

	std::ofstream out(path);
	out.precision(6);

	out << "G21" << "\n"; // metric values
	out << "G90" << "\n"; // absolute coordinates
	out << "M82" << "\n"; // absolute extrusion mode

	out << "G28 X0 Y0 Z0" << "\n"; // initial position
	out << "G0 Z100" << "\n"; // lowering table before heating
	out << "M140 S" << _params.table_temperature << "\n"; // turning on the table heating

	out << "M190" << "\n"; // waiting for table temperature
	out << "M104 S" << _params.nozzle_temperature << "\n"; // turning on the extruder heating
	out << "M109" << "\n"; // waiting for extruder temperature

	out << "G0 Z0" << "\n"; // initial position
	out << "G92 E0" << "\n"; // clears the amount of extruded plastic

	const float retraction = 6.5f;
	float extruded = 0.0f;

	for(unsigned l = 0; l < shells.size(); ++l) {
		out << ";LAYER:" << l << "\n";
		if (!shells[l].empty()) {

			out << "G1 E" << extruded - retraction << "\n";
			out << "G0 Z" << shells[l].front().front().z << " F" << _params.moving_speed << "\n";
			out << "G1 E" << extruded << "\n";

			for(auto& contour : shells[l]) {
				out << "G1 E" << extruded - retraction << "\n";
				out << "G0 X" << contour.front().x << " Y" << contour.front().y << " F" << _params.moving_speed << "\n";
				out << "G1 E" << extruded << "\n";

				for(unsigned i = 1; i < contour.size(); ++i) {
					extruded += 4 * _params.layer_height * _params.nozzle_diameter * contour[i].distance(contour[i-1]) / (M_PI * pow(_params.thread_thickness, 2));
					out << "G1 X" << contour[i].x << " Y" << contour[i].y << " E" << extruded << " F" << _params.printing_speed << "\n";
				}
			}
			out << ";INFILL" << "\n";

			for (auto& segment : infill[l]) {
				out << "G1 E" << extruded - retraction << "\n";
				out << "G0 X" << segment.v0.x << " Y" << segment.v0.y << " F" << _params.moving_speed << "\n";

				out << "G1 E" << extruded << "\n";
				extruded += 4 * _params.layer_height * _params.nozzle_diameter * segment.v1.distance(segment.v0) / (M_PI * pow(_params.thread_thickness, 2));
				out << "G1 X" << segment.v1.x << " Y" << segment.v1.y << " E" << extruded << " F" << _params.filling_speed << "\n";
			}
		}
	}

	out << "M104 S0" << "\n"; // turning off the extruder heating
	out << "M140 S0" << "\n"; // turning off the table heating
	out << "G0 X0 Y0 Z" << _params.printer_height << "\n"; // lowering table

	out << "M18" << "\n"; // engine power off
	out.close();
}

void
STL2gcode::processSTL(const std::string& path)
{


}
