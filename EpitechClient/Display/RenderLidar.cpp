#include <cmath>
#include "RenderLidar.hh"

namespace Display
{

RenderLidar::RenderLidar(void) :
	_background_color { 0x0F0100FF },
	_grid_lines_color { 0x440B05FF },
	_vertices { sf::Points, LIDAR_CAPTURE_RESOLUTION },
	_ray_lines { sf::Lines, LIDAR_CAPTURE_RESOLUTION * 2 },
	_background { sf::Triangles, 6 },
	_angle_grid_lines { sf::Lines, 28 * 2 },
	_enable_rays { true }
{
	/* Background */
	float half = 1000.0f; /* Background half size */
	_background[0].position = sf::Vector2f(-half, -half);
	_background[1].position = sf::Vector2f(-half, half);
	_background[2].position = sf::Vector2f(half, -half);
	_background[3].position = sf::Vector2f(half, half);
	_background[4].position = sf::Vector2f(-half, half);
	_background[5].position = sf::Vector2f(half, -half);
	for (size_t i = 0 ; i < 6 ; ++i) {
		_background[i].color = sf::Color(_background_color);
	}

	/* Grid */
	double angle = LIDAR_BEGIN_ANGLE;
	for (size_t idx = 0 ; idx < 28 * 2 ; ++idx) {
		double x = 1000.0 * cos(angle * M_PI / 180.0);
		double y = 1000.0 * sin(angle * M_PI / 180.0);
		_angle_grid_lines[idx].position = sf::Vector2f(0, 0);
		_angle_grid_lines[idx].color = sf::Color(_grid_lines_color);
		++idx;
		_angle_grid_lines[idx].position = sf::Vector2f(static_cast<float>(x), static_cast<float>(y));
		_angle_grid_lines[idx].color = sf::Color(_grid_lines_color);
		angle += 10.0;
	}
	float radius = 0.0f;
	for (auto && line : _radius_grid_lines) {
		radius += 50.0f;
		line.setPosition(-radius, -radius);
		line.setRadius(radius);
		line.setFillColor(sf::Color(0));
		line.setOutlineThickness(1);
		line.setOutlineColor(sf::Color(_grid_lines_color));
	}
}

void RenderLidar::update(const std::array<uint16_t, LIDAR_CAPTURE_RESOLUTION> & distances)
{
	for (size_t angle = 0 ; angle < distances.size() ; ++angle) {
		double dist = distances[angle] / 10.0;
		double x = dist * cos((static_cast<double>(angle) + LIDAR_BEGIN_ANGLE) * M_PI / 180.0);
		double y = dist * sin((static_cast<double>(angle) + LIDAR_BEGIN_ANGLE) * M_PI / 180.0);
		_vertices[angle].position = sf::Vector2f(static_cast<float>(x), static_cast<float>(y));
		_vertices[angle].color = sf::Color(0xFFFFFFFF);
		_ray_lines[angle * 2].position = sf::Vector2f(static_cast<float>(x), static_cast<float>(y));
		_ray_lines[angle * 2].color = sf::Color(0x004411FF);
		_ray_lines[angle * 2 + 1].position = sf::Vector2f();
		_ray_lines[angle * 2 + 1].color = sf::Color(0x004411FF);
	}
}

void RenderLidar::toggle_rays()
{
	_enable_rays = !_enable_rays;
}

void RenderLidar::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();
	states.texture = NULL;
	target.draw(_background, states);
	target.draw(_angle_grid_lines, states);
	for (auto && line : _radius_grid_lines) {
		target.draw(line, states);
	}
	if (_enable_rays) {
		target.draw(_ray_lines, states);
	}
	target.draw(_vertices, states);
}

}
