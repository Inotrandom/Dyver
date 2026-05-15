#ifndef BESCR3_H
#define BESCR3_H

#include "robot/driver/thruster_driver.h"
#include "BESCR3_T200.h"

enum BESCR3_CONFIG
{
	/**
	 * @brief Assume a T200 thruster is connected to the ESC
	 *
	 */
	T200
};

class bescr3_t : thruster_driver_t
{
public:
	explicit bescr3_t() {}
	~bescr3_t() {}

	auto set_throttle(double t) -> int override
	{
		switch (m_mode)
		{
		case (BESCR3_CONFIG::T200):
		{
		}
		}
	}

private:
	BESCR3_CONFIG m_mode = BESCR3_CONFIG::T200;
};

#endif // BESCR3_H