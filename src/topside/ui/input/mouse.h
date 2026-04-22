#ifndef H_MOUSE
#define H_MOUSE

#include <SDL3/SDL_mouse.h>

class mouse_t
{
public:
	explicit mouse_t()
	{
		x = new float;
		y = new float;

		*x = 0.0;
		*y = 0.0;
	}

	~mouse_t()
	{
		delete x;
		delete y;
	}

	auto get_x() -> float { return *x; }
	auto get_y() -> float { return *y; }

	void update() { SDL_GetMouseState(x, y); }

private:
	float *x = nullptr;
	float *y = nullptr;
};

#endif // H_MOUSE