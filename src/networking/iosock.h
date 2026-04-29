#ifndef IOSOCK_H
#define IOSOCK_H

#include <chrono>
#include <functional>
#include <thread>
#include <memory>

#include <netinet/in.h>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "delegate.h"

#include "cli/cli.h"

#include "networking_key.h"

typedef delegate_t<std::function<void(const std::string)>> onrx_delegate_t;

inline auto isfd_valid(int fd) -> bool { return (fd != INVALID); }
inline void close_fd_if_valid(int fd)
{
	if (isfd_valid(fd) == false)
	{
		return;
	}

	close(fd);
}

class client_handler_t
{
public:
	explicit client_handler_t(int fd, std::shared_ptr<onrx_delegate_t> onrx)
	{
		m_fd = fd;
		m_onrx = onrx;
	}
	~client_handler_t() {}

	void begin()
	{
		std::thread(
			[this]()
			{
				while (m_valid)
				{
					char buf[MSG_SIZE] = {0};
					int res = recv(m_fd, &buf, MSG_SIZE, 0);

					if (res == INVALID)
					{
						continue;
					}

					if (buf[0] != '\0')
					{
						m_onrx->call(std::string(buf));
					}

					buf[0] = '\0';
				}
			})
			.detach();
	}

	void kill()
	{
		close(m_fd);
		m_valid = false;
	}

private:
	bool m_valid = true;
	std::shared_ptr<onrx_delegate_t> m_onrx;
	int m_fd = INVALID;
};

/**
 * @brief Useful for peer-to-peer communication.
 *
 */
class iosock_t
{
public:
	explicit iosock_t() { m_onrx = std::make_shared<onrx_delegate_t>(); }

	~iosock_t()
	{
		close_fd_if_valid(m_ofd);
		close_fd_if_valid(m_ifd);
	}

	auto operator=(const iosock_t &) = delete;
	explicit iosock_t(const iosock_t &) = delete;

	/**
	 * @brief Initialize both send and recieve, blocking until they're both connected
	 *
	 * @param my_port Port of the "my" socket on this end.
	 * @param their_port Port of the "their" socket on the other end.
	 */
	void init(int my_port, int their_port, bool out = false, bool in = false, std::string send_to_address = "127.0.0.1",
		std::string recieve_from_address = "0.0.0.0", bool extended_retry = false)
	{
		m_extended_retry = extended_retry;

		if (in == out && send_to_address == recieve_from_address)
		{
			throw std::runtime_error("Cannot perform both I/O if the source and destination addresses are the same");
		}

		utils::log("(iosock_t) Initializing iosock");
		std::thread *handle_send = nullptr;
		if (out)
		{
			handle_send = new std::thread(&iosock_t::open_out, this, my_port, recieve_from_address);
		}

		std::thread *handle_recieve = nullptr;
		if (in)
		{
			handle_recieve = new std::thread(&iosock_t::open_in, this, their_port, send_to_address);
		}

		if (handle_send)
		{

			handle_send->detach();
		}

		if (handle_recieve)
		{
			handle_recieve->detach();
		}
	}

	auto get_onrx() -> std::shared_ptr<onrx_delegate_t> { return m_onrx; }

	void operator<<(const std::string msg)
	{
		if (m_i_connected == false)
		{
			return;
		}

		send(m_ifd, msg.c_str(), MSG_SIZE, 0);
	}

	void kill()
	{
		if (m_ofd == INVALID && m_ifd == INVALID && m_client_handlers.empty())
		{
			return;
		}

		for (auto c : m_client_handlers)
		{
			c->kill();
		}
		m_client_handlers.clear();

		shutdown(m_ofd, SHUT_RD);
		close(m_ofd);
		m_ofd = INVALID;
		shutdown(m_ifd, SHUT_WR);
		close(m_ifd);
		m_ifd = INVALID;

		m_accepted_o_connections = 0;
		m_i_connected = false;
		utils::log("(iosock_t::kill) Iosock kill completed");
	}

	auto is_connected() -> bool { return (m_i_connected == true && m_accepted_o_connections > 0); }

private:
	void open_in(int iport, std::string i_inet_address)
	{
		// Don't open a new one if you don't have to
		if (isfd_valid(m_ifd) == true)
		{
			return;
		}

		std::this_thread::yield();
		m_ifd = socket(AF_INET, SOCK_STREAM, PROTOCOL_AUTO);

		std::string i_logsig = "(socket " + std::to_string(m_ifd) + ")";

		if (isfd_valid(m_ifd) == false)
		{
			return;
		}

		sockaddr_in their_address = sockaddr_in();
		their_address.sin_family = AF_INET;
		their_address.sin_port = htons(iport);
		their_address.sin_addr.s_addr = inet_addr(i_inet_address.c_str());

		utils::log(i_logsig + " Attempting to connect to port " + std::to_string(iport) + " at " + i_inet_address);

		connection_loop(their_address, i_logsig);

		if (m_i_connected == true)
		{
			utils::log(i_logsig + " Connection success");
		}
	}

