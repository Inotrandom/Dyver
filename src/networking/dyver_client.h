#ifndef H_NW_CLIENT
#define H_NW_CLIENT

#include <memory>
#include <string>

#include "socket_helper.h"
#include "networking_key.h"

class dyver_client_t
{
public:
	explicit dyver_client_t() {}
	~dyver_client_t() {}

	auto initialize(std::string inet_address) -> bool
	{
		utils::log("(dyver client) Initialization sequence started...");

		m_dss = std::make_shared<listen_socket_t>();
		m_camera = std::make_shared<send_socket_t>();
		m_telemetry = std::make_shared<send_socket_t>();

		std::uint8_t exhaust = 0;

		utils::log("(dyver client) Initializing DSS socket.");
		m_dss->initialize(PORT_DSS);

		while (m_connected == false)
		{
			++exhaust;
			if (exhaust > KILL_INIT_AFTER)
			{
				throw std::runtime_error("Failure to initialize client (connection terminated after " + std::to_string(KILL_INIT_AFTER) + " attempts.)");
				break;
			}

			if (m_camera->is_connected() == false)
			{
				utils::log("(dyver client) Connecting camera socket.");
				m_camera->connect_to(PORT_VIDEO, inet_address);
			}

			if (m_telemetry->is_connected() == false)
			{
				utils::log("(dyver client) Connecting telemetry socket.");
				m_telemetry->connect_to(PORT_TELEMETRY, inet_address);
			}

			if (m_dss->is_connected() == false)
			{
				m_dss->accept_n(1);
			}

			m_connected = (m_dss->is_connected() && m_camera->is_connected() && m_telemetry->is_connected());

			std::this_thread::sleep_for(std::chrono::seconds(INIT_WAIT_S));
		}

		if (m_connected == false)
		{
			return false;
		}

		rx_thread(&(*m_dss), m_dss->get_connections().front());
		m_dss->get_onrx()->connect(
			[this](const std::string msg)
			{
				if (msg != COMMAND_PING)
				{
					return;
				}

				send_pong();
			});

		utils::log("(dyver client) Initialization completed successfully.");
		return true;
	}

	void kill()
	{
		if (m_dss)
		{
			m_dss->kill();
		}

		if (m_camera)
		{
			m_camera->kill();
		}

		if (m_telemetry)
		{
			m_telemetry->kill();
		}
	}

private:
	void send_pong() { m_telemetry->tx(RESPONSE_PONG); }

	bool m_connected = false;

	std::shared_ptr<listen_socket_t> m_dss = nullptr;
	std::shared_ptr<send_socket_t> m_camera = nullptr;
	std::shared_ptr<send_socket_t> m_telemetry = nullptr;
};

#endif // H_NW_CLIENT