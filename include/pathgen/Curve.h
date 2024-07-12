#include <vector>
#include <cassert>

#include <Eigen/Core>

class Curve
{
public:

	Curve(): _steps(100){};

	void add_way_point(const Eigen::Vector3d& point);
	void clear();

	Eigen::Vector3d node(int i) const { return _nodes[i]; }
	double length_from_starting_point(int i) const { return _distances[i]; }
	bool has_next_node(int i) const { return static_cast<int>(_nodes.size()) > i; }
	int node_count() const {  return static_cast<int>(_nodes.size()); }
	bool is_empty() { return _nodes.empty(); }
	double total_length() const
	{
		assert(!_distances.empty());
		return _distances[_distances.size() - 1];
	}
	void increment_steps(int steps) { _steps += steps; }
	void set_steps(int steps) { _steps = steps; }

protected:

	int _steps;

	std::vector<Eigen::Vector3d> _way_points;
	std::vector<Eigen::Vector3d> _nodes;
	std::vector<double> _distances;

	void add_node(const Eigen::Vector3d& node);
	virtual void _on_way_point_added() = 0;

};