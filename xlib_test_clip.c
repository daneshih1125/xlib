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
	Atom targets_atom, clipboard_atom, text_atom;
	Atom atom_list[2];

	// connect to X server.
	dpy = XOpenDisplay(NULL);

	if (dpy == NULL) {
		fprintf(stderr, "can not open dsiplay\n");
		exit(EXIT_FAILURE);
	}

	screen = DefaultScreen(dpy);
	win = XCreateSimpleWindow(dpy, RootWindow(dpy, screen),
		0, 0, 10, 10,
		1, BlackPixel(dpy, screen), WhitePixel(dpy, screen));

	XSelectInput(dpy, win, PropertyChangeMask);
	XMapWindow(dpy, win);

	clipboard_atom = XInternAtom(dpy, "CLIPBOARD", False);
	targets_atom = XInternAtom(dpy, "TARGETS", False);
  	text_atom = XInternAtom(dpy, "TEXT", False);
	atom_list[0] = targets_atom;
	atom_list[1] = text_atom;

	XSetSelectionOwner(dpy, clipboard_atom, win, CurrentTime);
	XSetSelectionOwner(dpy, XA_PRIMARY, win, CurrentTime);
 
	while (1) {
		XFlush(dpy);
		XNextEvent(dpy, &event);
		if (event.type == SelectionRequest) {
			xreq = &event.xselectionrequest;
			if (xreq->target == XA_STRING) {
				XChangeProperty(dpy, xreq->requestor, xreq->property , XA_STRING, 8,
						PropModeReplace, (unsigned char *)hello, strlen(hello));
				ev.property = xreq->property;
			} else if (xreq->target == targets_atom) {
				XChangeProperty(dpy, xreq->requestor, xreq->property , XA_ATOM, 32,
						PropModeReplace, (unsigned char *)atom_list, 2);
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
