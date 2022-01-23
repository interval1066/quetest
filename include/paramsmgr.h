#ifndef PARAMSMGR_H_
#define PARAMSMGR_H_

#include <string>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/json.h>

namespace slicer
{
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
}

#endif // PARAMSMGR_H_
