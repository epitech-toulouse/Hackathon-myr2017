#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include "Arguments.hh"

namespace Playground
{

static int _verbose_flag = 0;
static int _help_flag = 0;

static const char * default_host = "localhost";
static const char * default_main_port = "5559";
static const char * default_camera_port = "5558";
static const char * default_read_interval = "100";
static const char * default_write_interval = "100";

static struct option long_options[] = {
	{"verbose",        no_argument,       &_verbose_flag, 1},
	{"help",           no_argument,       &_help_flag, 1},
	{"host",           required_argument, 0, 'a'},
	{"main-port",      required_argument, 0, 'm'},
	{"camera-port",    required_argument, 0, 'c'},
	{"read-interval",  required_argument, 0, 'r'},
	{"write-interval", required_argument, 0, 'w'},
	{0, 0, 0, 0}
};

static void help(void)
{
	std::cout
	  << "EpitechClient v1.0.0-git" << "\n"
	  << "\n"
	  << "Usage:" << "\n"
	  << "  ./EpitechClient [OPTIONS...]" << "\n"
	  << "\n"
	  << "Options:" << "\n"
	  << "  -h, --help\t\t\tShow this help and exit" << "\n"
	  << "  -v, --verbose\t\t\tSets verbose output mode" << "\n"
	  << "  -a, --host=HOST\t\tTarget hostname to connect with (default: " << default_host << ")" << "\n"
	  << "  -m, --main-port=PORT\tTarget host port for main communication (default: " << default_main_port << ")" << "\n"
	  << "  -c, --camera-port=PORT\tTarget host port for camera communication (default: " << default_camera_port << ")" << "\n"
	  << "  -r, --read-interval=MS\tTime interval between socket reads (default: " << default_read_interval << ")" << "\n"
	  << "  -w, --write-interval=MS\tTime interval between socket writes (default: " << default_write_interval << ")" << "\n"
	  << std::endl;
}

struct Arguments arg_parse(int argc, char ** argv)
{
	struct Arguments args = {
		.verbose = _verbose_flag,
		.host = std::string(default_host),
		.main_port = std::string(default_main_port),
		.camera_port = std::string(default_camera_port),
		.read_interval = std::stoul(default_read_interval),
		.write_interval = std::stoul(default_write_interval)
	};
	int flag;
	int option_index;
	for (;;) {
		option_index = 0;
		flag = getopt_long(argc, argv, "a:c:hm:r:vw:", long_options, &option_index); 
		if (flag == -1) {
			break;
		}
		switch (flag) {
		case 0:
			break;
		case 'v':
			_verbose_flag = 1;
			break;
		case 'h':
			_help_flag = 1;
			break;
		case 'a':
			args.host = std::string(optarg);
			break;
		case 'm':
			args.main_port = std::string(optarg);
			break;
		case 'c':
			args.camera_port = std::string(optarg);
			break;
		case 'r':
			args.read_interval = std::stoul(optarg);
			break;
		case 'w':
			args.write_interval = std::stoul(optarg);
			break;
		case '?':
			/* getopt_long already printed an error message. */
			break;
		default:
			break;
		}
	}
	args.verbose = _verbose_flag;
	if (_help_flag) {
		help();
		exit(0);
	}
	if (optind < argc) {
		help();
		exit(1);
	}
	return args;
}

}
