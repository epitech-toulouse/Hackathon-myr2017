#include <ApiCodec/HaLidarPacket.hpp>
#include "Playground/Playground.hh"

namespace Playground
{
  Playground::Playground(int argc, char ** argv) :
    args { arg_parse(argc, argv) },
    system { },
    gateway { args.host, args.main_port, args.camera_port, args.read_interval, args.write_interval },
    oz { this->gateway },
    display { this->system, this->gateway, this->oz }
  {
    /* Launch playground */
    try {
      this->run();
    } catch (const ClientException & error) {
      std::cerr << "fatal: " << error.what() << std::endl;
    }
  }

  int Playground::run()
  {
    this->gateway.start();
    while (!this->gateway.is_running()) {
      std::this_thread::sleep_for(WAIT_TIME_MS);
    }
    using Command = ApiCommandPacket::CommandType;
    this->gateway.emplace<ApiCommandPacket>(Command::TURN_OFF_IMAGE_ZLIB_COMPRESSION);
    this->gateway.emplace<ApiCommandPacket>(Command::TURN_ON_API_RAW_STEREO_CAMERA_PACKET);
    auto update_thread = _update_thread();
    this->display.show();
    this->gateway.stop();
    update_thread.join();
    return 0;
  }
  
  std::thread Playground::_update_thread()
  {
    return std::thread([this](){
      while (this->gateway.is_running()) {
	for (Oz::Unit & unit : this->oz) {
	  unit.update();
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(args.read_interval));
      }
    });
  }

  Oz::Oz& Playground::getOz(void)
  {
    return (oz);
  }

  Gateway::Gateway& Playground::getGateway(void)
  {
    return (gateway);
  }

  Display::Display& Playground::getDisplay(void)
  {
    return (display);
  }
}
