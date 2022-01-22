#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include <vector>
#include <vertex.h>

namespace slicer
{
	class Triangle
	{
	public:
		Vertex v1;
		Vertex v2;
		Vertex v3;

		explicit Triangle(Vertex v1 = Vertex(), Vertex v2 = Vertex(), Vertex v3 = Vertex());

		float x_min() const;
		float x_max() const;
		float y_min() const;

		float y_max() const;
		float z_min() const;
		float z_max() const;

		Triangle& operator+=(Vertex shift);

		bool belong_to_plane_z() const;
		std::vector<Vertex> intersect(float z) const;
		friend std::ostream& operator << (std::ostream& stream, const Triangle& t);
	};
}

#endif
