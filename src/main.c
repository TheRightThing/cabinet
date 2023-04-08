#include "iolib/canvas.h"
#include "iolib/keyboard.h"
#include "dirview.h"

#include <ncurses/ncurses.h>

#define _countof(x) (sizeof(x) / sizeof(*(x)))


void on_selected(struct CabinetDirectoryEntry const * entry) {
	attron(A_REVERSE);
	mvaddstr(0,0, "selected entry '");
	addstr(entry->name);
	addstr("'");
	attroff(A_REVERSE);
}

int main(int argc, char** argv)
{
	(void) argc;
	(void) argv;

	struct CabinetDirectoryEntry entries[] = {
		{1, "."},
		{1, ".."},
		{1, "data"},
		{0, "top secret source code.java"},
		{0, "test.txt"},
		{0, "cabinet"},
		{0, "cabinet1"},
		{0, "cabinet2"},
		{0, "super gigantic cabinet"},
		{0, "cabinet4"},
		{0, "cabinet5"},
		{0, "cabinet6"},
		{1, "cabinet7"},
		{1, "cabinet8"},
		{1, "cabinet9"},
		{0, "cabinet10"},
		{0, "huge cabinet.txt"},
		{0, "huge cabinet2.txt"},
	};

	struct CabinetDirView dirView = {
		.entries = entries,
		.count = _countof(entries),

		.start = 0,
		.selected = 0,

		.on_selected = &on_selected
	};

	iolib_canvas_mode_set(0,0,0);

	cabinet_dir_view_activate(&dirView);
	iolib_canvas_draw();

	for(;;)
		switch(iolib_poll_event(-1))
		{
		case 0: continue;
		case -1: return 0;
		default:
			iolib_canvas_draw();
		}
	return 0;
}