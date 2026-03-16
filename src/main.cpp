#include "dss/runtime.h"
#include "dss/DSS.h"

#include <thread>

#include "app.h"

const std::string MAIN_CLI_NAME = "dyver-dss";
void command_line(DSS::cli_t *p_cli) { p_cli->init(); }

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

	DSS::cli_t *p_cli = new DSS::cli_t(main_executor, MAIN_CLI_NAME);

	// Dyver
	app_t app = app_t(main_executor);

	// Move DSS onto separate thread
	std::thread cli_handle(command_line, p_cli);

	// Run Dyver
	app.run();

	p_cli->kill();
	cli_handle.join();

	return 0;
}