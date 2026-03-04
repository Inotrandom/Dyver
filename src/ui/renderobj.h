#ifndef H_RENDEROBJ
#define H_RENDEROBJ

#include <SDL3/SDL_render.h>

class renderobj_t
{
public:
	explicit renderobj_t() {}
	~renderobj_t() {}

	virtual void render(SDL_Renderer *renderer, SDL_Window *window) = 0;

private:
};

#endif // H_RENDEROBJ