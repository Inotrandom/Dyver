#ifndef H_QUESTION
#define H_QUESTION

#include <iostream>

#include <string>
#include <sstream>
#include <vector>

#include <cstddef>

#include "strutils.h"

#define QUESTION_GIVE_UP_AFTER 1000

inline auto yes_in(std::string &which, bool answer_default = false) -> bool
{
	std::string lower = to_lower(which);

	if (lower.find('n') != std::string::npos)
	{
		return false;
	}

	if (lower.find('y') != std::string::npos)
	{
		return true;
	}

	return answer_default;
}

class yesno_question_t
{
public:
	explicit yesno_question_t() {}

	explicit yesno_question_t(const std::string body) { m_body = body; }

	~yesno_question_t() {}

	auto ask(bool answer_default = false) const -> bool
	{
		std::cout << m_body << std::endl;

		if (answer_default == true)
		{
			std::cout << "Please indicate [Y/n] : ";
		}
		else
		{
			std::cout << "Please indicate [y/N] : ";
		}

		std::string ans;

		std::getline(std::cin, ans);

		return yes_in(ans, answer_default);
	}

private:
	std::string m_body = std::string();
};

class option_question_t
{
public:
	explicit option_question_t() {}

	explicit option_question_t(const std::string body, const std::vector<std::string> choices)
	{
		m_body = body;
		m_choices = choices;

		m_choices.push_back("cancel");
	}

	~option_question_t() {}

	auto ask() const -> std::string
	{
		std::size_t choice_i = 0;
		std::string buf = "";

		for (const auto &choice : m_choices)
		{
			++choice_i;

			std::cout << choice_i << " : " << choice << "\n";
		}

		std::cout << std::endl;

		for (std::size_t i = 0; i < QUESTION_GIVE_UP_AFTER; ++i)
		{
			std::cout << "Please indicate a number : ";

			std::getline(std::cin, buf);

			std::stringstream s;
			std::size_t number_ans;

			s << buf;
			s >> number_ans;

			if (number_ans == 0)
			{
				std::cout << "Invalid option \"" << buf << "\" (note: input an integer number)" << std::endl;
				continue;
			}

			if (number_ans > m_choices.size())
			{
				std::cout << "Non-existent option \"" << number_ans << "\"" << std::endl;
				continue;
			}

			return m_choices[number_ans - 1];
		}

		std::cout << "Exhausted choice." << std::endl;
		exit(1);
	}

private:
	std::string m_body = std::string();
	std::vector<std::string> m_choices = std::vector<std::string>();
};

#endif