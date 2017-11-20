#pragma once

#include <SFML/Config.hpp>
#include "RenderLidar.hh"
#include "Oz/Oz.hh"
#include "constants.hh"
#include "Playground/System.hh"

namespace Display
{

struct Context
{
	RenderLidar lidar_renderer;
	sf::Texture left_camera_texture;
	sf::Texture right_camera_texture;
	sf::Sprite left_camera_sprite;
	sf::Sprite right_camera_sprite;
	sf::Image camera_disabled_sprite;
	sf::Text debug_text;
	sf::Text title_camera_left_text;
	sf::Text title_camera_right_text;
	sf::Text title_lidar_text;
	sf::Font monospace_font;
	sf::View lidar_view;
	sf::RectangleShape border_camera_left;
	sf::RectangleShape border_camera_right;
	sf::RectangleShape border_lidar;
	sf::RectangleShape title_border_camera_left;
	sf::RectangleShape title_border_camera_right;
	sf::RectangleShape title_border_lidar;
	bool enable_debug_text;
};

class Display
{
	Display(const Display &) = delete;
	Display & operator= (const Display &) = delete;

public:
	explicit Display(const Playground::System &, const Gateway::Gateway &, const Oz::Oz &);
	void show() noexcept;

private:
	std::unique_ptr<sf::RenderWindow> _build_window() const;
	std::unique_ptr<Context> _build_context() const;
	void _on_event(const sf::Event &, sf::RenderWindow &, Context &);
	void _update_camera(Context &) const;
	void _update_lidar(Context &) const;
	void _update_debug_text(Context &) const;
	void _draw(sf::RenderWindow &, Context &);

private:
	const Playground::System & _system;
	const Gateway::Gateway & _gateway;
	const Oz::Oz & _oz;
	const std::string _window_title;
	const sf::VideoMode _window_mode;
	const unsigned _window_framerate;
};

}
