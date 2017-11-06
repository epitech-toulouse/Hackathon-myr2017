#include <functional>
#include <ApiCodec/ApiCommandPacket.hpp>
#include "Gateway/Gateway.hh"
#include "Oz/Oz.hh"
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
	std::thread lidar_noise_thread = std::thread(lidar_noise, std::ref(display), std::ref(gateway));
	display.show();
	camera.stop();
	gateway.stop();
	lidar_noise_thread.join();
	return 0;
}

int main(int, char **)
{
	Gateway::Gateway gateway("127.0.0.1", 5559, 5558);
	Oz::Oz oz(gateway);
	Display::Display display;

	try {
	  return run(gateway, oz.getCamera(), display);
	} catch (const ClientException & error) {
		std::cerr << "fatal: " << error.what() << std::endl;
	}
	return 1;
}
