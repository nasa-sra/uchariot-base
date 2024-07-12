#include "Curve.h"

class Bezier : public Curve
{
public:
	Bezier() : Curve() {};

protected:
	virtual void _on_way_point_added();

	Eigen::Vector3d interpolate(double u, const Eigen::Vector3d& P0, const Eigen::Vector3d& P1, const Eigen::Vector3d& P2, const Eigen::Vector3d& P3);
};