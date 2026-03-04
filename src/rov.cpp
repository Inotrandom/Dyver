#include <algorithm>

#include "rov.h"

#include <Eigen/Dense>

/**
 * @brief Iterates through thrusters and optimizes each individual one using `rov_t::optimize_thruster`
 *
 * @param target_translational Passed to `rov_t::optimize_thruster`
 * @param target_rotational Passed to `rov_t::optimize_thruster`
 */
void rov_t::optimize_throttle_config(Eigen::Vector3d target_translational, Eigen::Vector3d target_rotational)
{
	for (auto [id, p_thruster] : m_thrusters)
	{
		optimize_thruster(p_thruster, target_translational, target_rotational);
	}
}

/**
 * @brief Calculates the force and torque that `which` produces, then uses `Eigen::MatrixBase::cross` and `Eigen::MatrixBase::dot` to calculate how "effective"
 * the thruster is at achieving `target_translational` and `target_rotational`
 *
 * @param which Pointer to the thruster and is used to get thruster data and store the result (`m_target_congruence`)
 * @param target_translational Normalized and used to compare against the thruster in a linear fashion
 * @param target_rotational Normalized and used to compare against the thruster in a rotational fashion
 */
void rov_t::optimize_thruster(std::shared_ptr<thruster_t> which, Eigen::Vector3d &target_translational, Eigen::Vector3d &target_rotational)
{
	Eigen::Vector3d &look = which->get_look();
	Eigen::Vector3d &pos = which->get_pos();

	double final_congruence = 0.0;

	// Thruster lookat target
	const Eigen::Vector3d lookat = target_translational.normalized();

	const double dot = lookat.dot(look);

	final_congruence = dot;

	const Eigen::Vector3d calc_torque = pos.cross(look);
	const Eigen::Vector3d calc_torque_direction = calc_torque.normalized();

	const Eigen::Vector3d lookat_torque = target_rotational.normalized();

	const double torque_dot = lookat_torque.dot(calc_torque_direction);

	final_congruence += torque_dot;
	final_congruence = std::clamp(final_congruence, -1.0, 1.0);

	which->get_target_congruence() = final_congruence;

	// For as disgusting as this down here is, I'm going to keep it in case it becomes useful again

	/*
	std::cout << "Congruence of the thruster:\n" << final_congruence << std::endl;
	std::cout << std::endl;
	std::cout << "Position of the thruster:\n" << pos << std::endl;
	std::cout << std::endl;
	std::cout << "Look of the thruster:\n" << look << std::endl;
	std::cout << std::endl;
	std::cout << "Calculated torque production:\n" << calc_torque << std::endl;
	std::cout << std::endl;
	*/
}