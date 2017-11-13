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

  /* Extern function to launch thread */
  static void lidar_noise(Display::Display & display, Gateway::Gateway & gateway)
  {
    std::array<uint16_t, LIDAR_CAPTURE_RESOLUTION> dist;
    while (gateway.is_running()) {
      for (auto & value : dist) {
	value = static_cast<uint16_t>(float(random()) / float(RAND_MAX) * 400);
      }
      display.update_lidar(dist);
      std::this_thread::sleep_for(WAIT_TIME_MS);
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
    //    std::thread lidar_noise_thread = std::thread(this->lidar_noise, std::ref(this->display), std::ref(this->gateway));
    std::thread lidar_noise_thread = std::thread(lidar_noise, std::ref(this->display), std::ref(this->gateway));
    this->display.show();
    this->oz.getCamera().stop();
    this->gateway.stop();
    lidar_noise_thread.join();
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
