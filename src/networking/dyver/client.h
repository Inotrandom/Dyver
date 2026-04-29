/**
 * @file client.h
 * @author Estelle Coonan (estellecoonan@gmail.com)
 * @brief Robot client
 * @date 2026-04-27
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "networking/iosock.h"
#include "cache/cache_manager.h"
#include "cli/cli.h"
#include "utils.h"

class client_t
{
public:
	explicit client_t()
	{
		m_plaintext = std::make_shared<iosock_t>();
		m_video = std::make_shared<iosock_t>();
	}

	~client_t() { kill(); }

	void init()
	{
		utils::log("(dyver client) Beginning initialization...");

#ifndef FLAG_DYVER_TEST
		cache_manager_t client_cache = cache_manager_t("client");
		client_cache.load_cache();
		std::string send_address = client_cache.read_buf_or("their_address", "0.0.0.0");
		std::string recv_address = client_cache.read_buf_or("my_address", "0.0.0.0");

		cache_manager_t robot_cache = cache_manager_t("robot");
		bool is_daemon = utils::from_yn(client_cache.read_buf_or("is_daemon", "no"));

		m_plaintext->init(PORT_PLAINTEXT, PORT_PLAINTEXT, true, true, send_address, recv_address, is_daemon);
		m_video->init(PORT_VIDEO, PORT_VIDEO, true, false, send_address, recv_address, is_daemon);
#endif
#ifdef FLAG_DYVER_TEST
		utils::log("(dyver client) NETWORKING TEST ENABLED");
		m_plaintext->init(PORT_PLAINTEXT, PORT_PLAINTEXT, true, true, "127.0.0.2", "127.0.0.6");
		m_video->init(PORT_VIDEO, PORT_VIDEO, true, false, "127.0.0.2", "0.0.0.0");
#endif

		m_plaintext->get_onrx()->connect(
			[this](std::string msg)
			{
				if (msg == COMMAND_PING)
				{
					utils::log("(dyver client) Ping recieved!");
					utils::log("(dyver client) Returning a pong...");
					*m_plaintext << RESPONSE_PONG;
				}
			});
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