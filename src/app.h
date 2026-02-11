/**
 * @file app.h
 * @brief The application interface for Dyver
 * @date 2025-11-01
 */

#ifndef H_APP
#define H_APP

#include "SDL3/SDL_video.h"
#include "SDL3/SDL_init.h"

#include "dss/runtime.h"

#include <string>

/*
Thanks https://www.studyplan.dev/sdl3/sdl3-application-loop for the resources.
*/

class window_helper_t
{
public:
	/**
	 * @brief Construct a new window helper t object
	 *
	 * @param title Title of the window
	 */
	window_helper_t(std::string title) { sdl_window = SDL_CreateWindow(title.c_str(), 700, 300, 0); }

	/**
	 * @brief Get the surface object
	 *
	 * @return SDL_Surface*
	 */
	SDL_Surface *get_surface() const { return SDL_GetWindowSurface(sdl_window); }

	// Prevent copying of the window
	window_helper_t(const window_helper_t &) = delete;
	window_helper_t &operator=(const window_helper_t &) = delete;

	~window_helper_t()
	{
		if (sdl_window && SDL_WasInit(SDL_INIT_VIDEO))
		{
			SDL_DestroyWindow(sdl_window);
		}
	}

	auto get_sdl_window() -> SDL_Window * { return sdl_window; }

private:
	SDL_Window *sdl_window = nullptr;
};

/**
 * @brief The heart of Dyver
 *
 */
class app_t
{
public:
	app_t(std::shared_ptr<DSS::executor_t> p_exec) { m_p_exec = p_exec; }

	void run();

	/**
	 * @brief Kills the application
	 *
	 */
	void kill() { m_running = false; }

private:
	/**
	 * @brief Pointer to a DSS executor, in case any DSS scripts need running
	 *
	 */
	std::shared_ptr<DSS::executor_t> m_p_exec{nullptr};

	/**
	 * @brief The application's status. This should not be directly set. Instead, use `app_t::kill()`
	 *
	 */
	bool m_running = true;
};

#endif