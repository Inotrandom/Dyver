#ifndef H_RECTPRISM
#define H_RECTPRISM

#include "shape.h"
#include <Eigen/Dense>

class rectprism_t : shape_t
{
public:
	rectprism_t(const double al, const double aw, const double ah)
	{
		l = al;
		w = aw;
		h = ah;
	}

	auto get_l() -> double { return l; }
	auto get_w() -> double { return w; }
	auto get_h() -> double { return h; }

	/**
	 * @brief Get the centroid of the rectangle in relation to the bottom corner (abstract position of the rectangle)
	 *
	 * @return Eigen::Vector3d
	 */
	auto centroid() -> Eigen::Vector3d override { return {l / 2.0, w / 2.0, h / 2.0}; }

private:
	double l;
	double w;
	double h;
};

#endif // H_RECTPRISM