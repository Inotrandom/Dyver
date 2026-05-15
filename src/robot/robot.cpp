/**
 * @file robot.cpp
 * @author Estelle Coonan (estellecoonan@domain.com)
 * @brief Robot computer daemon
 * @date 2026-03-31
 *
 * @copyright Copyright (c) 2026
 *
 */

#include <cerrno>
#include <cstring>

#include "cache/cache_manager.h"
#include "cli/cli.h"
#include "networking/dyver/client.h"
#include "robot/driver/abstract_driver.h"
#include "pigpio.h"
#include "utils.h"

#define ROBOT_TARGET_LINUX

#include "robot/driver/imu/ISM330DHCX-MMC5983MA.h"

struct robot_options_t
{
	bool daemon = false;
	bool tests = false;
	bool thruster_tests = false;
};

void hardware_tests()
{
	std::shared_ptr<ISM330DHCX_MMC5983MA_t> imu = std::make_shared<ISM330DHCX_MMC5983MA_t>();

	imu->start(driver_packet_t());

	for (;;)
	{
		driver_packet_t pack = imu->read();
		if (pack.empty())
		{
			continue;
		}
		utils::log(std::to_string(std::any_cast<double>(pack[common_data_headers::TEMPERATURE_C])));
	}
}

void user_acknowledge()
{
	std::cout << "Press enter to continue..." << std::endl;
	std::string buf;
	// ew
	std::getline(std::cin, buf);
	return;
}

void thruster_tests()
{
	std::stringstream s;
	int p = -1;

	if (gpioInitialise() == -1)
	{
		utils::log("(pigpio) Unable to initialize.", utils::MSG_TYPE::ERROR);
		return;
	}

	do
	{
		std::cout << "[info] Provide a GPIO pin number.\n->";
		std::string buf;
		std::getline(std::cin, buf);

		s << buf;
		s >> p;

		if (p <= 0)
		{
			std::cout << "[info] Malformed input, pin not set. Please try again." << std::endl;
		}
	} while (p == -1);

	utils::log("This test will initialize the ESC, run the thruster, and then stop the ESC. DO NOT ATTEMPT to terminate the program during this sequence, or "
			   "the ESC may get confused.");
	user_acknowledge();

	utils::log("Check for initialization.");
	gpioServo(p, 1500);
	time_sleep(1.5);
	user_acknowledge();

	utils::log("Check for running.");
	gpioServo(p, 1550);
	time_sleep(1);
	user_acknowledge();

	utils::log("Check for stopping.");
	time_sleep(1);
	gpioServo(p, 0);

	user_acknowledge();
	utils::log("The thruster test has concluded without software malfunction.");
	gpioTerminate();
}

auto main(int argc, char **argv) -> int
{
	(void)argv;
	(void)argc;

	robot_options_t opt = robot_options_t();
	if (argc > 1)
	{
		if (strcmp(argv[1], "--daemon") == 0)
		{
			opt.daemon = true;
		}

		if (strcmp(argv[1], "-t") == 0)
		{
			opt.tests = true;
		}

		if (strcmp(argv[1], "--test-bescr3") == 0)
		{
			opt.thruster_tests = true;
		}
	}

	client_t client = client_t();
	cache_manager_t robot_cache = cache_manager_t("robot");

	robot_cache.load_cache();
	robot_cache.write_buf("is_daemon", utils::to_yn(opt.daemon));
	robot_cache.rebuild_cache();

	if (opt.thruster_tests == true)
	{
		thruster_tests();
		return 0;
	}

	if (opt.tests == true)
	{
		hardware_tests();
		return 0;
	}

	if (gpioInitialise() == -1)
	{
		return -1;
	}

	if (opt.daemon == false)
	{
		cli_t cli = cli_t("Dyver Client CLI");

		cli.get_on_input()->connect(
			[&client](std::string got)
			{
				if (got == "client-start")
				{
					client.init();
				}

				if (got == "client-kill")
				{
					client.kill();
				}
			});

		cli.init(true);
	}
	else
	{
		client.init();
	}

	return 0;
}
