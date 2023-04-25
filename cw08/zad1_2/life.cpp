#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "threaded_update.h"

int main(int argc, char** argv)
{
	std::vector<std::string> args(argv, argv + argc);

	int num_threads = grid_size;
	if (args.size() == 2)
		num_threads = std::stoi(args[1]);
	else if (args.size() > 2)
		throw std::runtime_error("Too many arguments!");

	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode

	char *foreground = create_grid();
	char *background = create_grid();

	init_grid(foreground);

	ThreadedUpdater updater(num_threads, foreground, background);

	while (true)
	{
		draw_grid(foreground);

		// Step simulation
		updater.nextFrame();
		usleep(500 * 1000);
	}

	endwin(); // End curses mode
	destroy_grid(foreground);
	destroy_grid(background);

	return 0;
}
