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

point operator-(const point & lhs, const point & rhs) noexcept
{
	return { lhs.x - rhs.x, lhs.y - rhs.y, point::unbound };
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
	return std::sqrt((q.x - p.x) * (q.x - p.x) + (q.y - p.y) * (q.y - p.y));
}

double vector_length(const point & p)
{
	return std::sqrt(p.x * p.x + p.y * p.y);
}

point normalize(const point & p)
{
	point q { p.x, p.y, point::unbound };
	double len = vector_length(q);
	q.x /= len;
	q.y /= len;
	return q;
}

double vector_angle(const point & p, const point & q)
{
	point pn = normalize(p);
	point qn = normalize(q);
	return std::acos(pn.x * qn.x + pn.y * qn.y) * 180.0 / math::pi<double>();
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

void Algorithm::adjust()
{
	Oz::Motor & motor = _oz.getMotor();
	std::deque<std::vector<point>> sub_lines = _scanner.get_sub_lines();
	std::pair<point*, point*> nearpoint = get_near_point(sub_lines);
	if (nearpoint.first && nearpoint.second)
	{
		int distance1 = (int) sqrt(pow(nearpoint.first->x, 2) + pow(nearpoint.first->y, 2));
		int distance2 = (int) sqrt(pow(nearpoint.second->x, 2) + pow(nearpoint.second->y, 2));
		if (distance1 < distance2)
			motor.setAngle(static_cast<int8_t>(motor.getAngle() + 1));
		if (distance1 > distance2)
			motor.setAngle(static_cast<int8_t>(motor.getAngle() - 1));
	}
}

void Scanner::agglomerate(const std::array<uint16_t, LIDAR_CAPTURE_RESOLUTION> & capture)
{
	int16_t angle = LIDAR_BEGIN_ANGLE;
	_world_buffer.clear();
	for (uint16_t dist : capture) {
		if (dist > 0) {
			auto vec = sf_vec2_from_polar<double>(dist, angle);
#if 0
			if (!_world_buffer.empty()) {
				auto prev = _world_buffer.back();
				point a { vec.x, vec.y };
				point b { prev.x, prev.y };
				if (euclidean_distance(a, b) < 10.0) {
					_world_buffer.pop_back();
					_world_buffer.emplace_back((a.x + b.x) / 2.0, (a.y + b.y) / 2.0, point::unbound);
					continue;
				}
			}
#endif
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
#if 0
		if (cluster.points.size() > 1) {
			point a = cluster.points.back() - cluster.points[cluster.points.size() - 2];
			point b = pt_n - cluster.points.back();
			double angle = vector_angle(a, b);
			if (angle > 10.0) {
				pt_n.cluster = point::noise;
				continue;
			}
		}
#endif
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
	_scanner { },
	_scan_time { 0 },
	_last_update_time { },
	_run_distance { 0 }
{
	_scanner.set_epsilon(300.0);
}

void Algorithm::init()
{
	Oz::Camera & camera = _oz.getCamera();
	camera.enable_compression(false);
	camera.enable_raw(true);
	_next = &Algorithm::wait;
	_last_update_time = Clock::now();
}

void Algorithm::update()
{
	// Scan
	auto capture = _oz.getLidar().get_distances().lock();
	_scan_time = timed_call<std::chrono::milliseconds>([&](){
		_scanner.update(*capture);
	});
	this->adjust();

	// Update run distance
	auto now = Clock::now();
	this->update_run_distance(_oz.getODO().getSpeed(), duration_cast<std::chrono::milliseconds>(now - _last_update_time));
	_last_update_time = now;

	// Move
	(this->*_next)();
}

double Algorithm::get_run_distance() const noexcept
{
	return _run_distance;
}

void Algorithm::update_run_distance(double ground_speed, std::chrono::milliseconds delta_time) noexcept
{
	_run_distance += ground_speed * (double(delta_time.count()) / 1000.0);
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
		motor.setSpeed(125);
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
		if (motor.getMotorSpeed() < 50)
		{
			_next = &Algorithm::turnOnNextLigne;
		}
	}
}

void Algorithm::turnOnNextLigne()
{
	Oz::Motor & motor = _oz.getMotor();
	std::deque<std::vector<point>> sub_lines = _scanner.get_sub_lines();
	std::pair<point*, point*> pairPoint = get_near_point(sub_lines);

//	std::cout << (pairPoint.first ? std::to_string(pairPoint.first->x) : "null") << " " << (pairPoint.second ? std::to_string(pairPoint.second->x) : "null") << std::endl;
	motor.setAngle(125);
	motor.setSpeed(125);
}

std::pair<point*, point*> Algorithm::get_near_point(std::deque<std::vector<point>> & points)
{
	point *pointa = nullptr;
	point *pointb = nullptr;
	int distance = 0;

	for (auto & line : points) {
		distance = (int) sqrt(pow(line.front().x, 2) + pow(line.front().y, 2));
		if (pointa == nullptr)
			pointa = &line.front();
		else if (distance < pointa->x)
		{
			pointb = pointa;
			pointa = &line.front();
		}
	}
	return(std::make_pair(pointa, pointb));
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
