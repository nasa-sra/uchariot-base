#include "pathgen/Curve.h"

void Curve::clear() {
    _nodes.clear();
    _way_points.clear();
    _distances.clear();
}

/**
 * @brief Adds a way point to the curve.
 *
 * This function adds a new way point to the curve's list of way points.
 * It also triggers the _on_way_point_added() callback function.
 *
 * @param point The 3D position of the way point to be added.
 * The point is represented as an Eigen::Vector3d object.
 *
 * @return void
 * This function does not return any value.
 */
void Curve::add_way_point(const Eigen::Vector3d& point) {
    _way_points.push_back(point);
    _on_way_point_added();
}

/**
 * @brief Adds a node to the curve.
 *
 * This function adds a new node to the curve's list of nodes.
 * It also updates the distances between nodes.
 *
 * @param node The 3D position of the node to be added.
 * The node is represented as an Eigen::Vector3d object.
 *
 * @return void
 * This function does not return any value.
 */
void Curve::add_node(const Eigen::Vector3d& node) {
    _nodes.push_back(node);

    if (_nodes.size() == 1) {
        _distances.push_back(0);
    } else {
        int new_node_index = _nodes.size() - 1;

        double segment_distance =
            (_nodes[new_node_index] - _nodes[new_node_index - 1]).norm();
        _distances.push_back(segment_distance + _distances[new_node_index - 1]);
    }
}