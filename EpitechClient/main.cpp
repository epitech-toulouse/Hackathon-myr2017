#include "run_camera_and_display.hh"
#include "Oz/Motor.hh"

int main(int, char **)
{
	Motor *motor = new Motor();
	just_run_camera_and_display();
	return 0;
}
