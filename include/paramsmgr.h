#ifndef PARAMSMGR_H_
#define PARAMSMGR_H_

#include <string>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/json.h>

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

#endif // PARAMSMGR_H_
