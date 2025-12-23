#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h> /* TODO: Check if this is necessary. */
#include "platform.h"

struct key_pair {
	char xkb_key_name[5];
	enum platform_keycode keycode;
};

static struct {
	Display *display;
	/* NOTE: Only default screen supported for the moment. */
	int32_t screen_id;
	Window root_window;
	XEvent event;
} state;

static const struct key_pair key_table[] = {
	/* TODO: Add missing keycode translations. */
	/* Control and whitespace */
	{ "ESC ", PLATFORM_KEYCODE_ESCAPE },
	{ "TAB ", PLATFORM_KEYCODE_TAB },
	{ "RTRN", PLATFORM_KEYCODE_ENTER },
	{ "BKSP", PLATFORM_KEYCODE_BACKSPACE },
	{ "SPCE", PLATFORM_KEYCODE_SPACE },
	{ "CAPS", PLATFORM_KEYCODE_CAPS_LOCK },
	/* Digits */
	{ "AE01", PLATFORM_KEYCODE_1 },
	{ "AE02", PLATFORM_KEYCODE_2 },
	{ "AE03", PLATFORM_KEYCODE_3 },
	{ "AE04", PLATFORM_KEYCODE_4 },
	{ "AE05", PLATFORM_KEYCODE_5 },
	{ "AE06", PLATFORM_KEYCODE_6 },
	{ "AE07", PLATFORM_KEYCODE_7 },
	{ "AE08", PLATFORM_KEYCODE_8 },
	{ "AE09", PLATFORM_KEYCODE_9 },
	{ "AE10", PLATFORM_KEYCODE_0 },
	/* Letters */
	{ "AD01", PLATFORM_KEYCODE_Q },
	{ "AD02", PLATFORM_KEYCODE_W },
	{ "AD03", PLATFORM_KEYCODE_E },
	{ "AD04", PLATFORM_KEYCODE_R },
	{ "AD05", PLATFORM_KEYCODE_T },
	{ "AD06", PLATFORM_KEYCODE_Y },
	{ "AD07", PLATFORM_KEYCODE_U },
	{ "AD08", PLATFORM_KEYCODE_I },
	{ "AD09", PLATFORM_KEYCODE_O },
	{ "AD10", PLATFORM_KEYCODE_P },
	{ "AC01", PLATFORM_KEYCODE_A },
	{ "AC02", PLATFORM_KEYCODE_S },
	{ "AC03", PLATFORM_KEYCODE_D },
	{ "AC04", PLATFORM_KEYCODE_F },
	{ "AC05", PLATFORM_KEYCODE_G },
	{ "AC06", PLATFORM_KEYCODE_H },
	{ "AC07", PLATFORM_KEYCODE_J },
	{ "AC08", PLATFORM_KEYCODE_K },
	{ "AC09", PLATFORM_KEYCODE_L },
	{ "AB01", PLATFORM_KEYCODE_Z },
	{ "AB02", PLATFORM_KEYCODE_X },
	{ "AB03", PLATFORM_KEYCODE_C },
	{ "AB04", PLATFORM_KEYCODE_V },
	{ "AB05", PLATFORM_KEYCODE_B },
	{ "AB06", PLATFORM_KEYCODE_N },
	{ "AB07", PLATFORM_KEYCODE_M },
	/* Symbols */
	{ "TLDE", PLATFORM_KEYCODE_GRAVE },
	{ "AE11", PLATFORM_KEYCODE_MINUS },
	{ "AE12", PLATFORM_KEYCODE_EQUAL },
	{ "AD11", PLATFORM_KEYCODE_LEFT_BRACKET },
	{ "AD12", PLATFORM_KEYCODE_RIGHT_BRACKET },
	{ "BKSL", PLATFORM_KEYCODE_BACKSLASH },
	{ "AC10", PLATFORM_KEYCODE_SEMICOLON },
	{ "AC11", PLATFORM_KEYCODE_APOSTROPHE },
	{ "AB08", PLATFORM_KEYCODE_COMMA },
	{ "AB09", PLATFORM_KEYCODE_PERIOD },
	{ "AB10", PLATFORM_KEYCODE_SLASH },
	{ "NUBS", PLATFORM_KEYCODE_NON_US_BACKSLASH_PIPE },
	{ "NUHS", PLATFORM_KEYCODE_NON_US_HASH_TILDE },
	/* Modifiers */
	{ "LCTL", PLATFORM_KEYCODE_LEFT_CONTROL },
	{ "LFSH", PLATFORM_KEYCODE_LEFT_SHIFT },
	{ "LALT", PLATFORM_KEYCODE_LEFT_ALT },
	{ "LWIN", PLATFORM_KEYCODE_LEFT_GUI },
	{ "RCTL", PLATFORM_KEYCODE_RIGHT_CONTROL },
	{ "RTSH", PLATFORM_KEYCODE_RIGHT_SHIFT },
	{ "RALT", PLATFORM_KEYCODE_RIGHT_ALT },
	{ "RWIN", PLATFORM_KEYCODE_RIGHT_GUI }
};

static void translate_key(const char xkb_keyname)
{
	for (size_t i = 0; i < sizeof(key_table) / sizeof(key_table[0]); i++) {
	}
}

bool platform_start(void)
{
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
	XStoreName(state.display, window->handle, window_desc.title);
	XMapWindow(state.display, window->handle);
	XFlush(state.display);
}

void platform_window_finish(struct platform_window *window)
{
	XDestroyWindow(state.display, window->handle);
}

struct platform_window_desc platform_window_get_desc(
		struct platform_window *window)
{
	struct platform_window_desc wd = {0};
	char *title;
	Window dummy;
	unsigned int sink;

	XGetGeometry(state.display, window->handle, &dummy, &sink, &sink,
			&wd.width, &wd.height, &sink, &sink);
	XTranslateCoordinates(state.display, window->handle, state.root_window,
			0, 0, &wd.x, &wd.y, &dummy);
	if (XFetchName(state.display, window->handle, &title)) {
		size_t len = strlen(title);

		if (len > PLATFORM_WINDOW_TITLE_MAX_LEN - 1) {
			len = PLATFORM_WINDOW_TITLE_MAX_LEN - 1;
		}
		memcpy(wd.title, title, len);
		wd.title[len] = '\0';
	}
	return wd;
}

struct platform_event platform_poll_events(void)
{
	struct platform_event event;

	XNextEvent(state.display, state.event);
	switch (state.event.type) {
	case KeyPress:
		break;
	case KeyRelease:
		break;
	case ButtonPress:
		break;
	case MotionNotify:
		break;
	}
}

void platform_present_rgba8_buffer(struct platform_window *window,
		struct platform_present_info present_info)
{
}
