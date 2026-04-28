#include "cli.h"

bool cli_t::NO_LOG = false;

void utils::log(std::string s, MSG_TYPE type)
{
	if (cli_t::NO_LOG == true)
	{
		return;
	}

	switch (type)
	{
	case (MSG_TYPE::WARN):
	{
		std::cout << "\033[93m[warning] ";
		break;
	}

	case (MSG_TYPE::INFO):
	{
		std::cout << "\033[0m[info] ";
		break;
	}

	case (MSG_TYPE::ERROR):
	{
		std::cout << "\033[91m[error] ";
		break;
	}
	}

	std::cout << s;

	std::cout << "\033[0m\n" << std::flush;
}