#include "question.h"

auto main(int argv, char **argc) -> int
{
	(void)argv;
	(void)argc;

	yesno_question_t test = yesno_question_t("Do you want to proceed with the installation?");

	bool result = test.ask(true);
	std::cout << result << std::endl;

	return 0;
}