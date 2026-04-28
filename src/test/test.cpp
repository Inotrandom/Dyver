#include <chrono>
#include <cmath>
#include <string>
#include <thread>

#include "topside/core/amp_distribution.h"

// #include "networking/networking_key.h"
// #include "networking/socket_helper.h"

#include "networking/iosock.h"
#include "networking/networking_key.h"

// #include "networking/dyver_client.h"
// #include "networking/dyver_server.h"

#include "DSS.h"

#include "topside/core/rov.h"

#include "utils.h"

#include "test.h"

static const test_t TEST_TEST = test_t("test_test", __LINE__,
	[]()
	{
		if (1 + 1 == 2)
		{
			return true;
		}
		else
		{
			return false;
		}
	});

static const test_t TEST_AMP_DISTRIBUTOR = test_t("test_amp_distributor", __LINE__,
	[]()
	{
		amp_distributor_t distributor = amp_distributor_t(10.0);
		std::shared_ptr<dynamic_amp_request_t> req_a = distributor.invoke_request(5.0, AMP_REQUEST_PRIORITY::ALWAYS_FULFILL);
		std::shared_ptr<dynamic_amp_request_t> req_b = distributor.invoke_request(3.0, AMP_REQUEST_PRIORITY::DISTRIBUTE);
		std::shared_ptr<dynamic_amp_request_t> req_c = distributor.invoke_request(2.0, AMP_REQUEST_PRIORITY::DISTRIBUTE);

		distributor.compute();

		/*
		// Debug purposes
		std::cout << req_a->get_allowance() << std::endl;
		std::cout << req_b->get_allowance() << std::endl;
		std::cout << req_c->get_allowance() << std::endl;
		*/

		if (req_a->get_allowance() != 5.0)
			return false;
		if (req_b->get_allowance() != 3.0)
			return false;
		if (req_c->get_allowance() != 2.0)
			return false;

		std::shared_ptr<dynamic_amp_request_t> req_d = distributor.invoke_request(6.0, AMP_REQUEST_PRIORITY::DISTRIBUTE);

		/*
		std::cout << req_d->get_allowance() << std::endl;
		std::cout << req_a->get_allowance() << std::endl;
		std::cout << req_b->get_allowance() << std::endl;
		std::cout << req_c->get_allowance() << std::endl;
		*/

		if (req_a->get_allowance() != 5.0)
			return false;
		if (approx_eq(req_b->get_allowance(), 1.6667, 0.1) == false)
			return false;
		if (approx_eq(req_c->get_allowance(), 1.6667, 0.1) == false)
			return false;
		if (approx_eq(req_d->get_allowance(), 1.6667, 0.1) == false)
			return false;

		return true;
	});

static const test_t TEST_PWM_THROTTLE = test_t("test_pwm_throttle", __LINE__,
	[]()
	{
		utils::linear_percentage_t throttle = utils::linear_percentage_t(1000.0, 1100.0);
		double p = throttle.to_percentage(1050.0);

		if (p != 0.5)
			return false;

		p = throttle.to_percentage(1075.0);

		if (p != 0.75)
			return false;

		double v = throttle.to_value(p);

		if (v != 1075.0)
			return false;

		double sgn_p = throttle.sgn_to_percentage(1025.0);

		if (sgn_p != -0.5)
			return false;

		sgn_p = throttle.sgn_to_percentage(1075.0);

		if (sgn_p != 0.5)
			return false;

		double sgn_v = throttle.sgn_to_value(-0.5);

		if (sgn_v != 1025.0)
			return false;

		sgn_v = throttle.sgn_to_value(0.5);

		if (sgn_v != 1075.0)
			return false;

		return true;
	});

