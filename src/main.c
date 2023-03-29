#include <ncurses/ncurses.h>


int main(int argc, char** argv)
{
	(void) argc;
	(void) argv;

	initscr();
	cbreak();
	noecho();

	getch();

	endwin();

	return 0;
}