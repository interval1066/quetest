#include <contour.h>

std::ostream& operator<<(std::ostream& stream, const Contour& contour)
{
	for (auto& vert : contour)
		stream << vert << " ";

	return stream;
}
