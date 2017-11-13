#include "System.hh"
#include <iostream>
#include "utilities.hh"

namespace Playground
{

System::System(void)
{
    _runtime_path = getexe();
    std::clog << "Runtime path: " << _runtime_path << "\n";
    _assets_base_path = _runtime_path + "/../assets";
    _assets_paths.emplace("png", _assets_base_path + "/images");
    _assets_paths.emplace("jpg", _assets_base_path + "/images");
    _assets_paths.emplace("ttf", _assets_base_path + "/fonts");
}

const std::string & System::getRuntimePath() const noexcept
{
	return _runtime_path;
}

const std::string & System::getAssetsBasePath() const noexcept
{
	return _assets_base_path;
}

std::string System::queryAssetPath(const std::string & name) const noexcept
{
	const auto charno = name.find_last_of(".");
	if (charno != std::string::npos) {
		const std::string ext = name.substr(charno + 1);
		return _assets_paths.at(ext) + "/" + name;
	} else {
		return _assets_base_path + "/" + name;
	}
}

}
