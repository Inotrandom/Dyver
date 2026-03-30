#ifndef H_SHAPE
#define H_SHAPE

#include <Eigen/Dense>

/**
 * @brief Shape interface class
 *
 */
class shape_t
{
public:
	virtual auto centroid() -> Eigen::Vector3d;

private:
};

#endif // H_SHAPE