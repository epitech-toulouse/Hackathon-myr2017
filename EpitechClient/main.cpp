#include <ApiCodec/ApiCommandPacket.hpp>
#include "Gateway/Gateway.hh"
#include "Oz/Camera.hh"
#include "Display/Display.hh"
#include "exceptions.hh"

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
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	using Command = ApiCommandPacket::CommandType;
	gateway.enqueue(std::make_unique<ApiCommandPacket>(Command::TURN_ON_IMAGE_ZLIB_COMPRESSION));
	gateway.enqueue(std::make_unique<ApiCommandPacket>(Command::TURN_ON_API_RAW_STEREO_CAMERA_PACKET));
	display.show();
	camera.stop();
	gateway.stop();
	return 0;
}

int main(int, char **)
{
	Gateway::Gateway gateway("127.0.0.1", 5559, 5558);
	Oz::Camera camera(gateway);
	Display::Display display;

	try {
		return run(gateway, camera, display);
	} catch (const ClientException & error) {
		std::cerr << "fatal: " << error.what() << std::endl;
	}
	return 1;
}
