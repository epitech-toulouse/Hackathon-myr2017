#include <ApiCodec/HaLidarPacket.hpp>
#include "Playground/Playground.hh"

namespace Playground
{
  Playground::Playground(const std::string & host, const std::string & main_port, const std::string & camera_port) :
    system { },
    gateway { host, main_port, camera_port },
    oz { this->gateway },
    display { this->system, this->gateway }
  {
    /* Launch playground */
    try {
      this->run();
    } catch (const ClientException & error) {
      std::cerr << "fatal: " << error.what() << std::endl;
    }
  }

  static void get_lidar_info(Display::Display & display, Gateway::Gateway & gateway)
  {
    std::array<uint16_t, LIDAR_CAPTURE_RESOLUTION> dist;
    int i = 0;
    while (gateway.is_running()) {
      std::shared_ptr<HaLidarPacket> halidarpacket = gateway.get<HaLidarPacket>();
      if (halidarpacket != nullptr) {
        for (auto & value : dist) {
          value = static_cast<uint16_t>(halidarpacket->distance[i]);
          printf("%d\n", i);
          i = i + 1;
        }
        display.update_lidar(dist);
        i = 0;
        std::this_thread::sleep_for(WAIT_TIME_MS);
      }
    }
  }

  int Playground::run() {
    this->display.set_target_camera(&(this->oz.getCamera()));
    this->gateway.start();
    this->oz.getCamera().start();
    while (!this->oz.getCamera().is_running() || !this->gateway.is_running()) {
      std::this_thread::sleep_for(WAIT_TIME_MS);
    }
    using Command = ApiCommandPacket::CommandType;
    this->gateway.enqueue(std::make_unique<ApiCommandPacket>(Command::TURN_OFF_IMAGE_ZLIB_COMPRESSION));
    this->gateway.enqueue(std::make_unique<ApiCommandPacket>(Command::TURN_ON_API_RAW_STEREO_CAMERA_PACKET));
    std::thread get_lidar_info_thread = std::thread(get_lidar_info, std::ref(display), std::ref(gateway));
    this->display.show();
    this->oz.getCamera().stop();
    this->gateway.stop();
    get_lidar_info_thread.join();
    return 0;
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
