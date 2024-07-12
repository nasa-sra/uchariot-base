#include "pathgen/Curve.h"

void Curve::clear(){
	_nodes.clear();
	_way_points.clear();
	_distances.clear();
}


void Curve::add_way_point(const Eigen::Vector3d& point){
	_way_points.push_back(point);
	_on_way_point_added();
}

void Curve::add_node(const Eigen::Vector3d& node){
	_nodes.push_back(node);

	if(_nodes.size() == 1){
		_distances.push_back(0);
	}
	else{
		int new_node_index = _nodes.size() - 1;

		double segment_distance = (_nodes[new_node_index] - _nodes[new_node_index-1]).norm();
		_distances.push_back(segment_distance + _distances[new_node_index-1]);
	}
}