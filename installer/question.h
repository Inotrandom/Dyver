#ifndef H_QUESTION
#define H_QUESTION

#include <string>
#include <iostream>

#include "strutils.h"

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

class question_t
{
public:
	explicit question_t() {}
	~question_t() {}

	virtual auto ask(bool answer_default = false) const -> bool = 0;

private:
};

class yesno_question_t : question_t
{
public:
	explicit yesno_question_t() {}

	explicit yesno_question_t(const std::string body) { m_body = body; }

	~yesno_question_t() {}

	auto ask(bool answer_default = false) const -> bool override
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
	std::string m_body;
};

#endif