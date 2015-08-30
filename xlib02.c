#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>

int main(int argc, char **argv)
{
	Display *dpy;
	int screen;
	Window win, childwin;
	XEvent event;

	/* connection to X server. */
	dpy = XOpenDisplay(NULL);

	if (dpy == NULL) {
		fprintf(stderr, "can not open dsiplay\n");
		exit(EXIT_FAILURE);
	}

	screen = DefaultScreen(dpy);

	/* parent window */
	win = XCreateSimpleWindow(dpy, RootWindow(dpy, screen),
		100, 100, 600, 400,
		1, BlackPixel(dpy, screen), WhitePixel(dpy, screen));

	XSelectInput(dpy, win, ExposureMask | KeyPressMask);
	/* Expose events are generated for the window when part or all of it becomes visible on the screen. */
	XMapWindow(dpy, win);

	/* child window */
	childwin = XCreateSimpleWindow(dpy, win, 
		50, 50, 100, 100,
		5, BlackPixel(dpy, screen), WhitePixel(dpy, screen));
	XSelectInput(dpy, childwin, ExposureMask | KeyPressMask);
	XMapWindow(dpy, childwin);

	while (1) {
		XNextEvent(dpy, &event);
		if (event.xany.window == childwin) {
			if (event.type == Expose) {
				//  GC : graphics context
				XDrawLine(dpy, childwin, DefaultGC(dpy, screen),
					10, 10, 50, 50);
			}
		}
	}


}
