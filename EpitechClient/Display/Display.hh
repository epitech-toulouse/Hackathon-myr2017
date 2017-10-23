#pragma once

#include <SFML/Config.hpp>
#include "Oz/Camera.hh"

class ClientDisplay
{
public:
	explicit ClientDisplay(void);
	void set_target_camera(Oz::ClientCamera * camera) noexcept;
	void show() noexcept;

private:
	const std::string _window_title;
	const unsigned _window_framerate;
	const sf::Uint8 * _left_buffer;
	const sf::Uint8 * _right_buffer;
	const Oz::ClientCamera * _camera;
};
