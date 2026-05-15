#ifndef H_STR_UTILS
#define H_STR_UTILS

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <sstream>

/**
 * Splits a string into multiple substrings, separated by a delimiter.
 *
 * @param s The input string to be operated over
 * @param delimiter A delimiter to determine how tokens are separated
 *
 * @returns A vector of substrings after having been separated by delimiter.
 */
inline auto string_split(std::string s, const std::string &delimiter) -> std::vector<std::string>
{
	std::vector<std::string> tokens = {};
	std::size_t pos = 0;
	std::string token = "";

	while ((pos = s.find(delimiter)) != std::string::npos)
	{
		token = s.substr(0, pos);
		tokens.push_back(token);
		s.erase(0, (pos + delimiter.length()));
	}
	tokens.push_back(s);

	if (tokens.empty())
	{
		tokens.push_back(s);
	}

	return tokens;
}

inline auto string_split_whitespace(std::string s) -> std::vector<std::string>
{
	std::vector<std::string> tokens = {};
	std::stringstream buf = std::stringstream();
	buf << s;
	std::string token = "";
	while (buf >> token)
	{
		tokens.push_back(token);
	}

	return tokens;
}

inline auto last_in_path(std::string s) -> std::string
{
	std::vector<std::string> tokens = string_split(s, "/");
	return tokens.back();
}

/**
 * Finds all occurances and replaces them in a string.
 *
 * @param s The string to be operated over
 *
 * @param what The string to match occurences in `s`
 *
 * @param with The string to replace `what` with
 */
inline void string_replace(std::string &s, std::string what, const std::string with)
{
	std::size_t pos = 0;

	while ((pos = s.find(what)) != std::string::npos)
	{
		s.replace(pos, what.size(), with);
	}
}

inline auto vector_collect(std::vector<std::string> &v, const std::string delim = "") -> std::string
{
	std::stringstream res;

	for (auto s : v)
	{
		res << s << delim;
	}

	std::string s_res = res.str();
	std::string without_trailing = s_res.substr(0, (s_res.size() - delim.size()));

	return without_trailing;
}

inline auto trim_first_and_last(std::string s) -> std::string { return s.substr(1, s.size() - 2); }

inline auto in_str(const std::string &s, const std::string what) -> bool { return (s.find(what) != std::string::npos); }

inline auto trim_suffix(std::string s, const std::string which) -> std::string { return (s.substr(0, s.size() - which.size())); }

/// @brief Trim from the start (in place)
inline void string_ltrim(std::string &s)
{
	if (s.length() == 0)
		return;

	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
}

/// @brief Trim from the end (in place)
inline void string_rtrim(std::string &s)
{
	if (s.length() == 0)
		return;

	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
}

/// @brief Trim from both ends (in place)
inline void string_trim(std::string &s)
{
	string_rtrim(s);
	string_ltrim(s);
}

inline auto to_lower(std::string &s) -> std::string
{
	std::string res = s;
	std::transform(s.cbegin(), s.cend(), res.begin(), [](const char c) { return std::tolower(c); });
	return res;
}

inline auto string_safe_dcast(const std::string &s) -> double
{
	double res = 0.0;
	std::stringstream stream;
	stream << s;
	stream >> res;

	return res;
}

#endif // H_STR_UTILS