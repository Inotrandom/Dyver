/**
 * @file robot.cpp
 * @author Estelle Coonan (estellecoonan@domain.com)
 * @brief Robot computer daemon
 * @date 2026-03-31
 *
 * @copyright Copyright (c) 2026
 *
 */

#include <cstring>

#include "cache/cache_manager.h"
#include "cli/cli.h"
#include "networking/dyver/client.h"
#include "robot/driver/abstract_driver.h"
#include "utils.h"

#define ROBOT_TARGET_LINUX

#include "robot/driver/imu/ISM330DHCX-MMC5983MA.h"

struct robot_options_t
{
	bool daemon = false;
	bool tests = false;
};

void hardware_tests()
{
	std::shared_ptr<ISM330DHCX_MMC5983MA_t> imu = std::make_shared<ISM330DHCX_MMC5983MA_t>();
	for (;;)
	{
		driver_packet_t pack = imu->read();
		if (pack.empty())
		{
			utils::log("Empty data packet.", utils::MSG_TYPE::WARN);
		}
		utils::log(std::any_cast<std::string>(pack[common_data_headers::TEMPERATURE_C]));
	}
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
	}

	client_t client = client_t();
	cache_manager_t robot_cache = cache_manager_t("robot");

	robot_cache.load_cache();
	robot_cache.write_buf("is_daemon", utils::to_yn(opt.daemon));
	robot_cache.rebuild_cache();

	if (opt.tests == true)
	{
		hardware_tests();
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