static const test_t TEST_ABSTRACT_ROV = test_t("test_abstract_rov", __LINE__,
	[]()
	{
		rov_t rov = rov_t();
		const double FORCE = 28.63542; // Roughly approximation of a BlueRobotics T200 Thruster maximmum output

		// A terribly impractical ROV... but mathematically simple and easy to test
		rov.create_thruster({4.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, FORCE, "EastUp");
		rov.create_thruster({-4.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, FORCE, "WestUp");
		rov.create_thruster({0.0, 0.0, 1.0}, {0.0, 0.0, 1.0}, FORCE, "ForwardForward");
		rov.create_thruster({0.0, 0.0, -1.0}, {0.0, 0.0, 1.0}, FORCE, "BackForward");

		rov.optimize_throttle_config({0.0, 0.0, 0.0}, {0.0, 0.0, 1.0});

		if (rov.get_thrusters()["EastUp"]->get_target_congruence() != 1.0)
			return false;

		if (rov.get_thrusters()["WestUp"]->get_target_congruence() != -1.0)
			return false;

		if (rov.get_thrusters()["ForwardForward"]->get_target_congruence() != 0.0 || rov.get_thrusters()["BackForward"]->get_target_congruence() != 0.0)
			return false;

		rov.optimize_throttle_config({0.0, 1.0, 0.0}, {0.0, 0.0, 0.0});

		if (rov.get_thrusters()["EastUp"]->get_target_congruence() != 1.0)
			return false;

		if (rov.get_thrusters()["WestUp"]->get_target_congruence() != 1.0)
			return false;

		if (rov.get_thrusters()["ForwardForward"]->get_target_congruence() != 0.0 || rov.get_thrusters()["BackForward"]->get_target_congruence() != 0.0)
			return false;

		rov.optimize_throttle_config({0.0, 0.0, 1.0}, {0.0, 0.0, 0.0});

		if (rov.get_thrusters()["EastUp"]->get_target_congruence() != 0.0 || rov.get_thrusters()["WestUp"]->get_target_congruence() != 0.0)
			return false;

		if (rov.get_thrusters()["ForwardForward"]->get_target_congruence() != 1.0)
			return false;

		if (rov.get_thrusters()["BackForward"]->get_target_congruence() != 1.0)
			return false;

		rov.create_thruster({1.0, 0.0, 0.0}, {0.0, 0.0, 1.0}, FORCE, "HorribleDesignChoice");
		if (rov.calculate_unbalanced_torque().y() != -1.0)
		{
			return false;
		}

		return true;
	});

#if 0
static bool TEST_SOCKET_HELPER_SUCCEEDED = true;
static std::string TEST_SOCKET_HELPER_CURRENT = "";

static void run_sockethelper_test_server()
{
	listen_socket_t server = listen_socket_t();

	server.get_onrx()->connect(
		[](const std::string s)
		{
			std::cout << "Recieved: " << s << std::endl;
			if (TEST_SOCKET_HELPER_CURRENT != s)
			{
				TEST_SOCKET_HELPER_SUCCEEDED = false;
			}
		});

	server.initialize(PORT_DSS);

	server.accept_n(1);

	std::this_thread::sleep_for(std::chrono::seconds(2));
	server.kill();
}

static void run_sockethelper_test_client()
{
	send_socket_t client = send_socket_t();
	client.connect_to(PORT_DSS, "127.0.0.1");
	TEST_SOCKET_HELPER_CURRENT = "Check it out";
	client.tx(TEST_SOCKET_HELPER_CURRENT.c_str());
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	TEST_SOCKET_HELPER_CURRENT = "I'm in the house";
	client.tx(TEST_SOCKET_HELPER_CURRENT.c_str());
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	TEST_SOCKET_HELPER_CURRENT = "Like carpet";
	client.tx(TEST_SOCKET_HELPER_CURRENT.c_str());

	std::this_thread::sleep_for(std::chrono::seconds(1));
	client.kill();
}

static const test_t TEST_SOCKET_HELPER = test_t("test_socket_helper", __LINE__,
	[]()
	{
		std::thread server_handle(run_sockethelper_test_server);
		std::this_thread::yield();
		std::thread client_handle(run_sockethelper_test_client);

		server_handle.join();
		client_handle.join();
		return TEST_SOCKET_HELPER_SUCCEEDED;
	});
#endif

static const test_t TEST_DELEGATE = test_t("test_delegate", __LINE__,
	[]()
	{
		bool res = false;
		std::shared_ptr<delegate_t<std::function<void()>>> del = std::make_shared<delegate_t<std::function<void()>>>();
		del->connect([&res]() { res = true; });
		del->call();

		return res;
	});

static const test_t TEST_IOSOCK = test_t("test_iosock", __LINE__,
	[]()
	{
		iosock_t peer_a;
		iosock_t peer_b;

		peer_a.init(PORT_DSS, PORT_DSS, false, true);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		peer_b.init(PORT_DSS, PORT_DSS, true, false);
		std::this_thread::sleep_for(std::chrono::seconds(2));

		std::string tosend = "Meow!";
		std::string recieved = "";

		peer_b.get_onrx()->connect(
			[&recieved](std::string msg)
			{
				recieved = msg;
				std::cout << "Peer a Recieved: " << msg << std::endl;
			});

		peer_a << tosend;

		std::this_thread::sleep_for(std::chrono::seconds(1));

		peer_a.kill();
		peer_b.kill();

		if (recieved == tosend)
		{
			return true;
		}

		return false;
	});

#if 0
static void run_dyver_server_test()
{
	dyver_server_t server = dyver_server_t();
	server.initialize("127.0.0.1");
	std::this_thread::sleep_for(std::chrono::seconds(6));
	server.verify_inet_connection();
	std::this_thread::sleep_for(std::chrono::seconds(9));
	server.kill();
}

static void run_dyver_client_test()
{
	dyver_client_t client = dyver_client_t();
	client.initialize("127.0.0.1");
	std::this_thread::sleep_for(std::chrono::seconds(15));
	client.kill();
}

static const test_t TEST_CLIENT_SERVER = test_t("test_client_server", __LINE__,
	[]()
	{
		utils::log("(test) Initializing Dyver Server");
		std::thread server_handle(run_dyver_server_test);

		server_handle.detach();

		std::this_thread::sleep_for(std::chrono::seconds(5));

		utils::log("(test) Initializing Dyver Client");
		std::thread client_handle(run_dyver_client_test);

		client_handle.join();

		return true;
	});
#endif
/*
static const test_t TEST_DENSE_UTILS = test_t("", __LINE__, [](){
	quat_from_euler(Eigen::Vector3d());
});
*/
auto main() -> int
{
	std::cout << "Commencing Dyver Tests" << std::endl;

	console_clear();

	int passed_tests = 0;
	int failed_tests = 0;

	TEST_TEST.run(&passed_tests, &failed_tests);
	TEST_AMP_DISTRIBUTOR.run(&passed_tests, &failed_tests);
	TEST_PWM_THROTTLE.run(&passed_tests, &failed_tests);
	TEST_ABSTRACT_ROV.run(&passed_tests, &failed_tests);
	TEST_DELEGATE.run(&passed_tests, &failed_tests);
	TEST_IOSOCK.run(&passed_tests, &failed_tests);
	// TEST_SOCKET_HELPER.run(&passed_tests, &failed_tests);
	// TEST_CLIENT_SERVER.run(&passed_tests, &failed_tests);

	std::cout << passed_tests << " tests passed" << std::endl;
	std::cout << failed_tests << " tests failed" << std::endl;

	return 0;
}