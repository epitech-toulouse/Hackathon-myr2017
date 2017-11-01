#include "run_camera_and_display.hh"
#include "Gateway/Gateway.hh"

int main(int, char **)
{
	Gateway gateway("127.0.0.1", 5559, 5558);
	//just_run_camera_and_display();
	return 0;
}
