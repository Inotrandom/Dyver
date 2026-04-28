#ifndef H_NW_SERVER
#define H_NW_SERVER

// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <unistd.h>
#include <cstdint>

#include <thread>
#include <chrono>

#include <string>
#include <memory>

#include "networking_key.h"
#include "socket_helper.h"
#include "utils.h"

class dyver_server_t
{
public:
	explicit dyver_server_t() {}
	~dyver_server_t() {}

	/**
	 * @brief Initialize the server
	 *
	 * @param inet_address Address of the client robot
	 * @return true Initialization success
	 * @return false Initialization failure
	 */
	bool initialize(std::string inet_address)
	{
		utils::log("(dyver server) Initialization sequence started...");

		m_dss = std::make_shared<send_socket_t>();
		m_camera = std::make_shared<listen_socket_t>();
		m_telemetry = std::make_shared<listen_socket_t>();

		std::uint8_t exhaust = 0;

		utils::log("(dyver server) Opening camera socket.");
		m_camera->initialize(PORT_VIDEO);
		utils::log("(dyver server) Opening telemetry socket.");
		m_telemetry->initialize(PORT_TELEMETRY);

		while (m_connected == false)
		{
			++exhaust;
			if (exhaust > KILL_INIT_AFTER)
			{
				throw std::runtime_error("Failure to initialize server (connection terminated after " + std::to_string(KILL_INIT_AFTER) + " attempts.)");
				break;
			}

			if (m_camera->is_connected() == false)
			{
				m_camera->accept_n(1);
			}
			if (m_telemetry->is_connected() == false)
			{
				m_telemetry->accept_n(1);
			}

			if (m_dss->is_connected() == false)
			{
				utils::log("(dyver server) Connecting DSS socket.");
				m_dss->connect_to(PORT_DSS, inet_address);
			}
			m_connected = (m_camera->is_connected() && m_telemetry->is_connected() && m_dss->is_connected());

			std::this_thread::sleep_for(std::chrono::seconds(INIT_WAIT_S));
		}

		if (m_connected == false)
		{
			return false;
		}

		rx_thread(&(*m_camera), m_camera->get_connections().front());
		rx_thread(&(*m_telemetry), m_telemetry->get_connections().front());

		utils::log("(dyver server) Initialization completed successfully", utils::MSG_TYPE::INFO);
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

	void verify_inet_connection()
	{
		m_dss->tx(COMMAND_PING);
		m_telemetry->get_onrx()->connect(
			[](const std::string msg)
			{
				if (msg != RESPONSE_PONG)
				{
					return;
				}

				utils::log("(server) Pong recieved!", utils::MSG_TYPE::INFO);
			});
	}

	auto get_dss() -> std::shared_ptr<send_socket_t> { return m_dss; }
	auto get_camera() -> std::shared_ptr<listen_socket_t> { return m_camera; }
	auto get_telemetry() -> std::shared_ptr<listen_socket_t> { return m_telemetry; }

private:
	bool m_connected = false;

	std::shared_ptr<send_socket_t> m_dss = nullptr;
	std::shared_ptr<listen_socket_t> m_camera = nullptr;
	std::shared_ptr<listen_socket_t> m_telemetry = nullptr;
};

#endif