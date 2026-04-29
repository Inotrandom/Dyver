#ifndef DYVER_CLI_H
#define DYVER_CLI_H

#include "cache/cache_manager.h"
#include "strutils.h"
#include "delegate.h"
#include <iostream>
#include <thread>
#include <cstring>
#include <filesystem>
#include <functional>

typedef delegate_t<std::function<void(std::string)>> oninput_delegate_t;

class cli_t
{
public:
	static bool NO_LOG;

	explicit cli_t(std::string name)
	{
		m_name = name;
		NO_LOG = true;
		m_oninput = std::make_shared<oninput_delegate_t>();
	}
	~cli_t() {}

	void init(bool block = false)
	{
		clear();
		m_thread = std::thread(&cli_t::input_loop, this);

		if (block == false)
		{
			m_thread.detach();
			return;
		}

		m_thread.join();
	}

	void showcmdline()
	{
		std::cout << "\033[32m" << std::filesystem::current_path().generic_string() << "\033[0m\033[34m " << m_name << "\033[0m \033[1m->\033[0m" << std::flush;
	}

	void input_loop()
	{
		showcmdline();
		std::string buf = std::string();
		while (m_alive)
		{
			std::getline(std::cin, buf);

			std::vector<std::string> split = string_split(buf, "&&");

			for (const auto &s : split)
			{
				parse(s);
			}

			showcmdline();
		}
	}

	void parse(std::string what)
	{
		if (what.empty())
		{
			return;
		}

		std::string lower = to_lower(what);
		string_trim(lower);

		std::vector<std::string> tokens = string_split(lower, " ");
		std::string id = tokens[0];
		tokens.erase(tokens.begin());

		m_oninput->call(lower);

		if (lower == "exit")
		{
			std::cout << "Program forcefully exited from command line" << std::endl;
			exit(EXIT_SUCCESS);
		}

		if (lower == "clear")
		{
			clear();
			return;
		}

		if (lower == "cli-unblock")
		{
			NO_LOG = false;
			std::string buf;
			std::cout << "Press ENTER to end...\n\n" << std::flush;

			std::cin.get();

			NO_LOG = true;

			/*
			while (NO_LOG == false)
			{
				while (std::cin >> buf)
				{
					if (buf.find( char(0x0ab)) != std::string::npos)
					{
						NO_LOG = true;
						break;
					}
				}
			}
				*/
			return;
		}

		if (id == "wcache")
		{
			if (tokens.size() < 3)
			{
				return;
			}

			std::string name = tokens[0];
			std::string key = tokens[1];
			tokens.erase(tokens.begin(), tokens.begin() + 2);
			std::string pair = vector_collect(tokens, " ");

			cache_manager_t cache = cache_manager_t(name);
			cache.load_cache();
			cache.write_buf(key, pair);
			cache.rebuild_cache();
		}

		if (id == "rcache")
		{
			if (tokens.size() < 2)
			{
				return;
			}

			std::string name = tokens[0];
			std::string key = tokens[1];

			cache_manager_t cache = cache_manager_t(name);
			cache.load_cache();
			std::cout << cache.read_buf_or(key, "<undefined>") << std::endl;
		}

		if (id == "cache-display")
		{
			if (tokens.size() < 1)
			{
				return;
			}

			std::string name = tokens[0];
			cache_manager_t cache = cache_manager_t(name);
			cache.load_cache();
			std::size_t line_n = 0;
			for (auto &[k, p] : *cache.get_cache())
			{
				++line_n;
				std::cout << line_n << " : " << k << " = " << p << std::endl;
			}
		}
	}

	void clear()
	{
#if defined _WIN32
		system("cls");
#elif defined(__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
		system("clear");
#elif defined(__APPLE__)
		system("clear");
#endif
	}

	auto get_on_input() -> std::shared_ptr<oninput_delegate_t> { return m_oninput; }

private:
	std::thread m_thread;
	std::string m_name;
	bool m_alive = true;

	void esc() { std::cout << "\033["; }
	std::shared_ptr<oninput_delegate_t> m_oninput = nullptr;
};

namespace utils
{
enum MSG_TYPE
{
	INFO,
	WARN,
	ERROR
};

/**
 * @brief Log a nicely formatted string in the standard out
 *
 * @param type The type of the message (e.g. MSG_TYPE::WARN)
 * @param s The string message to output
 */
void log(std::string s, MSG_TYPE type = MSG_TYPE::INFO);
} // namespace utils

#endif // DYVER_CLI_H