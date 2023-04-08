#include "canvas.h"
#include "keyboard.h"
#include "mouse.h"

#include <ncurses/ncurses.h>
#include <stdlib.h>

static int registered_exit_handler = 0;
static int in_canvas_mode = 0;
static key_handler_fn_t key_handler = NULL;
static mouse_handler_fn_t mouse_handler = NULL;
static void * context = NULL;

int iolib_canvas_mode_set(
	key_handler_fn_t on_key,
	mouse_handler_fn_t on_mouse,
	void * _context)
{
	key_handler = on_key;
	mouse_handler = on_mouse;
	context = _context;

	if(in_canvas_mode)
		return 1;

	if(!initscr())
		return 0;


	if(!registered_exit_handler)
	{
		atexit(&iolib_canvas_mode_leave);
		registered_exit_handler = 1;
	}

	typeahead(-1);
	cbreak();
	meta(stdscr,1);
	keypad(stdscr,0); // keypad fucks up the ESC key.
	timeout(0);
	noecho();
	curs_set(0);
	leaveok(stdscr, 1);
	mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, 0);

	in_canvas_mode = 1;
	return 1;
}

void iolib_canvas_mode_leave()
{
	if(in_canvas_mode)
	{
		mousemask(0, 0);
		endwin();
		in_canvas_mode = 0;
	}
}

void iolib_canvas_draw()
{
	refresh();
}

unsigned iolib_mouse_poll_deadline(unsigned original_deadline);

iolib_event_result_t iolib_poll_event(int ms_timeout)
{
	unsigned x;
	unsigned y;
	enum MouseAction action;
	int button;

	int status = 0;
	/// If we have an outstanding mouse event, immediately process it and then return.
	while(mouse_handler && iolib_last_mouse_event(&x, &y, &action, &button))
		status |= mouse_handler(x, y, action, button, context);

	int key = iolib_keypress(iolib_mouse_poll_deadline(ms_timeout));

	if(mouse_handler && iolib_last_mouse_event(&x, &y, &action, &button))
		status |= mouse_handler(x, y, action, button, context);

	if(key != -1 && key_handler)
		status |= key_handler(key, context);

	return status;
}