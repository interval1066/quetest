#ifndef SEGMENT_H
#define SEGMENT_H

#include <ostream>
#include <vertex.h>

namespace slicer
{
	class Segment
	{
	public:
		Vertex v0;
		Vertex v1;
		explicit Segment(Vertex v0 = Vertex(), Vertex v1 = Vertex());

		const Vertex& operator[](bool v) const;
		float length() const;
		void shorten_by(const float& by);

		Vertex intersect_with_plane(const float &z) const ;
		float distance(const Vertex& v3) const ;
		bool intersect_with_segment(const Segment &s2, Vertex &intersection) const ;

		bool operator==(const Segment& s) const ;
		friend std::ostream& operator<<(std::ostream& stream, const Segment& segment);
	};
}

#endif
