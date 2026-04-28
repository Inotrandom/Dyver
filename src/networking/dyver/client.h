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

class client_t
{
public:
	explicit client_t()
	{
		m_plaintext = std::make_shared<iosock_t>();
		m_video = std::make_shared<iosock_t>();
	}

	~client_t() {}

	void init()
	{
#ifndef FLAG_DYVER_TEST
		m_plaintext->init(PORT_PLAINTEXT, PORT_PLAINTEXT, true, true, "0.0.0.0", "0.0.0.0");
		m_video->init(PORT_VIDEO, PORT_VIDEO, true, false, "0.0.0.0", "0.0.0.0");
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