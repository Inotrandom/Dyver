#ifndef H_FOCUSBOX
#define H_FOCUSBOX

#include "topside/ui/input/mouse.h"
#include "topside/ui/widget.h"

class focusbox_t
{
public:
	explicit focusbox_t() {}
	~focusbox_t() {}

private:
	bool m_just_hovered = false;
	bool m_just_clicked = false;
	bool m_being_hovered = false;
	bool m_being_clicked = false;

	ui_pos_t m_aa = ui_pos_t(0.0, 0.0);
	ui_pos_t m_bb = ui_pos_t(0.0, 0.0);
};

#endif // H_FOCUSBOX