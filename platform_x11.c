#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h> /* TODO: Check if this is necessary. */
#include "platform.h"

struct {
	Display *display;
	/* NOTE: Only default screen supported for the moment. */
	int32_t screen_id;
	Window root_window;
} state = {0};

bool platform_start(void)
{
	if (state.display) {
		return false;
	}
	state.display = XOpenDisplay(NULL);
	if (!state.display) {
		fprintf(stderr, "platform_start() failed:"
				"XOpenDisplay() returned NULL.");
		return false;
	}
	/* TODO: Allow for multiple screens. */
	state.screen_id = DefaultScreen(state.display);
	state.root_window = RootWindow(state.display, state.screen_id);
	return true;
}

void platform_shutdown(void)
{
	XCloseDisplay(state.display);
	/* TODO: Clear 'state' struct. */
}

void platform_window_init(struct platform_window *window,
		struct platform_window_desc window_desc)
{
	/* TODO: Figure out how to tackle border and background colors. */
	window->handle = (uintptr_t)XCreateSimpleWindow(state.display,
			state.root_window, window_desc.x, window_desc.y,
			window_desc.width, window_desc.height, 1,
			BlackPixel(state.display, state.screen_id), 
			WhitePixel(state.display, state.screen_id));
	XMapWindow(state.display, window->handle);
	XFlush(state.display);
}

void platform_window_finish(struct platform_window *window)
{
	XDestroyWindow(state.display, window->handle);
}
{
}
