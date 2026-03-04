#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"

#include "app.h"
#include "ui/rectangle.h"

void app_t::run()
{
	SDL_Init(SDL_INIT_VIDEO);

	std::shared_ptr<window_helper_t> window = std::make_shared<window_helper_t>("Dyver");
	SDL_Renderer *renderer = SDL_CreateRenderer(window->get_sdl_window(), NULL);

	rectangle_t rect = rectangle_t({0.0, 0.0}, {0.5, 0.5});

	while (m_running) // See app_t declaration
	{
		SDL_Event event;
		while (SDL_PollEvent(&event)) // Keep polling events for as long as they are coming
		{
			if (event.type == SDL_EVENT_QUIT)
			{
				kill();
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		// Render widgets
		rect.render(renderer, window->get_sdl_window());

		SDL_RenderPresent(renderer);
	}
}
