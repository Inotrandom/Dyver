/**
 * @file server.h
 * @author Estelle Coonan (estellecoonan@gmail.com)
 * @brief Topside server
 * @date 2026-04-27
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "networking/iosock.h"
#include <memory>
#include "cache/cache_manager.h"
#include "cli/cli.h"

class server_t
{
public:
	explicit server_t()
	{
		m_plaintext = std::make_shared<iosock_t>();
		m_video = std::make_shared<iosock_t>();
	}

	~server_t() { kill(); }

	void init()
	{
		utils::log("(dyver server) Beginning initialization...");
#ifndef FLAG_DYVER_TEST
		cache_manager_t cache = cache_manager_t("server");
		cache.load_cache();
		std::string send_address = cache.read_buf_or("their_address", "0.0.0.0");
		std::string recv_address = cache.read_buf_or("my_address", "0.0.0.0");

		m_plaintext->init(PORT_PLAINTEXT, PORT_PLAINTEXT, true, true, send_address, recv_address);
		m_video->init(PORT_VIDEO, PORT_VIDEO, false, true, send_address, recv_address);
#endif
#ifdef FLAG_DYVER_TEST
		utils::log("(dyver server) NETWORKING TEST ENABLED");
		m_plaintext->init(PORT_PLAINTEXT, PORT_PLAINTEXT, true, true, "127.0.0.6", "127.0.0.2");
		m_video->init(PORT_VIDEO, PORT_VIDEO, false, true, "0.0.0.0", "127.0.0.2");
#endif
	}

	bool verify_connection()
	{
		if (m_plaintext == nullptr)
		{
			return false;
		}

		bool res = false;
		utils::log("(dyver server) Ping...");

		std::function<void(std::string)> fn = [&res](std::string msg)
		{
			if (msg == RESPONSE_PONG)
			{
				utils::log("(dyver server) Pong recieved!");
				res = true;
				return;
			}
		};
		m_plaintext->get_onrx()->connect(fn);
		*m_plaintext << COMMAND_PING;

		// TODO: Prevent from yielding forever
		while (res == false)
		{
			std::this_thread::yield();
		}

		return true;
	}

	void kill()
	{
		if (m_plaintext)
		{
			m_plaintext->kill();
		}

		if (m_video)
		{
			m_video->kill();
		}
	}

private:
	std::shared_ptr<iosock_t> m_plaintext;
	std::shared_ptr<iosock_t> m_video;
};