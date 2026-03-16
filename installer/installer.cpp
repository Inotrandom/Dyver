#include "question.h"

auto main(int argv, char **argc) -> int
{
	(void)argv;
	(void)argc;

	yesno_question_t test = yesno_question_t("Do you want to proceed with the installation?");

	bool result = test.ask(true);

	if (result == false)
	{
		return 0;
	}

	option_question_t other_test = option_question_t("Select an interface to install.", {"Dyver Client", "Dyver Topside"});

	std::cout << other_test.ask() << std::endl;

	return 0;
}