	// TODO: Improve this and expose it so a retry is easier
	void connection_loop(sockaddr_in their_address, std::string i_logsig)
	{
		int max_retries = MAX_RETRIES;
		if (m_extended_retry == true)
		{
			max_retries = MAX_RETRIES_EXTENDED;
		}

		int attempt = 0;
		while (m_i_connected == false)
		{
			++attempt;

			if (attempt > max_retries)
			{
				utils::log(i_logsig + " Connection timed out after " + std::to_string(max_retries) + " failed attempts.", utils::MSG_TYPE::ERROR);
				return;
			}

			if (isfd_valid(m_ifd) == false)
			{
				utils::log(i_logsig + "Connection terminated after socket was forcibly closed", utils::MSG_TYPE::ERROR);
				return;
			}

			int connect_res = connect(m_ifd, (struct sockaddr *)&their_address, sizeof(their_address));

			if (connect_res == 0)
			{
				m_i_connected = true;
				return;
			}

			if (connect_res == INVALID)
			{
				utils::log(
					i_logsig + " Failure to connect. Attempt " + std::to_string(attempt) + " out of " + std::to_string(max_retries), utils::MSG_TYPE::WARN);
				std::this_thread::sleep_for(std::chrono::seconds(1));
				continue;
			}
		}
	}

	void open_out(int oport, std::string o_inet_address)
	{
		// Don't open a new one if you don't have to
		if (isfd_valid(m_ofd) == true)
		{
			return;
		}

		m_ofd = socket(AF_INET, SOCK_STREAM, PROTOCOL_AUTO);
		std::string o_logsig = "(socket " + std::to_string(m_ofd) + ")";

		if (isfd_valid(m_ofd) == false)
		{
			return;
		}

		sockaddr_in m_my_address = sockaddr_in();
		m_my_address.sin_family = AF_INET;
		m_my_address.sin_port = htons(oport);
		m_my_address.sin_addr.s_addr = inet_addr(o_inet_address.c_str());

		int reuseaddr_val = 1;
		if (setsockopt(m_ofd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_val, sizeof(int)) < 0)
		{
			utils::log(o_logsig + " Failure to configure socket options", utils::MSG_TYPE::ERROR);
			return;
		}

		utils::log(o_logsig + " Attempting to bind to port " + std::to_string(oport) + ", only listening to connections from " + o_inet_address);

		int bind_res = bind(m_ofd, (const struct sockaddr *)&m_my_address, sizeof(m_my_address));
		if (bind_res == INVALID)
		{
			utils::log(o_logsig + " Failure to bind.", utils::MSG_TYPE::ERROR);
			return;
		}

		utils::log(o_logsig + " Bind success, waiting for at most " + std::to_string(MAX_CONNECTIONS) + " connections");
		(void)listen(m_ofd, MAX_CONNECTIONS);

		std::thread accept_handle = std::thread(&iosock_t::accept_handler, this);
		accept_handle.detach();
	}

	void accept_handler()
	{
		bool fully_accepted = false;
		while (fully_accepted == false)
		{
			int accepted = accept(m_ofd, nullptr, nullptr);
			if (accepted == INVALID)
			{
				// Likely due to being terminated
				// utils::log("(socket " + std::to_string(m_ofd) + ") Failed to accept file descriptor", utils::MSG_TYPE::ERROR);
				std::this_thread::sleep_for(std::chrono::seconds(1));
				return;
			}

			utils::log("(socket " + std::to_string(m_ofd) + ") Connection accepted");

			open_client_handler(accepted);
		}
	}

	void open_client_handler(int fd)
	{
		++m_accepted_o_connections;
		std::shared_ptr<client_handler_t> new_handler = std::make_shared<client_handler_t>(fd, m_onrx);
		new_handler->begin();
	}

	std::vector<std::shared_ptr<client_handler_t>> m_client_handlers;

	std::shared_ptr<onrx_delegate_t> m_onrx = nullptr;
	int m_ofd = INVALID;
	int m_ifd = INVALID;

	bool m_extended_retry = false;

	std::string m_o_inet_address = std::string();
	std::string m_i_inet_address = std::string();

	bool m_i_connected = false;
	std::size_t m_accepted_o_connections = 0;
};

#endif // IOSOCK_H