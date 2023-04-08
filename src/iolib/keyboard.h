#ifndef __iolib_keyboard_h_defined
#define __iolib_keyboard_h_defined

enum IolibKey iolib_keypress(int ms_timeout);

/// Use this to query more keypresses after receiving an event.
enum IolibKey iolib_more_keypresses();

enum IolibKey
{
	IOLIB_NONE = -1,


	IOLIB_ESCAPE = 256,
	IOLIB_F1,
	IOLIB_F2,
	IOLIB_F3,
	IOLIB_F4,
	IOLIB_F5,
	IOLIB_F6,
	IOLIB_F7,
	IOLIB_F8,
	IOLIB_F9,
	IOLIB_F10,
	IOLIB_F11,
	IOLIB_F12,

	IOLIB_PAGE_UP,
	IOLIB_PAGE_DOWN,
	IOLIB_HOME,
	IOLIB_END,
	IOLIB_INSERT,
	IOLIB_DELETE,

	IOLIB_UP,
	IOLIB_DOWN,
	IOLIB_RIGHT,
	IOLIB_LEFT,

	IOLIB_SCROLL_UP,
	IOLIB_SCROLL_DOWN,

	IOLIB_RESIZED,

	IOLIB_MOD_META = 1024, // SHIFT, ALT, or CTRL. They seem to produce the same code.
	IOLIB_MOD_META2 = 2048, // Only ALT+CTRL, it seems.
	IOLIB_MODS = IOLIB_MOD_META | IOLIB_MOD_META2,
	IOLIB_NOMODS = ~IOLIB_MODS
};

#endif