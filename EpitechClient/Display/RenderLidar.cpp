#include <cmath>
#include "RenderLidar.hh"

namespace Display
{

RenderLidar::RenderLidar(void) :
	_vertices { sf::Points, LIDAR_CAPTURE_RESOLUTION }
{
}

void RenderLidar::update(const std::array<uint16_t, LIDAR_CAPTURE_RESOLUTION> & distances)
{
	for (size_t angle = 0 ; angle < distances.size() ; ++angle) {
		double dist = distances[angle];
		double x = dist * cos((static_cast<double>(angle) + LIDAR_BEGIN_ANGLE) * M_PI / 180.0);
		double y = dist * sin((static_cast<double>(angle) + LIDAR_BEGIN_ANGLE) * M_PI / 180.0);
		_vertices[angle].position = sf::Vector2f(static_cast<float>(x), static_cast<float>(y));
		_vertices[angle].color = sf::Color(0xFFFFFFFF);
	}
}

void RenderLidar::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();
	states.texture = NULL;

	sf::VertexArray bg(sf::Triangles, 6);
	double half = 1000;
	bg[0].position = sf::Vector2f(-half, -half);
	bg[1].position = sf::Vector2f(-half, half);
	bg[2].position = sf::Vector2f(half, -half);
	bg[3].position = sf::Vector2f(half, half);
	bg[4].position = sf::Vector2f(-half, half);
	bg[5].position = sf::Vector2f(half, -half);
	for (size_t i = 0 ; i < 6 ; ++i) { bg[i].color = sf::Color(0x0F0100FF); }
	target.draw(bg, states);

	std::array<sf::CircleShape, 10> radius_lines;

	float radius = 0.0;
	for (auto && line : radius_lines) {
		radius += 50.0;
		line.setPosition(-radius, -radius);
		line.setRadius(radius);
		line.setFillColor(sf::Color(0));
		line.setOutlineThickness(1);
		line.setOutlineColor(sf::Color(0x440B05FF));
	}

	for (auto && line : radius_lines) {
		target.draw(line, states);
	}

	sf::VertexArray angle_lines(sf::Lines, 28 * 2);
	double angle = LIDAR_BEGIN_ANGLE;
	for (size_t idx = 0 ; idx < 28 * 2 ; ++idx) {
		double x = 1000.0 * cos(angle * M_PI / 180.0);
		double y = 1000.0 * sin(angle * M_PI / 180.0);
		angle_lines[idx].position = sf::Vector2f(0, 0);
		angle_lines[idx].color = sf::Color(0x440B05FF);
		++idx;
		angle_lines[idx].position = sf::Vector2f(x, y);
		angle_lines[idx].color = sf::Color(0x440B05FF);
		angle += 10.0;
	}
	target.draw(angle_lines, states);

	target.draw(_vertices, states);
}

}
