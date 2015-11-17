#include "xwin.h"

struct _xwin {
	Display *dpy;
	Screen *screen;
	Window win;
	int width;
	int height;
	int screen_number;
	int fullscreen;
};

XWIN *xwin_init(int width, int height, int fullscreen)
{
	XWIN *xwin;
	XSetWindowAttributes attribs;

	xwin = (XWIN *) malloc(sizeof(XWIN));

	memset(xwin, 0, sizeof(XWIN));

	if ((xwin->dpy = XOpenDisplay(NULL)) == NULL)
		return NULL;

	xwin->screen = DefaultScreenOfDisplay(xwin->dpy);
	xwin->width = width;
	xwin->height = height;
	xwin->fullscreen = fullscreen;
	xwin->screen_number = DefaultScreen(xwin->dpy);


	attribs.background_pixel = WhitePixelOfScreen(xwin->screen);
	attribs.override_redirect = False;
	attribs.bit_gravity = NorthWestGravity;
	attribs.win_gravity = NorthWestGravity;

#if 1

	xwin->win = XCreateWindow(xwin->dpy, RootWindow(xwin->dpy, xwin->screen_number),
			0, 0, 1, 1, 0,
			DefaultDepthOfScreen(xwin->screen), InputOutput,
			DefaultVisualOfScreen(xwin->screen),
			CWBackPixel | CWBackingStore | CWOverrideRedirect | CWColormap |
			CWBorderPixel | CWWinGravity | CWBitGravity, &attribs);
#else

	xwin->win = XCreateSimpleWindow(xwin->dpy, RootWindow(xwin->dpy, xwin->screen_number),
			0, 0, xwin->width, xwin->height,
			1, BlackPixel(xwin->dpy, xwin->screen_number), WhitePixel(xwin->dpy, xwin->screen_number));
#endif

	return xwin;
}

static void xwin_fullscreen(XWIN *xwin)
{
	int ret;
	XEvent xevent;
	Atom _NET_WM_STATE_FULLSCREEN = XInternAtom(xwin->dpy, "_NET_WM_STATE_FULLSCREEN", False);

	/* it works on openbox, KDE and ubuntu unity */
	XChangeProperty(xwin->dpy, xwin->win, XInternAtom(xwin->dpy, "_NET_WM_STATE", True),
				XA_ATOM, 32, PropModeReplace, (unsigned char *) &_NET_WM_STATE_FULLSCREEN, 1);
	XSync(xwin->dpy, False);
}

void xwin_event_loop(XWIN *xwin)
{
	XEvent event;

	if (xwin->fullscreen) {
		xwin_fullscreen(xwin);
	}

	XMapWindow(xwin->dpy, xwin->win);
	while (1) {
		XNextEvent(xwin->dpy, &event);
	}
}
