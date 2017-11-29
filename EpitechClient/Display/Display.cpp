#include <iostream>
#include <sstream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Display/Display.hh"
#include "Playground/Playground.hh"
#include "constants.hh"

namespace Display
{

static const size_t OUTER_GAP = 10;
static const size_t INNER_GAP = 10;
static const size_t LINE_HEIGHT = 16;
static const size_t LIDAR_TV_X = OUTER_GAP;
static const size_t LIDAR_TV_Y = OUTER_GAP + INNER_GAP + LINE_HEIGHT * 2 + CAMERA_CAPTURE_HEIGHT;
static const size_t LIDAR_TV_WIDTH = CAMERA_CAPTURE_WIDTH * 2 + INNER_GAP;
static const size_t LIDAR_TV_HEIGHT = static_cast<size_t>(LIDAR_TV_WIDTH * 0.6);
static const size_t VIDEO_WIDTH = LIDAR_TV_WIDTH + OUTER_GAP * 2;
static const size_t VIDEO_HEIGHT = LIDAR_TV_HEIGHT + CAMERA_CAPTURE_HEIGHT + INNER_GAP + OUTER_GAP * 2 + LINE_HEIGHT * 2;
static const size_t CAMERA_LEFT_X = OUTER_GAP;
static const size_t CAMERA_LEFT_Y = OUTER_GAP + LINE_HEIGHT;
static const size_t CAMERA_RIGHT_X = OUTER_GAP + CAMERA_CAPTURE_WIDTH + INNER_GAP;
static const size_t CAMERA_RIGHT_Y = OUTER_GAP + LINE_HEIGHT;
static const size_t CAMERA_DISABLED_IMAGE_LEN = 100;
static const size_t CAMERA_DISABLED_X = (CAMERA_CAPTURE_WIDTH - CAMERA_DISABLED_IMAGE_LEN) / 2;
static const size_t CAMERA_DISABLED_Y = (CAMERA_CAPTURE_HEIGHT - CAMERA_DISABLED_IMAGE_LEN) / 2;
static const sf::Color BORDER_COLOR = sf::Color(0x34498EFF);

Display::Display(
	const Playground::System & sys,
	const Gateway::Gateway & gateway,
	const Oz::Oz & oz,
	const Algorithm::Algorithm & algorithm
) :
	_system { sys },
	_gateway { gateway },
	_oz { oz },
	_algorithm { algorithm },
	_window_title { "Naio Move your Robot 2017 :: Epitech's API Client" },
	_window_mode { VIDEO_WIDTH, VIDEO_HEIGHT },
	_window_framerate { 60 }
{
}

void Display::show() noexcept
{
	std::unique_ptr<sf::RenderWindow> window = _build_window();
	std::unique_ptr<Context> ctx = _build_context();
	while (window->isOpen()) {
		sf::Event event;
		while (window->pollEvent(event)) {
			_on_event(event, *window, *ctx);
		}
		_update_camera(*ctx);
		_update_lidar(*ctx);
		_update_debug_text(*ctx);
		window->clear();
		_draw(*window, *ctx);
		window->display();
	}
}

std::unique_ptr<sf::RenderWindow> Display::_build_window() const
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 1;
	auto w = std::make_unique<sf::RenderWindow>(_window_mode, _window_title, sf::Style::Default, settings);
	w->setVerticalSyncEnabled(false);
	w->setFramerateLimit(_window_framerate);
	return w;
}

