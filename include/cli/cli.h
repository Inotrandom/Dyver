#ifndef DYVER_CLI_H
#define DYVER_CLI_H

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

	explicit cli_t()
	{
		NO_LOG = true;
		m_oninput = std::make_shared<oninput_delegate_t>();
	}
	~cli_t() {}

	void init()
	{
		clear();
		m_thread = std::thread(&cli_t::input_loop, this);
		m_thread.detach();
	}

	void showcmdline()
	{
		std::cout << "\033[32m" << std::filesystem::current_path().generic_string() << "\033[0m\033[34m Dyver Topside CLI \033[0m \033[1m->\033[0m"
				  << std::flush;
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
		std::string lower = to_lower(what);
		string_trim(lower);
		m_oninput->call(lower);

		if (lower == "exit")
		{
			std::cout << "Program forcefully exited from command line" << std::endl;
			exit(EXIT_SUCCESS);
		}

		if (lower == "cli-unblock")
		{
			NO_LOG = false;
			std::string buf;
			std::cout << "Press ENTER to end...\n\n" << std::flush;

			std::cin.get();

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
	bool m_alive = true;

	void esc() { std::cout << "\033["; }
	std::shared_ptr<oninput_delegate_t> m_oninput = nullptr;
};

#endif // DYVER_CLI_H