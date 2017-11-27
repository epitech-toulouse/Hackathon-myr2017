#pragma once

#include <array>
#include <deque>
#include <list>
#include <cstdint>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Algorithm/Algorithm.hh"
#include "constants.hh"
#include "utilities.hh"

namespace Display
{

class RenderLidar : public sf::Drawable, public sf::Transformable
{
public:
	explicit RenderLidar(void);
	void update_rays(const std::array<uint16_t, LIDAR_CAPTURE_RESOLUTION> & distances);
	void update_lines(const std::deque<std::vector<Algorithm::point>> & lines);
	void toggle_rays();
	void zoom(float factor);

private:
	void draw(sf::RenderTarget & target, sf::RenderStates states) const;

private:
	sf::Color _background_color;
	sf::Color _grid_lines_color;
	sf::VertexArray _vertices;
	sf::VertexArray _ray_lines;
	sf::VertexArray _background;
	sf::VertexArray _angle_grid_lines;
	std::array<sf::CircleShape, 10> _radius_grid_lines;
	std::list<sf::VertexArray> _vao_list_vegetable_lines;
	bool _enable_rays;
	float _zoom;
};

}
