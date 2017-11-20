#pragma once

#include <string>
#include <map>

namespace Playground
{

class System
{
	System(const System &) = delete;
	System & operator=(const System &) = delete;

public:
	explicit System(void);
	const std::string & getRuntimePath() const noexcept;
	const std::string & getAssetsBasePath() const noexcept;
	std::string queryAssetPath(const std::string & name) const noexcept;

private:
	std::string _runtime_path;
	std::string _assets_base_path;
	std::map<std::string, std::string> _assets_paths;
};

}
