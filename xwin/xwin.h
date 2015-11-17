#ifndef __XWIN_H__
#define __XWIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <string.h>

typedef struct _xwin XWIN;


XWIN *xwin_init(int , int height, int fullscreen);

void xwin_event_loop(XWIN *xwin);

#endif
