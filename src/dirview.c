#include <ncurses/ncurses.h>
#include <string.h>
#include <malloc.h>

#include "dirview.h"

#include "iolib/keyboard.h"
#include "iolib/canvas.h"

static void print_entry(
	unsigned y,
	unsigned w,
	struct CabinetDirectoryEntry const * entry);
static void cabinet_dir_view(struct CabinetDirView * ctx);

static int handle_mouse(unsigned x, unsigned y, enum MouseAction action, int button, struct CabinetDirView * ctx)
{
	(void) x;
	(void) y;
	(void) action;
	if(action != IOLIB_MOUSE_PRESSED)
		return 0;

	int modified = 0;
	switch(button)
	{
	case IOLIB_SCROLL_UP:
		if(ctx->selected)
		{
			modified = 1;
			--ctx->selected;
		}
		break;
	case IOLIB_SCROLL_DOWN:
		modified |= ctx->selected != ctx->count-1;
		if(ctx->selected+1 < ctx->count)
			++ctx->selected;
		break;
	case 1:
		unsigned w, h;
		getmaxyx(stdscr, h, w);
		(void) w;
		int halfh = h>>1;
		int halfcount = ctx->count >> 1;

		unsigned i;
		if(ctx->count < h)
			i = y-1 - (halfh - halfcount);
		else i = y-1 + ctx->start;

		if(i < ctx->count)
		{
			unsigned len = strlen(ctx->entries[i].name);
			unsigned start = len >= w ? 0 : (w - len) >> 1;
			if((x-1)+1 >= start && (x-1) <= start+(len-1)+1)
			{
				if(ctx->selected != i)
				{
					modified = 1;
					ctx->selected = i;
				}
				else if(ctx->on_selected)
					ctx->on_selected(&ctx->entries[ctx->selected]);
			}
		}
	}

	if(modified)
		cabinet_dir_view(ctx);

	return modified;
}

static int handle_input(int key, struct CabinetDirView * ctx)
{
	int modified = 0;

	do {
		switch(key & IOLIB_NOMODS)
		{
		case 'q':
			return -1;

		case IOLIB_LEFT:
		case 'h':
		case IOLIB_ESCAPE:
			return  -1;
		case IOLIB_SCROLL_UP:
		case IOLIB_UP:
		case 'j':
			if(ctx->selected)
			{
				modified = 1;
				--ctx->selected;
			}
			break;
		case IOLIB_PAGE_UP:
			modified |= ctx->selected != 0;
			if(ctx->selected > 5)
				ctx->selected -= 5;
			else ctx->selected = 0;
			break;
		case IOLIB_PAGE_DOWN:
			modified |= ctx->selected != ctx->count-1;
			if(ctx->selected+5 < ctx->count)
				ctx->selected += 5;
			else
				ctx->selected = ctx->count-1;
			break;
		case IOLIB_HOME:
			modified |= ctx->selected != 0;
			ctx->selected = 0;
			break;
		case IOLIB_END:
			modified |= ctx->selected != ctx->count-1;
			ctx->selected = ctx->count-1;
			break;
		case IOLIB_SCROLL_DOWN:
		case IOLIB_DOWN:
		case 'k':
			modified |= ctx->selected != ctx->count-1;
			if(ctx->selected+1 < ctx->count)
				++ctx->selected;
			break;
		case IOLIB_RIGHT:
		case '\n':
		case 'l':
			if(ctx->on_selected)
				ctx->on_selected(&ctx->entries[ctx->selected]);
			break;
		case IOLIB_RESIZED:
			modified = 1;
			break;
		default:
			break;
		}
	} while((key = iolib_more_keypresses()) != -1);
	if(modified)
		cabinet_dir_view(ctx);

	return modified;
}

static void cabinet_dir_view(struct CabinetDirView * ctx)
{
	unsigned w, h;
	getmaxyx(stdscr, h, w);

	if(ctx->start >= ctx->count)
		ctx->start = ctx->count-1;

	int halfh = h>>1;
	int halfcount = ctx->count >> 1;

	if(ctx->selected < ctx->start)
		ctx->start = ctx->selected;
	if(ctx->selected >= ctx->start + h)
		ctx->start = ctx->selected - h + 1;

	erase();
	if(ctx->count < h)
		for(unsigned i = 0; i < ctx->count; i++)
		{
			if(i == ctx->selected) attron(A_REVERSE); else attroff(A_REVERSE);
			unsigned y = halfh - halfcount + i;
			print_entry(y, w, &ctx->entries[i]);
		}
	else
		for(unsigned i = ctx->start; i < ctx->start+h && i < ctx->count; i++)
		{
			if(i == ctx->selected) attron(A_REVERSE); else attroff(A_REVERSE);
			print_entry(i-ctx->start, w, &ctx->entries[i]);
		}
}

void cabinet_dir_view_activate(struct CabinetDirView * ctx)
{
	iolib_canvas_mode_set(
		(key_handler_fn_t)&handle_input,
		(mouse_handler_fn_t)&handle_mouse,
		ctx);
	cabinet_dir_view(ctx);
}

static void print_entry(
	unsigned y,
	unsigned w,
	struct CabinetDirectoryEntry const * entry)
{
	if(entry->is_dir)
		attron(A_BOLD);
	else
		attroff(A_BOLD);

	unsigned len = strlen(entry->name);
	if(len > w)
	{
		char * crop = (char *) malloc(w+1);
		memcpy(crop, entry->name, w);
		for(unsigned j = w - 3; j < w; j++)
			crop[j] = '.';
		crop[w] = '\0';
		mvaddstr(y, 0, crop);
		free(crop);
	} else {
		mvaddstr(y, (w - len) >> 1, entry->name);
	}
}