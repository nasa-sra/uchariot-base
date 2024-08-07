#include "pathgen/Bezier.h"

#include <cmath>

/**
 * @brief This function is called when a new waypoint is added to the Bezier
 * curve. It generates new nodes based on the control points of the Bezier
 * curve.
 *
 * @note This function assumes that the Bezier curve is defined by a list of
 * waypoints. The control points are determined based on the waypoints.
 *
 * @return void
 */
void Bezier::_on_way_point_added() {
    if (_way_points.size() < 4) {
        return;
    }

    int new_control_point_index = _way_points.size() - 1;

    if (new_control_point_index == 3) {
        for (int i = 0; i <= _steps; i++) {
            double t = (double)i / (double)_steps;

            add_node(interpolate(t, _way_points[0], _way_points[1],
                                 _way_points[2], _way_points[3]));
        }
    } else {
        if (new_control_point_index % 2 == 0) {
            return;
        }

        int pt = new_control_point_index - 2;
        for (int i = 0; i <= _steps; i++) {
            double t = (double)i / (double)_steps;
            Eigen::Vector3d point4 = 2 * _way_points[pt] - _way_points[pt - 1];

            add_node(interpolate(t, _way_points[pt], point4,
                                 _way_points[pt + 1], _way_points[pt + 2]));
        }
    }
}

/**
 * @brief This function calculates a point on a Bezier curve defined by four
 * control points.
 *
 * @param t The parameter t in the range [0, 1] that determines the position on
 * the curve.
 * @param P0 The first control point of the Bezier curve.
 * @param P1 The second control point of the Bezier curve.
 * @param P2 The third control point of the Bezier curve.
 * @param P3 The fourth control point of the Bezier curve.
 *
 * @return An `Eigen::Vector3d` representing the point on the Bezier curve at
 * parameter t.
 *
 * The function uses the de Casteljau algorithm to calculate the point on the
 * curve. The algorithm is based on the recursive definition of Bezier curves.
 * The function calculates the point by summing the weighted control points,
 * where the weights are determined by the parameter t.
 */
Eigen::Vector3d Bezier::interpolate(double t, const Eigen::Vector3d& P0,
                                    const Eigen::Vector3d& P1,
                                    const Eigen::Vector3d& P2,
                                    const Eigen::Vector3d& P3) {
    Eigen::Vector3d point;
    point = pow(t, 3) * (-P0 + 3 * P1 - 3 * P2 + P3);
    point += pow(t, 2) * (3 * P0 - 6 * P1 + 3 * P2);
    point += t * (-3 * P0 + 3 * P1);
    point += P0;

    return point;
}
