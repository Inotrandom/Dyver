#include "SDL3/SDL_init.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"

#include "SDL3_ttf/SDL_ttf.h"

#include "app.h"
#include "ui/rectangle.h"
#include "ui/textbox.h"

/**
 * @brief Main application loop for dyver. Initializes SDL, produces a window, initializes widgets, and then handles SDL events and renders widgets in a while
 * loop.
 *
 */
void app_t::run()
{
	bool ttf_res = TTF_Init();
	bool sdl_res = SDL_Init(SDL_INIT_VIDEO);

	if (ttf_res == false)
	{
		m_p_exec->exec("out [error] failed to initialize SDL_ttf");
		return;
	}

	if (sdl_res == false)
	{
		m_p_exec->exec("out [error] failed to initialize SDL");
		return;
	}

	// TTF_Font *p_hevletica = TTF_OpenFont("/home/estelle/projects/Dyver/assets/font/Helvetica Neue/Helvetica Neue Regular/Helvetica Neue Regular.otf", 90.0);
	// SDL_Surface *p_test_text = TTF_RenderText_Blended(p_hevletica, "DYYYYVER", 0, {0, 0, 0, SDL_ALPHA_OPAQUE});

	std::shared_ptr<window_helper_t> window = std::make_shared<window_helper_t>("Dyver");
	SDL_Renderer *renderer = SDL_CreateRenderer(window->get_sdl_window(), NULL);

	// SDL_Texture *p_test_text_texture = SDL_CreateTextureFromSurface(renderer, p_test_text);

	// rectangle_t rect = rectangle_t({0.0, 0.0}, {0.5, 0.5});
	textbox_t textbox = textbox_t(rectangle_t({0.0, 0.0}, {0.4, 0.05}));
	textbox_t textboxb = textbox_t(rectangle_t({0.0, 0.051}, {0.4, 0.101}), {255, 0, 0, SDL_ALPHA_OPAQUE});

	textbox.set_text("Static text box");

	std::uint8_t counter = 0;

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
		/*
		int w = 0, h = 0;
		SDL_FRect dst;
		const float scale = 0.1;

		// Center the text and scale it up
		SDL_GetRenderOutputSize(renderer, &w, &h);
		SDL_GetTextureSize(p_test_text_texture, &dst.w, &dst.h);
		dst.x = ((w / scale) - dst.w) / 2;
		dst.y = ((h / scale) - dst.h) / 2;
		*/

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		// Render widgets
		// rect.render(renderer, window->get_sdl_window());

		++counter;

		textboxb.set_text("Test Counter: " + std::to_string(counter));

		textbox.render(renderer, window->get_sdl_window());
		textboxb.render(renderer, window->get_sdl_window());

		// SDL_SetRenderScale(renderer, scale, scale);

		// SDL_FRect dst2 = (rect.to_sdl_rect(window->get_sdl_window()).value());
		// SDL_RenderTexture(renderer, p_test_text_texture, NULL, &dst2);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
}