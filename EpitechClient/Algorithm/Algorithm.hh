#pragma once

#include <tuple>
#include <set>
#include <functional>
#include <chrono>
#include <list>
#include <deque>
#include "Oz/Oz.hh"
#include "utilities.hh"

using std::chrono::duration_cast;

namespace Algorithm
{

/*
 * Algorithm::point -- Represent a 2D point viewed from the lidar.
 */
struct point
{
	enum { unbound = -1, noise = -2 };
	point(double, double, ssize_t = unbound);
	double x;
	double y;
	ssize_t cluster;
	constexpr std::tuple<const double&, const double&> tie() const noexcept;
};

point operator-(const point & lhs, const point & rhs) noexcept;
bool operator<(const point &, const point &);
bool operator==(const point &, const point &);
double euclidean_distance(const point &, const point &);
double vector_length(const point & p);
point normalize(const point & p);
double vector_angle(const point & p, const point & q);


/*
 * Algorithm::Scanner -- Scan the viewed environment
 * Detect vegetable lines from points, using dbscan.
 */
class Scanner
{
public:
	using point_reference = std::reference_wrapper<point>;
	using line = std::tuple<point, point>;

	struct cluster_info
	{
		std::vector<point> & points;
		int id;
	};

public:
	explicit Scanner(void);
	void update(const std::array<uint16_t, LIDAR_CAPTURE_RESOLUTION> & capture);

	void set_epsilon(double) noexcept;

	double get_epsilon() const noexcept;
	size_t get_iterations_count() const noexcept;
	const std::deque<std::vector<point>> & get_sub_lines() const noexcept;

private:
	void agglomerate(const std::array<uint16_t, LIDAR_CAPTURE_RESOLUTION> &);
	void scan_sub_lines();
	std::list<point_reference> neighbors_of(const point &);
	void expand(cluster_info cluster, std::list<point_reference> & neighbors);

private:
	double _epsilon;
	size_t _iterations_count;
	std::vector<point> _world_buffer;
	std::vector<point> _world;
	std::deque<std::vector<point>> _sub_lines;
};


/*
 * Algorithm::Algorithm -- The algorithm main class
 */
class Algorithm
{
	using Clock = std::chrono::steady_clock;

public:
	explicit Algorithm(Oz::Oz &);
	void init();
	void update();

	const std::chrono::milliseconds get_scan_time() const noexcept;
	const Scanner & get_scanner() const noexcept;
	double get_run_distance() const noexcept;
	const std::string getNextFunctionName() const noexcept;

/*
 * Tools
 */
private:
	void update_run_distance(double, std::chrono::milliseconds) noexcept;
	void adjust();
	std::pair<point, point> get_near_point(std::deque<std::vector<point>> & points);

/*
 * Variables
 */
private:
	void (Algorithm::*_next)(void);
	Oz::Oz & _oz;
	Scanner _scanner;
	std::chrono::milliseconds _scan_time;
	std::chrono::time_point<Clock> _last_update_time;
	double _run_distance;
	double _startTurn;

/*
 * Etapes
 */
private:
	void wait();
	void goStraightForPlow();
	void endPlow();
	void turnOnNextLigne();

};


}
