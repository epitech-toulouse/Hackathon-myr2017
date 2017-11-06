#pragma once

#include <SFML/Config.hpp>
#include "RenderLidar.hh"
#include "Oz/Camera.hh"
#include "constants.hh"

namespace Display
{

class Display
{
public:
	explicit Display(void);
	void set_target_camera(Oz::Camera * camera) noexcept;
	void update_lidar(const std::array<uint16_t, LIDAR_CAPTURE_RESOLUTION> & distances);
	void show() noexcept;

private:
	const std::string _window_title;
	const sf::VideoMode _window_mode;
	const unsigned _window_framerate;
	const sf::Uint8 * _left_buffer;
	const sf::Uint8 * _right_buffer;
	const Oz::Camera * _camera;
	std::unique_ptr<RenderLidar> _lidar;
};

}
