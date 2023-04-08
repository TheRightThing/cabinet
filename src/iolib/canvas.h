#ifndef __iolib_canvas_h_defined
#define __iolib_canvas_h_defined

#include "mouse.h"

/// User-defined event handler status codes to be returned by poll_events.
typedef int iolib_event_result_t;
typedef iolib_event_result_t (*key_handler_fn_t)(int, void*);

/// the button argument should be ignored for mouse move events.
typedef iolib_event_result_t (*mouse_handler_fn_t)(
	unsigned x, unsigned y, enum MouseAction action, int button, void*);

int iolib_canvas_mode_set(
	key_handler_fn_t on_key,
	mouse_handler_fn_t on_mouse,
	void * context);
void iolib_canvas_mode_leave();
void iolib_canvas_mode_get_resolution(int * x, int * y);
void iolib_canvas_draw();

/// poll event and forward it to the currently active context. Returns the status of the user event handler, if any.
iolib_event_result_t iolib_poll_event(int ms_timeout);

#endif