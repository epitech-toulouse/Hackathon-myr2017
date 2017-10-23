#include <chrono>
#include <iostream>
#include <unistd.h>
#include <Oz/Camera.hh>
#include <Oz/CameraException.hh>
#include <Display/Display.hh>
#include <ApiCodec/ApiCommandPacket.hpp>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "run_camera_and_display.hh"

static void write_command_packet(int fd, ApiCommandPacket::CommandType type) noexcept
{
	ApiCommandPacket packet(type);
	std::unique_ptr<cl_copy::Buffer> buffer = packet.encode();
	write(fd, buffer->data(), buffer->size());
}

static void set_zlib_and_raw_image_mode(const std::string & host, uint16_t port)
{
	struct sockaddr_in target;
	target.sin_addr.s_addr = inet_addr(host.c_str());
	target.sin_family = AF_INET;
	target.sin_port = htons(port);
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (0 > sock) {
		throw std::runtime_error(strerror(errno));
	}
	if (0 > connect(sock, (struct sockaddr *) &target, sizeof(target))) {
		throw std::runtime_error(strerror(errno));
	}
	write_command_packet(sock, ApiCommandPacket::CommandType::TURN_ON_IMAGE_ZLIB_COMPRESSION); // FIXME: Don't work ?!
	write_command_packet(sock, ApiCommandPacket::CommandType::TURN_ON_API_RAW_STEREO_CAMERA_PACKET);
	if (0 > shutdown(sock, SHUT_RDWR)) {
		throw std::runtime_error(strerror(errno));
	}
}

static void oz_camera__run(Oz::ClientCamera * oz_camera) noexcept
{
	try {
		oz_camera->run();
	} catch (const Oz::ClientCameraException & error) {
		std::cerr << "Oz::Camera::run(): " << error.what() << std::endl;
	}
}

void just_run_camera_and_display(void) noexcept
{
	Oz::ClientCamera oz_camera;
	ClientDisplay client_display;
	try {
		set_zlib_and_raw_image_mode("127.0.0.1", 5559);
	} catch (const std::runtime_error & error) {
		std::cerr << "set_zlib_and_raw_image_mode(): " << error.what() << std::endl;
		return;
	}
	try {
		oz_camera.connect("127.0.0.1", 5558);
	} catch (const Oz::ClientCameraException & error) {
		std::cerr << "Oz::Camera::connect(): " << error.what() << std::endl;
	}
	if (!oz_camera.is_connected()) {
		return;
	}
	client_display.set_target_camera(&oz_camera);
	std::thread oz_camera_run_thread = std::thread(oz_camera__run, &oz_camera);
	while (!oz_camera.is_running() && oz_camera.is_connected()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	if (oz_camera.is_running()) {
		client_display.show();
		oz_camera.stop();
	}
	oz_camera_run_thread.join();
}
