#pragma once

#include <string>
#include <cstdint>

namespace Playground {

struct Arguments
{
	int verbose;
	std::string host;
	std::string main_port;
	std::string camera_port;
	unsigned read_interval;
	unsigned write_interval;
	bool enable_camera;
};

struct Arguments arg_parse(int argc, char ** argv);

}
