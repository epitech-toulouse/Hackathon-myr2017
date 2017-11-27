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
	_angle_grid_lines { sf::Lines },
	_enable_rays { true }
{
	/* Background */
	float half = 1.0f; /* Background half size */
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
	_angle_grid_lines.append({{0, 0}, sf::Color(_grid_lines_color)});
	_angle_grid_lines.append({sf_vec2_from_polar(4000.0f, {LIDAR_BEGIN_ANGLE}), _grid_lines_color});

	_angle_grid_lines.append({{0, 0}, sf::Color(_grid_lines_color)});
	_angle_grid_lines.append({sf_vec2_from_polar(4000.0f, {LIDAR_END_ANGLE}), _grid_lines_color});

	_angle_grid_lines.append({{0, 0}, sf::Color(_grid_lines_color)});
	_angle_grid_lines.append({sf_vec2_from_polar(4000.0f, 0.0f), _grid_lines_color});

	_angle_grid_lines.append({{0, 0}, sf::Color(_grid_lines_color)});
	_angle_grid_lines.append({sf_vec2_from_polar(4000.0f, 90.0f), _grid_lines_color});

	_angle_grid_lines.append({{0, 0}, sf::Color(_grid_lines_color)});
	_angle_grid_lines.append({sf_vec2_from_polar(4000.0f, 180.0f), _grid_lines_color});

	float radius = 0.0f;
	for (auto & line : _radius_grid_lines) {
		radius += 400.0f;
		line.setPosition(-radius, -radius);
		line.setRadius(radius);
		line.setFillColor(sf::Color(0));
		line.setOutlineThickness(1);
		line.setOutlineColor(sf::Color(_grid_lines_color));
	}
}

void RenderLidar::update_rays(const std::array<uint16_t, LIDAR_CAPTURE_RESOLUTION> & distances)
{
	for (size_t angle = 0 ; angle < distances.size() ; ++angle) {
		float dist = distances[angle];
		auto vec = sf_vec2_from_polar<float>(dist, static_cast<float>(angle) + static_cast<float>(LIDAR_BEGIN_ANGLE));
		_vertices[angle].position = vec;
		_vertices[angle].color = sf::Color(0xFFFFFFFF);
		_ray_lines[angle * 2].position = vec;
		_ray_lines[angle * 2].color = sf::Color(0x004411FF);
		_ray_lines[angle * 2 + 1].position = sf::Vector2f();
		_ray_lines[angle * 2 + 1].color = sf::Color(0x004411FF);
	}
}

void RenderLidar::update_lines(const std::deque<std::vector<Algorithm::point>> & lines)
{
	size_t new_size = 0;
	for (auto & current_line : lines) {
		new_size += current_line.size();
	}
	_vao_list_vegetable_lines.clear();
	for (auto & current_line : lines) {
		size_t len = current_line.size();
		sf::VertexArray vao_strip { sf::LineStrip, len };
		for (size_t idx = 0 ; idx < len ; ++idx) {
			vao_strip[idx].position = sf::Vector2f(
				static_cast<float>(current_line[idx].x),
				static_cast<float>(current_line[idx].y)
			);
			vao_strip[idx].color = sf::Color(0x0000FFFF);
		}
		_vao_list_vegetable_lines.push_back(vao_strip);
		sf::VertexArray vao_line { sf::LineStrip, 2 };
		vao_line[0] = sf::Vertex{{
			static_cast<float>(current_line.front().x),
			static_cast<float>(current_line.front().y)
		}, sf::Color{0x00FFFFFF}};
		vao_line[1] = sf::Vertex{{
			static_cast<float>(current_line.back().x),
			static_cast<float>(current_line.back().y)
		}, sf::Color{0x00FFFFFF}};
		_vao_list_vegetable_lines.push_back(vao_line);
	}
}

void RenderLidar::toggle_rays()
{
	_enable_rays = !_enable_rays;
}

void RenderLidar::zoom(float factor)
{
	_zoom = factor;
}

void RenderLidar::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	states.texture = NULL;
	sf::RenderStates inv = states;
	inv.transform *= target.getView().getTransform().getInverse();
	states.transform *= this->getTransform();
	target.draw(_background, inv);
	target.draw(_angle_grid_lines, states);
	for (auto line : _radius_grid_lines) {
		line.setOutlineThickness(1.0f * _zoom);
		target.draw(line, states);
	}
	if (_enable_rays) {
		target.draw(_ray_lines, states);
	}
	for (const auto & vao : _vao_list_vegetable_lines) {
		target.draw(vao, states);
	}
	target.draw(_vertices, states);
}

}
