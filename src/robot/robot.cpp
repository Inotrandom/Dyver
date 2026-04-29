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
#include "wiringPi.h"
#include "utils.h"

auto main(int argc, char **argv) -> int
{
	(void)argv;
	(void)argc;

	bool daemon = false;
	if (argc > 1)
	{
		if (strcmp(argv[1], "--daemon") == 0)
		{
			daemon = true;
		}
	}

	client_t client = client_t();
	cache_manager_t robot_cache = cache_manager_t("robot");

	robot_cache.load_cache();
	robot_cache.write_buf("is_daemon", utils::to_yn(daemon));
	robot_cache.rebuild_cache();

	if (daemon == false)
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