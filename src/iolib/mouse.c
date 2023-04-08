#include "mouse.h"
#include "keyboard.h"
#define _POSIX_C_SOURCE 202304
#include <time.h>
#include <string.h>

#define _countof(x) (sizeof(x) / sizeof(*(x)))


static clockid_t clockid = CLOCK_MONOTONIC;

static unsigned ms_time()
{
	struct timespec res;
	clock_gettime(clockid, &res);
	return res.tv_sec * 1000 + (unsigned)(res.tv_nsec / 1000000ul);
}

/// By default, this is the minimum duration of mouse button actions before they are passed on to the application. This prevents faulty mouse buttons from messing with you with unsteady signals.
static unsigned safety_latency_ms = 20;

/// We maintain a queue of two mouse events, to handle spurious mouse events (contemporary mice, even from previously reputable brands, now use trash sensors in their buttons and start spuriously interrupting keypresses). Any inhumanely fast release-press sequences are ignored. The variable safety_latency_ms controls the safety window within which a press-release or release-press sequence is ignored; set to 0 to disable this feature).
static struct {
	int set;
	unsigned x;
	unsigned y;
	enum MouseAction action;
	unsigned button;
	unsigned ms;
} mouse_event[2] = { {.set = 0}, {.set = 0} };

static unsigned umin(unsigned a, unsigned b)
{
	return a < b ? a : b;
}

unsigned iolib_mouse_poll_deadline(unsigned original_deadline)
{
	unsigned time = ms_time();
	if(!safety_latency_ms)
		return original_deadline;

	else if(mouse_event[1].set && time - mouse_event[1].ms < safety_latency_ms)
		return umin(original_deadline, safety_latency_ms + mouse_event[1].ms - time);
	else if(mouse_event[0].set && time - mouse_event[0].ms < safety_latency_ms)
		return umin(original_deadline, safety_latency_ms + mouse_event[0].ms - time);
	else
		return original_deadline;
}

void iolib_mouse_init(unsigned ms_safety_latency)
{
	safety_latency_ms = ms_safety_latency;
	if(-1 != clock_gettime(CLOCK_TAI, &(struct timespec){}))
		clockid = CLOCK_TAI;
}

int iolib_last_mouse_event(
	unsigned * x, unsigned *y,
	enum MouseAction * action, int * button)
{
	int selected = -1;
	unsigned time = ms_time();
	if(mouse_event[1].set && time - mouse_event[1].ms >= safety_latency_ms)
		selected = 1;
	else if(mouse_event[0].set && time - mouse_event[0].ms >= safety_latency_ms)
		selected = 0;

	if(selected == -1)
		return 0;

	if(x) *x = mouse_event[selected].x;
	if(y) *y = mouse_event[selected].y;
	if(action) *action = mouse_event[selected].action;
	if(button) *button = mouse_event[selected].button;

	mouse_event[selected].set = 0;

	return 1;
}

static unsigned parse_coord(int const * scan)
{
	unsigned coord = 0;
	while(*scan >= '0' && *scan <= '9')
	{
		coord *= 10;
		coord += *scan++ - '0';
	}

	return coord;
}

int iolib_interpret_mouse(int const * scan, unsigned len)
{
	static char const k_mouse_prefix[] = {27, 91, 60};
	for(unsigned i = 0; i < _countof(k_mouse_prefix); i++)
		if(scan[i] != k_mouse_prefix[i])
			return 0;

	if(mouse_event[0].set)
		mouse_event[1] = mouse_event[0];
	mouse_event->set = 0;

	static struct { char scan[2]; int button; int mods; } const k_btn_table[] = {
		{ {48, 59}, 1, 0 },
		{ {49, 59}, 2, 0 },
		{ {50, 59}, 3, 0 },
		{ {54, 52}, IOLIB_SCROLL_UP, 0 },
		{ {54, 53}, IOLIB_SCROLL_DOWN, 0 }
	};

	int have_button = 0;
	for(unsigned i = 0; i < _countof(k_btn_table); i++)
	{
		for(unsigned j = 0; j < _countof(k_btn_table[0].scan); j++)
			if(scan[_countof(k_mouse_prefix) + j] != k_btn_table[i].scan[j])
				goto next_entry;
		mouse_event->button = k_btn_table[i].button;
		have_button = 1;
		break;
	next_entry:
	}

	unsigned i = 0;
	while(i < len)
		if(scan[i++] == 59)
		{
			mouse_event->x = parse_coord(scan+(i));
			break;
		}
	while(i < len)
		if(scan[i++] == 59)
		{
			mouse_event->y = parse_coord(scan+(i));
			break;
		}

	static struct { char scan; enum MouseAction action; } const k_action_table[] = {
		{77, IOLIB_MOUSE_PRESSED},
		{109, IOLIB_MOUSE_RELEASED}
	};

	int have_action = 0;
	for(unsigned i = 0; i < _countof(k_action_table); i++)
		if(scan[len-1] == k_action_table[i].scan)
		{
			mouse_event->action = k_action_table[i].action;
			have_action = 1;
			break;
		}

	if(have_action && have_button)
	{
		unsigned time = ms_time();
		if(mouse_event->button == IOLIB_SCROLL_UP || mouse_event->button == IOLIB_SCROLL_DOWN)
		{
			mouse_event->set = 1;
			mouse_event->ms = time - safety_latency_ms; /// instantly dispatch scrolling.
			return 1;
		}

		/// if we have two events that were not fetched, that means we were faster than the safety latency, discard both events.
		if(mouse_event[1].set
		&& time - mouse_event[1].ms < safety_latency_ms)
			mouse_event->set = mouse_event[1].set = 0;
		else
		{
			mouse_event->ms = time;
			mouse_event->set = 1;
		}
	}

	return have_action && have_button;
}