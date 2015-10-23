#ifndef __XWIN_H__
#define __XWIN_H__

#include <stdlib.h>
#include <X11/Xlib.h>
#include <string.h>

typedef struct _xwin XWIN;


XWIN *xwin_init(int , int height, int fullscreen);

void xwin_event_loop(XWIN *xwin);

#endif
