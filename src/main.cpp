#include "dss/runtime.h"
#include "dss/DSS.h"

#include <thread>

#include "app.h"

const std::string MAIN_CLI_NAME = "dyver-dss";
void command_line(std::shared_ptr<DSS::executor_t> p_exec)
{
	DSS::cli_t cli = DSS::cli_t(p_exec, MAIN_CLI_NAME);
	cli.init();
}

auto main(int argv, char **argc) -> int
{
	(void)argv;
	(void)argc;

	// DSS
	DSS::environment_t env = DSS::environment_t();

	env.init();
	std::shared_ptr<DSS::executor_t> main_executor = env.main_executor();

	if (main_executor == nullptr)
	{
		return 1;
	}

	// Dyver
	app_t app = app_t(main_executor);

	// Move DSS onto separate thread
	std::thread cli_handle(command_line, main_executor);

	// Run Dyver
	app.run();

	// Wait for the CLI to kill itself
	cli_handle.join();

	return 0;
}