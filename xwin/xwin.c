//#include <stdio.h>
#include "xwin.h"

struct _xwin {
	Display *dpy;
	int screen;
	Window win;
	int width;
	int height; 
	int fullscreen;
};

struct hints
{
	unsigned long flags;
	unsigned long functions;
	unsigned long decorations;
	long inputMode;
	unsigned long status;
};

XWIN *xwin_init(int width, int height, int fullscreen)
{
	XWIN *xwin;

	xwin = (XWIN *) malloc(sizeof(XWIN));

	memset(xwin, 0, sizeof(XWIN));

	if ((xwin->dpy = XOpenDisplay(NULL)) == NULL)
		return NULL;

	xwin->screen = DefaultScreen(xwin->dpy);
	xwin->width = width;
	xwin->height = height;
	xwin->fullscreen = fullscreen;
	
	return xwin;
}

/* 
 * http://stackoverflow.com/questions/5134297/xlib-how-does-this-removing-window-decoration-work
 * Hardly find the _MOTIF_WM_HINTS definition.
 * refrence freerdp source code.
 */
static void xwin_fullscreen(XWIN *xwin)
{
	struct hints hints;
	Atom _MOTIF_WM_HINTS;

	hints.decorations = 0;
	hints.functions = 1;
	hints.flags = 1L << 1;
	hints.inputMode = 0;
	hints.status = 0;

	_MOTIF_WM_HINTS = XInternAtom(xwin->dpy, "_MOTIF_WM_HINTS", False);

	XChangeProperty(xwin->dpy, xwin->win, _MOTIF_WM_HINTS, _MOTIF_WM_HINTS, 32,
		PropModeReplace, (unsigned char*) &hints, 5);

	xwin->width = DisplayWidth(xwin->dpy, xwin->screen);
	xwin->height = DisplayWidth(xwin->dpy, xwin->screen);

	XMoveResizeWindow(xwin->dpy, xwin->win, 0, 0, xwin->width, xwin->height);
}

void xwin_event_loop(XWIN *xwin)
{
	XEvent event;

	xwin->win = XCreateSimpleWindow(xwin->dpy, RootWindow(xwin->dpy, xwin->screen),
		0, 0, xwin->width, xwin->height,
		1, BlackPixel(xwin->dpy, xwin->screen), WhitePixel(xwin->dpy, xwin->screen));

	if (xwin->fullscreen)
		xwin_fullscreen(xwin);

	XMapRaised(xwin->dpy, xwin->win);

	while (1) {
		XNextEvent(xwin->dpy, &event);
	}
}
