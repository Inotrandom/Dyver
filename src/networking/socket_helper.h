/**
 * @file socket_helper.h
 * @author Estelle Coonan (yacoonan@gmail.com)
 * @brief Boilerplate socket utilities
 * @date 2026-01-20
 *
 * @copyright Copyright (c) 2026
 *
 */

// NOTE: THIS FILE HAS BEEN DEPRECATED!

#ifndef H_SOCKET_HELPER
#define H_SOCKET_HELPER

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include <functional>

#include <chrono>

#include <cstdint>

#include <stdexcept>

#include <map>
#include <sstream>

#include "utils.h"
#include "delegate.h"

#include "networking_key.h"

#define MSG_SIZE 1024

#define MAX_CONNECTION_RETRIES 10
const std::chrono::seconds RETRY_AFTER = std::chrono::seconds(1);

#define INVALID_FD -1

typedef char msg_t[MSG_SIZE];

class listen_socket_t;
class send_socket_t;

void rx_thread(listen_socket_t *p_self, std::int16_t fd);
void tx_thread(send_socket_t *p_self);

struct msg_buf_t
{
	explicit msg_buf_t() {}
	~msg_buf_t() {}

	std::vector<std::string> buf;

	void clear() { buf.clear(); }

	void operator<<(const char *c) { buf.push_back(std::string(c)); }
};

class listen_socket_t
{
public:
	explicit listen_socket_t() { m_onrx = std::make_shared<delegate_t<std::function<void(const std::string)>>>(); }
	~listen_socket_t() { kill(); }

	void initialize(std::uint64_t port)
	{
		m_socket_fd = socket(AF_INET, SOCK_STREAM, PROTOCOL_AUTO);

		if (m_socket_fd == INVALID_FD)
		{
			throw std::runtime_error("Failure to initialize network socket.");
		}

		std::stringstream msg;
		msg << "(listen socket) Listen socket opened. (socket " << m_socket_fd << ")";
		utils::log(msg.str());

		m_address = sockaddr_in();

		// IPV4
		m_address.sin_family = AF_INET;

		// Port
		m_address.sin_port = htons(port);

		// Will listen to anyone from any address
		m_address.sin_addr.s_addr = INADDR_ANY;

		sockaddr *p_sock_addr = (sockaddr *)&m_address;
		int bind_res = bind(m_socket_fd, p_sock_addr, sizeof(m_address));

		if (bind_res < 0)
		{
			utils::log("(listen socket) Failure to produce listen socket. (bind failure, code " + std::to_string(bind_res) + ")", utils::MSG_TYPE::ERROR);
			//	throw std::runtime_error("Listen socket bind failure");
			return;
		}

		(void)listen(m_socket_fd, MAX_CONNECTIONS);
	}

	/**
	 * @brief Accept a number of connections
	 *
	 * @param n The number of connections to accept
	 *
	 * @return true All connections were accepted
	 * @return false One or more connections failed
	 */
	auto accept_n(std::int16_t n) -> bool
	{
		if (is_dead() == true)
		{
			throw std::runtime_error("Dead socket cannot accept connections.");
			return false;
		}

		std::cout << "listen socket is waiting for " << n << " connections (socket " << m_socket_fd << ")" << std::endl;

		int accepted = accept(m_socket_fd, nullptr, nullptr);

		if (accepted < 0)
		{
			std::stringstream msg;
			msg << "(listen socket) Failure to accept socket, invalid file descriptor. (socket " << m_socket_fd << ", resulting fd " << accepted << ")";

			utils::log(msg.str());

			std::this_thread::sleep_for(RETRY_AFTER);

			return false;
		}

		// Log it for the smelly humans
		std::stringstream msg;
		msg << "(listen socket) Socket connection accepted on port " << "\033[4m" << m_address.sin_port << "\033[0m";
		utils::log(msg.str());

		m_connected[accepted] = true;
		// Spawn a handler for it
		std::thread rx_handle = std::thread(rx_thread, this, accepted);
		rx_handle.detach();

		m_connection_status = true;

		return true;
	}

	void kill()
	{
		if (m_socket_fd == INVALID_FD)
			return;

		for (auto [k, p] : m_connected)
		{
			close(k);
		}

		(void)close(m_socket_fd);
		m_socket_fd = INVALID_FD;
		utils::log("(listen socket) Listen socket closed.");
	}

	auto is_dead() -> bool { return (m_socket_fd == INVALID_FD); }

	auto get_out() -> msg_buf_t { return m_msg_buf; }