std::unique_ptr<Context> Display::_build_context() const
{
	auto ctx = std::make_unique<Context>();

	/* Options */
	ctx->enable_debug_text = true;

	/* Font */
	ctx->monospace_font.loadFromFile(_system.queryAssetPath("iosevka-term-custom-regular.ttf"));

	/* Camera left */
	ctx->left_camera_texture.create(CAMERA_CAPTURE_WIDTH, CAMERA_CAPTURE_HEIGHT);
	ctx->left_camera_sprite.setTexture(ctx->left_camera_texture);
	ctx->left_camera_sprite.setPosition(CAMERA_LEFT_X, CAMERA_LEFT_Y);

	ctx->border_camera_left.setSize({CAMERA_CAPTURE_WIDTH, CAMERA_CAPTURE_HEIGHT});
	ctx->border_camera_left.setPosition(CAMERA_LEFT_X, CAMERA_LEFT_Y);
	ctx->border_camera_left.setOutlineThickness(1.0f);
	ctx->border_camera_left.setOutlineColor(BORDER_COLOR);
	ctx->border_camera_left.setFillColor(sf::Color(0x0));

	ctx->title_camera_left_text.setCharacterSize(10);
	ctx->title_camera_left_text.setFont(ctx->monospace_font);
	ctx->title_camera_left_text.setFillColor(sf::Color::White);
	ctx->title_camera_left_text.setString("Left camera");
	ctx->title_camera_left_text.setPosition(CAMERA_LEFT_X + 3, CAMERA_LEFT_Y - LINE_HEIGHT + 2);

	ctx->title_border_camera_left.setPosition(CAMERA_LEFT_X - 1, CAMERA_LEFT_Y - LINE_HEIGHT);
	ctx->title_border_camera_left.setSize({ctx->title_camera_left_text.getGlobalBounds().width + 10, LINE_HEIGHT});
	ctx->title_border_camera_left.setFillColor(BORDER_COLOR);

	/* Camera right */
	ctx->right_camera_texture.create(CAMERA_CAPTURE_WIDTH, CAMERA_CAPTURE_HEIGHT);
	ctx->right_camera_sprite.setTexture(ctx->right_camera_texture);
	ctx->right_camera_sprite.setPosition(CAMERA_RIGHT_X, CAMERA_RIGHT_Y);

	ctx->border_camera_right.setSize({CAMERA_CAPTURE_WIDTH, CAMERA_CAPTURE_HEIGHT});
	ctx->border_camera_right.setPosition(CAMERA_RIGHT_X, CAMERA_RIGHT_Y);
	ctx->border_camera_right.setOutlineThickness(1.0f);
	ctx->border_camera_right.setOutlineColor(BORDER_COLOR);
	ctx->border_camera_right.setFillColor(sf::Color(0x0));

	ctx->title_camera_right_text.setCharacterSize(10);
	ctx->title_camera_right_text.setFont(ctx->monospace_font);
	ctx->title_camera_right_text.setFillColor(sf::Color::White);
	ctx->title_camera_right_text.setString("Right camera");
	ctx->title_camera_right_text.setPosition(CAMERA_RIGHT_X + 3, CAMERA_RIGHT_Y - LINE_HEIGHT + 2);

	ctx->title_border_camera_right.setPosition(CAMERA_RIGHT_X - 1, CAMERA_RIGHT_Y - LINE_HEIGHT);
	ctx->title_border_camera_right.setSize({ctx->title_camera_right_text.getGlobalBounds().width + 10, LINE_HEIGHT});
	ctx->title_border_camera_right.setFillColor(BORDER_COLOR);

	/* Camera no render sprite */
	ctx->camera_disabled_sprite.loadFromFile(_system.queryAssetPath("camera_disabled.png"));

	/* Text */
	ctx->debug_text.setFont(ctx->monospace_font);
	ctx->debug_text.setCharacterSize(10);
	ctx->debug_text.setFillColor(sf::Color::Yellow);
	ctx->debug_text.setOutlineColor(sf::Color::Black);
	ctx->debug_text.setOutlineThickness(1.0);
	ctx->debug_text.setPosition(LIDAR_TV_X + INNER_GAP, LIDAR_TV_Y + INNER_GAP);

	/* Lidar */
	ctx->lidar_view.setCenter(0, LIDAR_TV_HEIGHT / 2 - LIDAR_TV_HEIGHT / 10);
	ctx->lidar_view.setSize(LIDAR_TV_WIDTH, LIDAR_TV_HEIGHT);
	ctx->lidar_view.setViewport(sf::FloatRect(
		float(LIDAR_TV_X) / float(VIDEO_WIDTH),
		float(LIDAR_TV_Y) / float(VIDEO_HEIGHT),
		float(LIDAR_TV_WIDTH) / float(VIDEO_WIDTH),
		float(LIDAR_TV_HEIGHT) / float(VIDEO_HEIGHT)
	));
	ctx->lidar_view.zoom(10.0f);
	ctx->lidar_renderer.zoom(10.0f);

	ctx->border_lidar.setSize({LIDAR_TV_WIDTH, LIDAR_TV_HEIGHT});
	ctx->border_lidar.setPosition(LIDAR_TV_X, LIDAR_TV_Y);
	ctx->border_lidar.setOutlineThickness(1.0f);
	ctx->border_lidar.setOutlineColor(BORDER_COLOR);
	ctx->border_lidar.setFillColor(sf::Color(0x0));

	ctx->title_lidar_text.setCharacterSize(10);
	ctx->title_lidar_text.setFont(ctx->monospace_font);
	ctx->title_lidar_text.setFillColor(sf::Color::White);
	ctx->title_lidar_text.setString("Lidar (Top-View)");
	ctx->title_lidar_text.setPosition(LIDAR_TV_X + 3, LIDAR_TV_Y - LINE_HEIGHT + 2);

	ctx->title_border_lidar.setPosition(LIDAR_TV_X - 1, LIDAR_TV_Y - LINE_HEIGHT);
	ctx->title_border_lidar.setSize({ctx->title_lidar_text.getGlobalBounds().width + 10, LINE_HEIGHT});
	ctx->title_border_lidar.setFillColor(BORDER_COLOR);

	return ctx;
}

