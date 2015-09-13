#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

int main(int argc, char **argv)
{
	Display *dpy;
	int screen;
	Window win;
	XEvent event;
	char hello[] = "hello";
	XSelectionRequestEvent *xreq;
	XSelectionEvent ev;

	// connection to X server.
	dpy = XOpenDisplay(NULL);

	if (dpy == NULL) {
		fprintf(stderr, "can not open dsiplay\n");
		exit(EXIT_FAILURE);
	}

	screen = DefaultScreen(dpy);
	win = XCreateSimpleWindow(dpy, RootWindow(dpy, screen),
		0, 0, 100, 100,
		1, BlackPixel(dpy, screen), WhitePixel(dpy, screen));

	XSelectInput(dpy, win, PropertyChangeMask);
	XMapWindow(dpy, win);
	XChangeProperty (dpy, win, XA_WM_NAME, XA_STRING, 8,
                   PropModeAppend, NULL, 0);

	while (1) {
		XNextEvent (dpy, &event);
		if (event.type == PropertyNotify)
			break;
	}
 
	while (1) {
		XSetSelectionOwner(dpy, XA_PRIMARY, win, CurrentTime);
		XFlush(dpy);
		XNextEvent(dpy, &event);
		if (event.type == SelectionRequest) {
			xreq = &event.xselectionrequest;
			if (xreq->target == XA_STRING) {
				XChangeProperty(dpy, xreq->requestor, xreq->property , XA_STRING, 8,
						PropModeReplace, (unsigned char *)hello, strlen(hello));
				ev.property = xreq->property;
			} else {
				ev.property = None;
			}
			ev.type = SelectionNotify;
			ev.display = xreq->display;
			ev.requestor = xreq->requestor;
			ev.selection = xreq->selection;
			ev.time = xreq->time;
			ev.target = xreq->target;
			XSendEvent(dpy, ev.requestor, False,(unsigned long)NULL, (XEvent *)&ev);
			XSync(dpy, False);
		}
	}

}
