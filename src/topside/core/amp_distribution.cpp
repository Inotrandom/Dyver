#include "amp_distribution.h"

/**
 * @brief Creates a new request on the heap, stores it in `m_active_requests`, and then recomputes.
 *
 * @param request Passed to the constructor of dynamic_amp_request_t
 * @param priority Passed to the constructor of dynamic_amp_request_t
 * @return std::shared_ptr<dynamic_amp_request_t> Pointer to the new request after recomputation
 */
auto amp_distributor_t::invoke_request(const double request, const AMP_REQUEST_PRIORITY priority) -> std::shared_ptr<dynamic_amp_request_t>
{
	std::shared_ptr<dynamic_amp_request_t> new_request = std::make_shared<dynamic_amp_request_t>(request, priority);
	m_active_requests.push_back(new_request);

	compute();

	return new_request;
}

/**
 * @brief Iterates over requests and tallies the requests that match `which`
 *
 * @param which `AMP_REQUEST_PRIORITY` to test every active request against
 * @return std::uint32_t Clone of a local variable that is incremented upon a match between `which` and an element in `m_active_requests`
 */
auto amp_distributor_t::tally_by_priority(const AMP_REQUEST_PRIORITY which) -> std::uint32_t
{
	std::uint32_t res = 0;

	for (auto request : m_active_requests)
	{
		if (request->get_priority() != which)
		{
			continue;
		}

		res++;
	}

	return res;
}

/**
 * @brief Iterates over active requests, passes "ALWAYS_FULFILL" requests, and attempts to find a request lower in value than the lowest previously encountered.
 * The "lowest previously encountered" request value is initialized to INFINITY.
 *
 * @return std::shared_ptr<dynamic_amp_request_t> A local variable initialized to nullptr is populated with an address upon every time the "lowest previously
 * encountered" value is higher than the currently tested value.
 */
auto amp_distributor_t::min_variable_request() -> std::shared_ptr<dynamic_amp_request_t>
{
	double found_min = INFINITY;
	std::shared_ptr<dynamic_amp_request_t> p_found = nullptr;

	for (auto request : m_active_requests)
	{
		if (request->get_priority() != AMP_REQUEST_PRIORITY::DISTRIBUTE)
			continue;
		if (request->get_request() > found_min)
			continue;

		found_min = request->get_request();
		p_found = request;
	}

	return p_found;
}

/**
 * @brief Segments a cake base on the `m_max_allowance` and intelligently distributes amperage among requests as efficiently as possible.
 *
 */
void amp_distributor_t::compute()
{
	// Splitting the cake between requests
	double cake = m_max_allowance_A;

	for (auto request : m_active_requests)
	{
		request->mark_computed(false);
		request->reset_allowance();
	}

	for (auto request : m_active_requests)
	{
		if (request->get_priority() != AMP_REQUEST_PRIORITY::ALWAYS_FULFILL)
			continue;

		cake -= request->get_request(); // "Always Fulfilled Requests" must be fulfilled
		request->set_allowance(request->get_request());
		request->mark_computed(); // Nothing more to do to this request.
	}

	std::shared_ptr<dynamic_amp_request_t> p_min_request = min_variable_request();

	if (p_min_request == nullptr)
		return; // Also implies that there are no dynamic requests

	std::uint32_t ndynamic_requests = tally_by_priority(AMP_REQUEST_PRIORITY::DISTRIBUTE);
	std::uint32_t unfulfilled_requests = ndynamic_requests;
	double distributed;

	// Brief: Start by evenly distributing amperage, collect leftovers from smaller
	// requests, then distribute that among the other requests.
	for (std::uint32_t i = 0; i < ndynamic_requests; i++)
	{
		for (auto request : m_active_requests)
		{
			distributed = cake / unfulfilled_requests; // Split the cake

			if (request->get_priority() != AMP_REQUEST_PRIORITY::DISTRIBUTE)
				continue;
			if (request->is_computed() == true)
				continue; // Already satisfied, can skip

			request->set_allowance(distributed); // Helps determine if it's fulfilled.

			double diff = distributed - request->get_request(); // Leftovers

			if (diff > 0) // Nothing left-over if it's negative
			{
				request->set_allowance(request->get_request()); // Had some to spare, so crop
			}

			if (request->is_fulfilled() == true)
			{
				request->mark_computed();  // Fulfilled! Not taking any more amperage away from this.
				unfulfilled_requests -= 1; // One request has been fulfilled
				cake -= request->get_request();
				continue;
			}
		}
	}

	for (auto request : m_active_requests)
	{
		if (request->is_computed() == true)
			continue;			  // Already computed
		request->mark_computed(); // Processed to its full ability.
	}
}