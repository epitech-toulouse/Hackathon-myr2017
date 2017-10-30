#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Display/Display.hh"

static const size_t CAMERA_CAPTURE_WIDTH = 752 / 2;
static const size_t CAMERA_CAPTURE_HEIGHT = 480 / 2;

ClientDisplay::ClientDisplay(void) :
	_window_title { "Naio Move your Robot 2017 :: Epitech's API Client" },
	_window_framerate { 60 },
	_left_buffer { nullptr },
	_right_buffer { nullptr },
	_camera { nullptr }
{
}

void ClientDisplay::set_target_camera(Oz::ClientCamera * camera) noexcept
{
	_camera = camera;
	if (_camera != nullptr) {
		_camera->share_screen_buffers(&_left_buffer, &_right_buffer);
	}
}

void ClientDisplay::show() noexcept
{
	const size_t border_width = 3;
	sf::VideoMode window_video_mode(CAMERA_CAPTURE_WIDTH*2+border_width, CAMERA_CAPTURE_HEIGHT);
	sf::RenderWindow window(window_video_mode, _window_title);
	window.setVerticalSyncEnabled(false);
	window.setFramerateLimit(_window_framerate);

	sf::Texture left_texture, right_texture;
	left_texture.create(CAMERA_CAPTURE_WIDTH, CAMERA_CAPTURE_HEIGHT);
	right_texture.create(CAMERA_CAPTURE_WIDTH, CAMERA_CAPTURE_HEIGHT);
	sf::Sprite left_sprite(left_texture);
	sf::Sprite right_sprite(right_texture);
	right_sprite.setPosition(CAMERA_CAPTURE_WIDTH+border_width, 0);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}
		left_texture.update(_left_buffer);
		right_texture.update(_right_buffer);
		window.clear();
		window.draw(left_sprite);
		window.draw(right_sprite);
		window.display();
	}
}
