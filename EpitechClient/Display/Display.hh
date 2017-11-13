#pragma once

#include <SFML/Config.hpp>
#include "RenderLidar.hh"
#include "Oz/Camera.hh"
#include "constants.hh"
#include "Playground/System.hh"

namespace Display
{

class Display
{
	Display(const Display &) = delete;
	Display & operator= (const Display &) = delete;

public:
	explicit Display(const Playground::System &, const Gateway::Gateway &);
	void set_target_camera(Oz::Camera * camera) noexcept;
	void update_lidar(const std::array<uint16_t, LIDAR_CAPTURE_RESOLUTION> & distances);
	void show() noexcept;

private:
	const Playground::System & _system;
	const Gateway::Gateway & _gateway;
	const std::string _window_title;
	const sf::VideoMode _window_mode;
	const unsigned _window_framerate;
	const sf::Uint8 * _left_buffer;
	const sf::Uint8 * _right_buffer;
	const Oz::Camera * _camera;
	std::unique_ptr<RenderLidar> _lidar;
};

}
