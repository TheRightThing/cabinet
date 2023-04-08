#include "keyboard.h"
#include <ncurses/ncurses.h>

#define _countof(x) (sizeof(x) / sizeof(*(x)))

enum IolibKey iolib_more_keypresses() { return iolib_keypress(0); }


int iolib_interpret_mouse(int const * keys, unsigned len);
enum IolibKey iolib_keypress(int ms_timeout)
{
	static struct { char scan[4]; enum IolibKey key; } const k_key_table[] = {
		{{27, -1, -1, -1}, IOLIB_ESCAPE},
		{{27, 91, 65, -1}, IOLIB_UP},
		{{27, 91, 65, 27}, IOLIB_SCROLL_UP},
		{{27, 91, 66, -1}, IOLIB_DOWN},
		{{27, 91, 66, 27}, IOLIB_SCROLL_DOWN},
		{{27, 91, 67, -1}, IOLIB_RIGHT},
		{{27, 91, 68, -1}, IOLIB_LEFT},
		{{27, 91, 50, 126}, IOLIB_INSERT},
		{{27, 91, 51, 126}, IOLIB_DELETE},
		{{27, 91, 53, 126}, IOLIB_PAGE_UP},
		{{27, 91, 54, 126}, IOLIB_PAGE_DOWN},
		{{27, 91, 70, -1}, IOLIB_END},
		{{27, 91, 72, -1}, IOLIB_HOME},
		{{27, 79, 80, -1}, IOLIB_F1},
		{{27, 79, 81, -1}, IOLIB_F2},
		{{27, 79, 82, -1}, IOLIB_F3},
		{{27, 79, 83, -1}, IOLIB_F4},
		{{27, 91, 49, 53}, IOLIB_F5},
		{{27, 91, 49, 55}, IOLIB_F6},
		{{27, 91, 49, 56}, IOLIB_F7},
		{{27, 91, 49, 57}, IOLIB_F8},
		{{27, 91, 50, 48}, IOLIB_F9},
		{{27, 91, 50, 49}, IOLIB_F10},
		{{27, 91, 50, 52}, IOLIB_F12}
	};

	int keys[32];
	for(unsigned i = 0; i < _countof(keys); i++)
		keys[i] = -1;

	wtimeout(stdscr, ms_timeout);
	unsigned keys_len = 1;
	keys[0] = getch();
	if(keys[0] == 27)
	{
		wtimeout(stdscr, 0);
		for(unsigned i = 1; i < _countof(keys); i++)
		{
			keys[i] = getch();
			if(keys[i] == -1)
			{
				keys_len = i;
				break;
			}
		}
	}

	if(keys[0] == KEY_RESIZE)
		return IOLIB_RESIZED;
	if(keys[0] == KEY_MOUSE)
		return -0x8000;

	for(unsigned i = 0; i < _countof(k_key_table); i++)
	{
		for(unsigned j = 0; j < _countof(k_key_table[0].scan); j++)
			if(k_key_table[i].scan[j] != keys[j])
				goto search_next_key;
		// return key if it matched.
		return k_key_table[i].key;
	search_next_key:
	}

	if(iolib_interpret_mouse(keys, keys_len))
		return -1;

	if(keys[0] == 27 && keys[1] > 0 && keys[2] == -1)
		return keys[1] | IOLIB_MOD_META;

	if(keys[1] == -1 && keys[0] <= 255) // only had a single character?
		return *keys;

	attron(A_REVERSE);
	mvprintw(0, 0,
		"IOLIB: unknown scancode: ");
	for(unsigned i = 0; i < _countof(keys) && keys[i] != -1; i++)
		printw(i ? "/%d" : "%d", keys[i]);
	addstr(" (iolib/keyboard.c)");
	attroff(A_REVERSE);

	return -1;
}