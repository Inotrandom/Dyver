/**
 * @file robot.cpp
 * @author Estelle Coonan (estellecoonan@domain.com)
 * @brief Robot computer daemon
 * @date 2026-03-31
 *
 * @copyright Copyright (c) 2026
 *
 */

#include <iostream>

#include "wiringPi.h"

auto main(int argv, char **argc) -> int
{
	(void)argv;
	(void)argc;

	std::cout << "Hello, robot!\n";

	return 0;
}