#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h> /* TODO: Check if this is necessary. */
#include "platform.h"

#define MAX_NUM_SCREENS 16
#define MAX_NUM_WINDOWS 32

struct {
	Display *display;
	/* NOTE: Only default screen supported for the moment. */
	int32_t screen_id;
	Window root_window;
	Window windows[MAX_NUM_WINDOWS];
	uint8_t window_count;
} state = {0};

bool platform_start(void)
{
	if (state.display) {
		return false;
	}
	state.display = XOpenDisplay(NULL);
	if (!state.display) {
		fprintf(stderr, "platform_x11_start() failed:"
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
	for (uint8_t i = 0; i < state.window_count; i++) {
		XDestroyWindow(state.display, state.windows[i]);
	}
	XCloseDisplay(state.display);
}

struct platform_window platform_create_window(
		struct platform_window_desc window_desc)
{
	struct platform_window window;

	/* TODO: Figure out how to tackle border and background colors. */
	state.windows[state.window_count] = XCreateSimpleWindow(state.display,
			state.root_window, window_desc.x, window_desc.y,
			window_desc.width, window_desc.height, 1,
			BlackPixel(state.display, state.screen_id), 
			WhitePixel(state.display, state.screen_id));
	window.handle = (uintptr_t)state.windows[state.window_count];
	XMapWindow(state.display, state.windows[state.window_count]);
	XFlush(state.display);
	state.window_count++;
	return window;
}

void platform_present_rgba8_buffer(void)
{
}
