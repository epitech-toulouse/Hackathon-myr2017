#pragma once

#include <array>
#include <cstdint>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "constants.hh"

namespace Display
{

class RenderLidar : public sf::Drawable, public sf::Transformable
{
public:
	explicit RenderLidar(void);
	void update(const std::array<uint16_t, LIDAR_CAPTURE_RESOLUTION> & distances);

private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	sf::Color _background_color;
	sf::Color _grid_lines_color;
	sf::VertexArray _vertices;
	sf::VertexArray _background;
	sf::VertexArray _angle_grid_lines;
	std::array<sf::CircleShape, 10> _radius_grid_lines;
};

}
