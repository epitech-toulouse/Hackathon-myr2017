#include <chrono>
#include <algorithm>
#include <cmath>
#include "Algorithm.hh"
#include "Oz/Oz.hh"
#include "Oz/Camera.hh"
#include "Oz/Motor.hh"

namespace Algorithm
{

point::point(double v_x, double v_y, ssize_t v_cluster) :
	x { v_x },
	y { v_y },
	cluster { v_cluster }
{
}

constexpr std::tuple<const double&, const double&> point::tie() const noexcept
{
	return std::tie(this->y, this->x);
}

bool operator<(const point & lhs, const point & rhs)
{
	return lhs.tie() < rhs.tie();
}

bool operator==(const point & lhs, const point & rhs)
{
	return lhs.tie() == rhs.tie();
}

double euclidean_distance(const point & p, const point & q)
{
	return sqrt((q.x - p.x) * (q.x - p.x) + (q.y - p.y) * (q.y - p.y));
}


Scanner::Scanner(void) :
	_epsilon { 0.0 },
	_iterations_count { 0 },
	_world_buffer { },
	_world { },
	_sub_lines { }
{
}

void Scanner::update(const std::array<uint16_t, LIDAR_CAPTURE_RESOLUTION> & capture)
{
	this->agglomerate(capture);
	this->scan_sub_lines();
}

void Scanner::set_epsilon(double e) noexcept
{
	_epsilon = e;
}

double Scanner::get_epsilon() const noexcept
{
	return _epsilon;
}

size_t Scanner::get_iterations_count() const noexcept
{
	return _iterations_count;
}

const std::deque<std::vector<point>> & Scanner::get_sub_lines() const noexcept
{
	return _sub_lines;
}

void Scanner::agglomerate(const std::array<uint16_t, LIDAR_CAPTURE_RESOLUTION> & capture)
{
	int16_t angle = LIDAR_BEGIN_ANGLE;
	_world_buffer.clear();
	for (uint16_t dist : capture) {
		if (dist > 0) {
			auto vec = sf_vec2_from_polar<double>(dist, angle);
			_world_buffer.emplace_back(vec.x, vec.y, point::unbound);
		}
		++angle;
	}
	std::sort(_world_buffer.begin(), _world_buffer.end());
	_world.swap(_world_buffer);
}

void Scanner::scan_sub_lines()
{
	int current_id = 0;
	std::deque<std::vector<point>> sub_lines_buffer;
	_iterations_count = 0;
	for (point & pt : _world) {
		if (pt.cluster != point::unbound) {
			continue;
		}
		auto neighbors = this->neighbors_of(pt);
		sub_lines_buffer.emplace_back();
		cluster_info cluster { sub_lines_buffer.back(), current_id };
		pt.cluster = cluster.id;
		cluster.points.push_back(pt);
		this->expand(cluster, neighbors);
		++current_id;
	}
	_sub_lines.swap(sub_lines_buffer);
}

std::list<Scanner::point_reference> Scanner::neighbors_of(const point & origin)
{
	std::list<Scanner::point_reference> neighbors;
	for (point & candidate : _world) {
		if (candidate.cluster != point::unbound) {
			continue;
		}
		if (euclidean_distance(origin, candidate) < _epsilon) {
			neighbors.push_back(candidate);
		}
	}
	return neighbors;
}

/*
 * add each neighbors to the current cluster, appending new
 * neighbors to the iterated list each iteration, but it's fine
 * because std::list doesn't invalidate it's iterators on
 * std::list::splice().
 */
void Scanner::expand(cluster_info cluster, std::list<Scanner::point_reference> & neighbors)
{
	// NOTE: we explicitly call neighbors.end() as it might change each iteration
	for (auto it = neighbors.begin() ; it != neighbors.end() ; ++it) {
		++_iterations_count;
		point & pt_n = *it;
		if (pt_n.cluster != point::unbound) {
			continue;
		}
		pt_n.cluster = cluster.id;
		cluster.points.push_back(pt_n);
		auto new_neighbors = this->neighbors_of(pt_n);
		point & last = neighbors.back();
		new_neighbors.erase(
			new_neighbors.begin(), std::find_if(
				new_neighbors.begin(),
				new_neighbors.end(),
				[last] (auto current) { return last < current; }
		));
		neighbors.splice(neighbors.end(), std::move(new_neighbors));
	}
}


Algorithm::Algorithm(Oz::Oz & oz) :
	_next { nullptr },
	_oz { oz },
	_scanner { }
{
	_scanner.set_epsilon(300.0);
}

void Algorithm::init()
{
	Oz::Camera & camera = _oz.getCamera();
	camera.enable_compression(false);
	camera.enable_raw(true);
	_next = &Algorithm::wait;
}

void Algorithm::update()
{
	// Scan
	auto capture = _oz.getLidar().get_distances().lock();
	_scan_time = timed_call<std::chrono::milliseconds>([&](){
		_scanner.update(*capture);
	});

	// Move
	(this->*_next)();
}

void Algorithm::wait()
{
	Oz::Lidar & lidar = _oz.getLidar();
	if (lidar.detect() > 1)
	{
		_next = &Algorithm::goStraightForPlow;
	}	
}

void Algorithm::goStraightForPlow()
{
	Oz::Motor & motor = _oz.getMotor();
	Oz::Lidar & lidar = _oz.getLidar();
	if (lidar.detect() > 0) {
		motor.setSpeed(50);
		motor.setAngle(0);
	} else {
		//TODO Do some actions
	}
	_next = &Algorithm::endPlow;
}

void Algorithm::endPlow()
{
	Oz::Motor & motor = _oz.getMotor();
	Oz::Lidar & lidar = _oz.getLidar();
	if (lidar.detect() == 0) {
		motor.setSpeed(0);
		motor.setAngle(0);
		if (motor.getMotorSpeed() == 0)
		{
			_next = &Algorithm::turnOnNextLigne;
		}
	}
}

void Algorithm::turnOnNextLigne()
{
	Oz::Motor & motor = _oz.getMotor();
	motor.setSpeed(-125);
}

const std::chrono::milliseconds Algorithm::get_scan_time() const noexcept
{
	return _scan_time;
}

const Scanner & Algorithm::get_scanner() const noexcept
{
	return _scanner;
}


}
