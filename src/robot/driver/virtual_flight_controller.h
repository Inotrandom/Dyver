#ifndef H_VIRTUAL_FLIGHT_CONTROLLER
#define H_VIRTUAL_FLIGHT_CONTROLLER

#include <vector>
#include <memory>

#include "abstract_driver.h"

/**
 * @brief In charge of all the drivers and stuff
 *
 */
class virtual_flight_controller_t
{
public:
private:
	std::vector<std::shared_ptr<abstract_driver_t>> m_drivers;
};

#endif // H_VIRTUAL_FLIGHT_CONTROLLER