void Display::_on_event(const sf::Event & event, sf::RenderWindow & window, Context & ctx)
{
	static bool mouse_drag = false;
	static sf::Vector2f mouse_drag_start;
	static float zoom = 10.0f;

	switch (event.type) {
	case sf::Event::Closed:
		window.close();
		break;
	case sf::Event::MouseButtonPressed:
		if (event.mouseButton.button == 0) {
			mouse_drag = true;
			mouse_drag_start = window.mapPixelToCoords(
				{ event.mouseButton.x, event.mouseButton.y }
			);
		}
		break;
	case  sf::Event::MouseButtonReleased:
		if (event.mouseButton.button == 0) {
			mouse_drag = false;
		}
		break;
	case sf::Event::MouseMoved: {
		if (!mouse_drag) {
			break;
		}
		const sf::Vector2f mouse_drag_end = window.mapPixelToCoords(
			{ event.mouseMove.x, event.mouseMove.y }
		);
		const sf::Vector2f delta = (mouse_drag_start - mouse_drag_end) * zoom;
		ctx.lidar_view.setCenter(ctx.lidar_view.getCenter() + delta);
		mouse_drag_start = window.mapPixelToCoords(
			{ event.mouseMove.x, event.mouseMove.y }
		);
		break;
	}
	case sf::Event::MouseWheelScrolled:
		if (mouse_drag) {
		    break;
		}
		if (event.mouseWheelScroll.delta <= -1) {
		    zoom = std::min(20.f, zoom + .1f * zoom);
		} else if (event.mouseWheelScroll.delta >= 1) {
		    zoom = std::max(.5f, zoom - .1f * zoom);
		}
		ctx.lidar_view.setSize(LIDAR_TV_WIDTH, LIDAR_TV_HEIGHT);
		ctx.lidar_view.zoom(zoom);
		ctx.lidar_renderer.zoom(zoom);
		break;
	case sf::Event::KeyReleased:
		switch (event.key.code) {
		case sf::Keyboard::F5:
			ctx.enable_debug_text = !ctx.enable_debug_text;
			break;
		case sf::Keyboard::F6:
			ctx.lidar_renderer.toggle_rays();
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void Display::_update_camera(Context & ctx) const
{
	if (_oz.getCamera().has_image() || is_sf_image_empty(ctx.camera_disabled_sprite)) {
		auto left_image = _oz.getCamera().get_left_image().lock();
		auto right_image = _oz.getCamera().get_right_image().lock();
		ctx.left_camera_texture.update(left_image.get());
		ctx.right_camera_texture.update(right_image.get());
	} else {
		ctx.left_camera_texture.update(
			ctx.camera_disabled_sprite, CAMERA_DISABLED_X, CAMERA_DISABLED_Y);
		ctx.right_camera_texture.update(
			ctx.camera_disabled_sprite, CAMERA_DISABLED_X, CAMERA_DISABLED_Y);
	}
}

void Display::_update_lidar(Context & ctx) const
{
	auto distances = _oz.getLidar().get_distances().lock();
	if (distances) {
		ctx.lidar_renderer.update_rays(*distances.get());
	}
	ctx.lidar_renderer.update_lines(_algorithm.get_scanner().get_sub_lines());
}

void Display::_update_debug_text(Context & ctx) const
{
	if (!ctx.enable_debug_text) {
		return;
	}
	std::stringstream out;
	const Oz::GPS & gps = _oz.getGPS();
	const Oz::Lidar& lidar = _oz.getLidar();
	auto & stats = _gateway.get_stats();

	/* Gateway */
	out << "Gateway ["
	  << std::to_string(stats.at("command_packets_transmitted")) << "/"
	  << std::to_string(stats.at("camera_packets_transmitted")) << " tx | "
	  << std::to_string(stats.at("command_packets_received")) << "/"
	  << std::to_string(stats.at("camera_packets_received")) << " rx | "
	  << std::to_string(stats.at("command_packets_lost")) << "/"
	  << std::to_string(stats.at("camera_packets_lost")) << " lost | "
	  << std::to_string(stats.at("command_packets_bad")) << "/"
	  << std::to_string(stats.at("camera_packets_bad")) << " bad]"
	  << "\n\n";

	/* Sockets */
	out
	  << "Command Socket [" << (_gateway.is_command_connected()? "ON" : "OFF") << "]\n"
	  << "Camera Socket [" << (_gateway.is_camera_connected()? "ON" : "OFF") << "]\n\n"

	/* GPS */
	  << "GPS (" << gps.getLat() << ", " << gps.getLon() << ", " << gps.getAlt() << ")";
	if (gps.getUnit() != 0) {
		out << " [unit: " << gps.getUnit() << "]";
	}
	out << " [" << static_cast<unsigned>(gps.getSatelliteCount()) << " satellites, "
	  << "quality: " << static_cast<unsigned>(gps.getQuality()) << "]\n"
	  << "Speed: " << gps.getGroundSpeed() * 100000.0 / 3600.0 << " cm/s\n"
	  << "Epoch: " << gps.getTime().count() << "ms\n";

	out << "Motor: Speed = " << (int)(_oz.getMotor().getSpeed()) << "\n"
	  << "Motor: Angle = " << (int)(_oz.getMotor().getAngle()) << "\n";

	/* Lidar */
	out
	  << "Lidar [" << lidar.detect() << " hits: "
	  << lidar.detect_right() << " right, "
	  << lidar.detect_left() << " left]"
	  << " [extremums: " << lidar.detect_rightmost_angle() << " "
	  << lidar.detect_leftmost_angle() << "]\n";

	/* Gyrometer */
	out
	  << "Gyro X = " << (_oz.getGyro().getX()) << "\n"
	  << "Gyro Y = " << (_oz.getGyro().getY()) << "\n"
	  << "Gyro Z = " << (_oz.getGyro().getZ()) << "\n"
	  << "Gyro XX = " << (_oz.getGyro().getXX()) << "\n"
	  << "Gyro YY = " << (_oz.getGyro().getYY()) << "\n"
	  << "Gyro ZZ = " << (_oz.getGyro().getZZ()) << "\n"
	  << "Speed = " << (_oz.getODO().getSpeed()) << "\n"
	  << "Position Actuator = " << static_cast<int>(_oz.getActuator().getPosition()) << "\n";

	/* Algorithm */
	out
	  << "Algorithm: " << _algorithm.get_scan_time().count() << "ms, "
	  << _algorithm.get_scanner().get_iterations_count() << " iterations, "
	  << _algorithm.get_scanner().get_sub_lines().size() << " connected lines" << "\n"
	  << "Run distance: " << _algorithm.get_run_distance() << "cm\n"
	  << "Run distance: " << _oz.getODO().getDistance() << "cm\n"
	  << "Function call: " << _algorithm.getNextFunctionName() << "\n";
	ctx.debug_text.setString(out.str());
}

void Display::_draw(sf::RenderWindow & window, Context & ctx)
{
	window.draw(ctx.border_camera_left);
	window.draw(ctx.title_border_camera_left);
	window.draw(ctx.title_camera_left_text);
	window.draw(ctx.left_camera_sprite);

	window.draw(ctx.border_camera_right);
	window.draw(ctx.title_border_camera_right);
	window.draw(ctx.title_camera_right_text);
	window.draw(ctx.right_camera_sprite);

	window.draw(ctx.border_lidar);
	window.draw(ctx.title_border_lidar);
	window.draw(ctx.title_lidar_text);
	window.setView(ctx.lidar_view);
	window.draw(ctx.lidar_renderer);
	window.setView(window.getDefaultView());

	if (ctx.enable_debug_text) {
		window.draw(ctx.debug_text);
	}
}

}
