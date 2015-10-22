#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>

int main(int argc, char **argv)
{
	Display *dpy;
	int screen;
	Window win, childwin;
	XEvent event;

	int i, x, y, shadow_width;
	unsigned int width, height;
	unsigned int border_width, depth;
	Window root_win;
	Colormap colormap;
	XColor button_color, lightgray_color, darkgray_color;
	XGCValues gcv_lightgray, gcv_darkgray, gcv_button;
	GC gc_lightgray, gc_darkgray, gc_button;

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

	/* return color map ID. All drawable type are XIDs. */
	colormap = DefaultColormap(dpy, screen);
	XParseColor(dpy, colormap, "rgb:cc/cc/cc", &button_color);
	XAllocColor(dpy, colormap, &button_color);
	
	
	XParseColor(dpy, colormap, "rgb:ee/ee/ee", &lightgray_color);
	XAllocColor(dpy, colormap, &lightgray_color);
	/* GC Light gray */
	gcv_lightgray.foreground = lightgray_color.pixel;
	gcv_lightgray.background = button_color.pixel;
	gc_lightgray = XCreateGC(dpy, RootWindow(dpy, screen), 
			GCForeground |  GCBackground, &gcv_lightgray);

	XParseColor(dpy, colormap, "rgb:88/88/88", &darkgray_color);
	XAllocColor(dpy, colormap, &darkgray_color);
	/* GC Dark gray */
	gcv_darkgray.foreground = darkgray_color.pixel;
	gcv_darkgray.background = button_color.pixel;
	gc_darkgray = XCreateGC(dpy, RootWindow(dpy, screen), 
			GCForeground |  GCBackground, &gcv_darkgray);

	/* GC button */
	gcv_button.foreground = button_color.pixel;
	gcv_button.background = button_color.pixel;
	gc_button = XCreateGC(dpy, RootWindow(dpy, screen), 
			GCForeground |  GCBackground, &gcv_button);

	childwin = XCreateSimpleWindow(dpy, win, 
			50, 50, 100, 100,
			1, BlackPixel(dpy, screen), button_color.pixel);

	XSelectInput(dpy, childwin, ExposureMask | KeyPressMask |
			ButtonPressMask | ButtonReleaseMask);
	XMapWindow(dpy, childwin);

	XGetGeometry(dpy, childwin, &root_win, &x, &y, &width, &height,
			&border_width, &depth);

	shadow_width = 3;
	while (1) {
		XNextEvent(dpy, &event);
		if (event.xany.window == childwin) {
			if (event.type == Expose) {
				for (i = 0; i < shadow_width; i++) {
					XDrawLine(dpy, childwin, gc_lightgray,
							0, i, width-1, i);
					XDrawLine(dpy, childwin, gc_lightgray,
							i, 0, i, height-1);
					
					XDrawLine(dpy, childwin, gc_darkgray,
							width-1-i, 0, width-1-i, height-1);
					XDrawLine(dpy, childwin, gc_darkgray,
							0, height-1-i, width-1, height-1-i);
				}
			}
			if (event.type == ButtonPress) {
				for (i = 0; i < shadow_width; i++) {
					XDrawLine(dpy, childwin, gc_button,
							0, i, width-1, i);
					XDrawLine(dpy, childwin, gc_button,
							i, 0, i, height-1);
					XDrawLine(dpy, childwin, gc_button,
							width-1-i, 0, width-1-i, height-1);
					XDrawLine(dpy, childwin, gc_button,
							0, height-1-i, width-1, height-1-i);
				}
			}
			if (event.type == ButtonRelease) {
				for (i = 0; i < shadow_width; i++) {
					XDrawLine(dpy, childwin, gc_lightgray,
							0, i, width-1, i);
					XDrawLine(dpy, childwin, gc_lightgray,
							i, 0, i, height-1);
					
					XDrawLine(dpy, childwin, gc_darkgray,
							width-1-i, 0, width-1-i, height-1);
					XDrawLine(dpy, childwin, gc_darkgray,
							0, height-1-i, width-1, height-1-i);
				}
			}
		}
	}

	return 0;
}
