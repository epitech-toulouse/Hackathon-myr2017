#include <iostream>
#include <Oz/Camera.hh>
#include <Oz/CameraException.hh>

int main(int, char **)
{
	try {
		Oz::ClientCamera oz_camera;
		oz_camera.connect("127.0.0.1", 5559);
		oz_camera.run();
	} catch (const Oz::ClientCameraException & error) {
		std::cerr << "Oz::Camera: " << error.what() << std::endl;
	}
	return 0;
}