	auto is_connection_valid(int16_t fd) -> bool
	{
		if (m_connected.find(fd) == m_connected.end())
		{
			return false;
		}

		return m_connected[fd];
	}

	void rx(std::int16_t fd)
	{
		msg_t msg = {0};

		while (is_connection_valid(fd))
		{
			(void)recv(fd, msg, MSG_SIZE, 0);

			if (msg[0] != '\0')
			{
				m_msg_buf << msg;

				m_onrx->call<const std::string>(m_msg_buf.buf.back());
			}

			msg[0] = '\0';
		}

		utils::log("(listen socket) Invalid send socket caught, RX terminated.");
	}

	auto get_connections() -> std::vector<std::int16_t>
	{
		std::vector<std::int16_t> res = {};
		for (const auto &[key, pair] : m_connected)
		{
			res.push_back(key);
		}

		return res;
	}

	auto is_connected() -> bool { return m_connection_status; }

	auto get_onrx() -> std::shared_ptr<delegate_t<std::function<void(const std::string)>>> { return m_onrx; }

	listen_socket_t(const listen_socket_t &) = delete;
	listen_socket_t &operator=(const listen_socket_t &) = delete;

private:
	bool m_connection_status = false;

	sockaddr_in m_address = sockaddr_in();
	int m_socket_fd = INVALID_FD;
	std::map<int, bool> m_connected;
	msg_buf_t m_msg_buf;
	std::string m_id;

	std::shared_ptr<delegate_t<std::function<void(const std::string)>>> m_onrx;
};

class send_socket_t
{
public:
	explicit send_socket_t() {}
	~send_socket_t() { kill(); }

	/**
	 * @brief Attempts to connect to an address and port
	 *
	 * @param port The port of the other computer to connect to
	 * @param inet_address The address of the other computer
	 * @return true Connection succeeded
	 * @return false Connection failed, likely during a timeout
	 */
	auto connect_to(std::uint64_t port, std::string inet_address) -> bool
	{
		m_socket_fd = socket(AF_INET, SOCK_STREAM, PROTOCOL_AUTO);

		if (m_socket_fd == INVALID_FD)
		{
			throw std::runtime_error("(send socket) Failure to initialize network socket.");
		}

		std::stringstream msg;
		msg << "(send socket) Send socket opened. (socket " << m_socket_fd << ")";
		utils::log(msg.str());

		m_address = sockaddr_in();

		// IPV4
		m_address.sin_family = AF_INET;

		// Port
		m_address.sin_port = htons(port);

		std::int16_t address;
		inet_pton(AF_INET, inet_address.c_str(), &address);

		// Will send to the specific address
		m_address.sin_addr.s_addr = INADDR_ANY;

		sockaddr *p_sock_addr = (sockaddr *)&m_address;
		bool has_connected = false;
		std::int16_t i = 0;

		while (has_connected == false)
		{
			++i;

			if (i >= MAX_CONNECTION_RETRIES)
			{
				std::stringstream msg;
				msg << "(send socket) Connection terminated after " << MAX_CONNECTION_RETRIES << " failed attempts. (timeout)";
				utils::log(msg.str(), utils::MSG_TYPE::ERROR);
				return false;
			}

			std::int16_t res = connect(m_socket_fd, p_sock_addr, sizeof(m_address));

			if (res < 0)
			{
				std::stringstream msg;
				msg << "(send socket) Failed to connect to server. (attempt " << i << " out of " << MAX_CONNECTION_RETRIES << ")";
				utils::log(msg.str(), utils::MSG_TYPE::WARN);

				std::this_thread::sleep_for(RETRY_AFTER);
				continue;
			}

			utils::log("(send socket) Connection accepted on port \033[4m" + std::to_string(m_address.sin_port) + "\033[0m");
			m_connection_status = true;
			has_connected = true;
		}

		return true;
	}

	auto is_connected() -> bool { return m_connection_status; }

	void kill()
	{
		if (m_socket_fd == INVALID_FD)
			return;

		(void)close(m_socket_fd);
		m_socket_fd = INVALID_FD;
		utils::log("(send socket) Send socket closed.");
	}

	auto is_dead() -> bool { return (m_socket_fd == INVALID_FD); }

	void tx(const char *msg) { send(m_socket_fd, msg, MSG_SIZE, 0); }

	send_socket_t(const send_socket_t &) = delete;
	send_socket_t &operator=(const send_socket_t &) = delete;

private:
	bool m_connection_status;

	sockaddr_in m_address = sockaddr_in();
	std::int16_t m_socket_fd = INVALID_FD;
	std::string m_id;
};

#endif // H_SOCKET_HELPER