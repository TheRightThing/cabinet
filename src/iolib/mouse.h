#ifndef __iolib_mouse_h_defined
#define __iolib_mouse_h_defined

enum MouseAction {
	IOLIB_MOUSE_PRESSED,
	IOLIB_MOUSE_RELEASED,
	IOLIB_MOUSE_CLICKED,
	IOLIB_MOUSE_MOVED
};

int iolib_last_mouse_event(
	unsigned * x, unsigned *y,
	enum MouseAction * action, int * button);

#endif