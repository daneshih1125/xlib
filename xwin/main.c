#include <stdio.h>
#include <unistd.h>
#include "xwin.h"

#define DEFAULT_WIDTH	800
#define DEFAULT_HEIGHT	600

int main(int argc, char **argv)
{
	int opt; 
	int width = DEFAULT_WIDTH, height = DEFAULT_HEIGHT; 
	int fullscreen = 0;
	XWIN *xwin;

	while ((opt = getopt(argc, argv, "w:h:f")) != -1) {
		switch (opt) {
		case 'w':
			width = atoi(optarg);
			break;
		case 'h':
			height = atoi(optarg);
			break;
		case 'f':
			fullscreen = 1;
		}
	}

	xwin = xwin_init(width, height, fullscreen);
	xwin_event_loop(xwin);

	return 0;
}
