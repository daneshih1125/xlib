#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>

int main(int argc, char **argv)
{
	Display *dpy;
	int screen;
	Window win;
	XEvent event;

	// connection to X server.
	dpy = XOpenDisplay(NULL);

	if (dpy == NULL) {
		fprintf(stderr, "can not open dsiplay\n");
		exit(EXIT_FAILURE);
	}

	screen = DefaultScreen(dpy);

	win = XCreateSimpleWindow(dpy, RootWindow(dpy, screen),
		100, 100, 600, 400,
		1, BlackPixel(dpy, screen), WhitePixel(dpy, screen));

	XSelectInput(dpy, win, ExposureMask | KeyPressMask);
	// Expose events are generated for the window when part or all of it becomes visible on the screen.
	XMapWindow(dpy, win);

	while (1) {
		XNextEvent(dpy, &event);
	}


}
