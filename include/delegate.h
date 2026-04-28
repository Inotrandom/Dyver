#ifndef H_DELEGATE
#define H_DELEGATE

#include <algorithm>
#include <vector>

template <typename fn> class delegate_t
{
public:
	explicit delegate_t() {}
	~delegate_t() {}

	template <typename... args> void call(args... a)
	{
		for (const auto function : m_connected)
		{
			function(a...);
		}
	}

	template <typename... args> void operator()(args... a) { call<args...>(a...); }

	void connect(fn what) { m_connected.push_back(what); }

	void disconnect(fn what)
	{
		auto found = std::find(m_connected.begin(), m_connected.end(), what);
		if (found == m_connected.end())
		{
			return;
		}

		m_connected.erase(found);
	}

private:
	std::vector<fn> m_connected;
};

#endif // H_DELEGATE