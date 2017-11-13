#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Display/Display.hh"
#include "Playground/Playground.hh"
#include "constants.hh"

namespace Display
{

static const size_t INNER_GAP = 3;
static const size_t LIDAR_TV_WIDTH = CAMERA_CAPTURE_WIDTH * 2 + INNER_GAP;
static const size_t LIDAR_TV_HEIGHT = LIDAR_TV_WIDTH * 0.6;
static const size_t VIDEO_WIDTH = LIDAR_TV_WIDTH;
static const size_t VIDEO_HEIGHT = LIDAR_TV_HEIGHT + CAMERA_CAPTURE_HEIGHT + INNER_GAP;

Display::Display(const Playground::System & sys, const Gateway::Gateway & gateway) :
	_system { sys },
	_gateway { gateway },
	_window_title { "Naio Move your Robot 2017 :: Epitech's API Client" },
	_window_mode { VIDEO_WIDTH, VIDEO_HEIGHT },
	_window_framerate { 60 },
	_left_buffer { nullptr },
	_right_buffer { nullptr },
	_camera { nullptr },
	_lidar { nullptr }
{
}

void Display::set_target_camera(Oz::Camera * camera) noexcept
{
	_camera = camera;
	if (_camera != nullptr) {
		_camera->share_screen_buffers(&_left_buffer, &_right_buffer);
	}
}

void Display::update_lidar(const std::array<uint16_t, LIDAR_CAPTURE_RESOLUTION> & distances)
{
	if (_lidar) {
		_lidar->update(distances);
	}
}

void Display::show() noexcept
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 1;
	sf::RenderWindow window(_window_mode, _window_title, sf::Style::Default, settings);
	window.setVerticalSyncEnabled(false);
	window.setFramerateLimit(_window_framerate);

	sf::Texture left_texture, right_texture;
	left_texture.create(CAMERA_CAPTURE_WIDTH, CAMERA_CAPTURE_HEIGHT);
	right_texture.create(CAMERA_CAPTURE_WIDTH, CAMERA_CAPTURE_HEIGHT);
	sf::Sprite left_sprite(left_texture);
	sf::Sprite right_sprite(right_texture);
	right_sprite.setPosition(CAMERA_CAPTURE_WIDTH + INNER_GAP, 0);

	sf::Font monospace_font;
	monospace_font.loadFromFile(_system.queryAssetPath("iosevka-term-custom-regular.ttf"));

	sf::Image camera_disabled_img;
	camera_disabled_img.loadFromFile(_system.queryAssetPath("camera_disabled.png"));

	sf::View lidar_view(sf::FloatRect(0, 0, LIDAR_TV_WIDTH, LIDAR_TV_HEIGHT));
	_lidar = std::make_unique<RenderLidar>();
	lidar_view.setCenter(sf::Vector2f(0, LIDAR_TV_HEIGHT / 2 - LIDAR_TV_HEIGHT / 10));
	lidar_view.setSize(sf::Vector2f(LIDAR_TV_WIDTH, LIDAR_TV_HEIGHT));
	lidar_view.setViewport(sf::FloatRect(
		0,
		static_cast<float>(CAMERA_CAPTURE_HEIGHT + INNER_GAP) / static_cast<float>(VIDEO_HEIGHT),
		static_cast<float>(LIDAR_TV_WIDTH) / static_cast<float>(VIDEO_WIDTH),
		static_cast<float>(LIDAR_TV_HEIGHT) / static_cast<float>(VIDEO_HEIGHT)
	));

	sf::Text gateway_stats_text;
	gateway_stats_text.setFont(monospace_font);
	gateway_stats_text.setCharacterSize(10);
	gateway_stats_text.setFillColor(sf::Color::Yellow);
	gateway_stats_text.setOutlineColor(sf::Color::Black);
	gateway_stats_text.setOutlineThickness(1.0);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}
		if (
			(_camera != nullptr && _camera->has_image()) ||
			camera_disabled_img.getSize() == sf::Vector2u()
		) {
			left_texture.update(_left_buffer);
			right_texture.update(_right_buffer);
		} else {
			left_texture.update(camera_disabled_img, CAMERA_CAPTURE_WIDTH / 2 - 50, CAMERA_CAPTURE_HEIGHT / 2 - 50);
			right_texture.update(camera_disabled_img, CAMERA_CAPTURE_WIDTH / 2 - 50, CAMERA_CAPTURE_HEIGHT / 2 - 50);
		}
		auto & stats = _gateway.get_stats();
		std::string gateway_stats_str = "Gateway [";
		gateway_stats_str += std::to_string(stats.at("command_packets_transmitted")) + "/";
		gateway_stats_str += std::to_string(stats.at("camera_packets_transmitted")) + " tx | ";
		gateway_stats_str += std::to_string(stats.at("command_packets_received")) + "/";
		gateway_stats_str += std::to_string(stats.at("camera_packets_received")) + " rx | ";
		gateway_stats_str += std::to_string(stats.at("command_packets_lost")) + "/";
		gateway_stats_str += std::to_string(stats.at("camera_packets_lost")) + " lost | ";
		gateway_stats_str += std::to_string(stats.at("command_packets_bad")) + "/";
		gateway_stats_str += std::to_string(stats.at("camera_packets_bad")) + " bad]\n";
		gateway_stats_str += "Command Socket [";
		gateway_stats_str += (_gateway.is_command_connected()? "ON" : "OFF");
		gateway_stats_str += "]\n";
		gateway_stats_str += "Camera Socket [";
		gateway_stats_str += (_gateway.is_camera_connected()? "ON" : "OFF");
		gateway_stats_str += "]";
		gateway_stats_text.setString(gateway_stats_str);
		gateway_stats_text.setPosition(10, 10);
		window.clear();
		window.draw(left_sprite);
		window.draw(right_sprite);
		window.setView(lidar_view);
		window.draw(*_lidar);
		window.setView(window.getDefaultView());
		window.draw(gateway_stats_text);
		window.display();
	}
}

}
