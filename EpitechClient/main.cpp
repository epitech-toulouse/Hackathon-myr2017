<<<<<<< HEAD
#include <functional>
#include <ApiCodec/ApiCommandPacket.hpp>
#include <ApiCodec/HaLidarPacket.hpp>
#include <string>
#include "Gateway/Gateway.hh"
#include "Oz/Camera.hh"
#include "Display/Display.hh"
#include "exceptions.hh"
#include "constants.hh"
#include <cstdlib>


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

static void get_lidar_info(Display::Display & display, Gateway::Gateway & gateway)
{
	std::array<uint16_t, LIDAR_CAPTURE_RESOLUTION> dist;
	int i = 0;
	while (gateway.is_running()) {
		std::shared_ptr<HaLidarPacket> halidarpacket = gateway.get<HaLidarPacket>();
		if (halidarpacket != nullptr) {
			for (auto & value : dist) {
				value = static_cast<uint16_t>(halidarpacket->distance[i]);
				i = i + 1;
			}
			display.update_lidar(dist);
			std::this_thread::sleep_for(WAIT_TIME_MS);
		}
	}
}

/*
 * TODO: This code must be moved to Playground class when implemented.
 */
static int run(
	Gateway::Gateway & gateway,
	Oz::Camera & camera,
	Display::Display & display
) {
	display.set_target_camera(&camera);
	gateway.start();
	camera.start();
	std::cout << "Waiting for connection..." << std::endl;
	while (!camera.is_running() || !gateway.is_running()) {
		std::this_thread::sleep_for(WAIT_TIME_MS);
	}
	using Command = ApiCommandPacket::CommandType;
	gateway.enqueue(std::make_unique<ApiCommandPacket>(Command::TURN_ON_IMAGE_ZLIB_COMPRESSION));
	gateway.enqueue(std::make_unique<ApiCommandPacket>(Command::TURN_ON_API_RAW_STEREO_CAMERA_PACKET));
	//std::thread lidar_noise_thread = std::thread(lidar_noise, std::ref(display), std::ref(gateway));
	std::thread get_lidar_info_thread = std::thread(get_lidar_info, std::ref(display), std::ref(gateway));
	display.show();
	camera.stop();
	gateway.stop();
	//lidar_noise_thread.join();
	get_lidar_info_thread.join();
	return 0;
}
=======
#include "Playground/Playground.hh"
>>>>>>> 437273d70176d43e2e39df55b8292fc6b2d78322

int main(int, char **)
{
  Playground::Playground playground("127.0.0.1", "5559", "5558");
}
