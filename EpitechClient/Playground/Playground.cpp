#include <ApiCodec/HaLidarPacket.hpp>
#include "Playground/Playground.hh"

namespace Playground
{
  Playground::Playground(int argc, char ** argv) :
    args { arg_parse(argc, argv) },
    system { },
    gateway { args.host, args.main_port, args.camera_port, args.read_interval, args.write_interval },
    oz { this->gateway },
    algorithm { this->oz },
    display { this->system, this->gateway, this->oz, this->algorithm }
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
    this->gateway.enable_camera(args.enable_camera);
    this->gateway.start();
    this->algorithm.init();
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
      	this->algorithm.update();
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
