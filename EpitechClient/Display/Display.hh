#pragma once

#include <SFML/Config.hpp>
#include "Oz/Camera.hh"

namespace Display
{

class Display
{
public:
	explicit Display(void);
	void set_target_camera(Oz::Camera * camera) noexcept;
	void show() noexcept;

private:
	const std::string _window_title;
	const unsigned _window_framerate;
	const sf::Uint8 * _left_buffer;
	const sf::Uint8 * _right_buffer;
	const Oz::Camera * _camera;
};

}
