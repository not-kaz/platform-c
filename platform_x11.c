#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include "platform.h"
#define MAX_NUM_KEYCODES 256
#define MAX_NUM_EVENTS 256

struct platform_window {
	Window handle;
	GC gc;
	XImage *image;
};

struct keymap_entry {
	char xkb_key_name[5];
	enum platform_keycode keycode;
};

static struct {
	Display *display;
	/* NOTE: Only default screen supported for the moment. */
	Window root_window;
	Visual *visual;
	int depth;
	int screen;
} x11_state;

struct {
	struct platform_event buffer[MAX_NUM_EVENTS];
	uint8_t read_idx;
	uint8_t write_idx;
} event_queue;

static enum platform_keycode keycodes[MAX_NUM_KEYCODES];

static const struct keymap_entry keymap[] = {
	/* TODO: Add missing keycode translations. */
	/* Control and whitespace */
	{ "ESC", PLATFORM_KEYCODE_ESCAPE },
	{ "TAB", PLATFORM_KEYCODE_TAB },
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


static inline int32_t clamp_and_cast_int_to_int32(int x)
{
	if (x > INT32_MAX) {
		return INT32_MAX;
	}
	if (x < INT32_MIN) {
		return INT32_MIN;
	}
	return (int32_t)x;
}

static inline int32_t clamp_and_cast_unsigned_to_int32(unsigned int x)
{
	if ((uintmax_t)x > (uintmax_t)INT32_MAX) {
		return INT32_MAX;
	}
	return (int32_t)x;
}

static inline enum platform_mouse_button translate_mouse_button(int x_button)
{
	enum platform_mouse_button mb = PLATFORM_MOUSE_BUTTON_NONE;

	/* NOTE: Consider moving towards translation map. */
	/* TODO: Add more buttons (atleast 32 total). */
	switch (x_button) {
	case Button1:
		mb = PLATFORM_MOUSE_BUTTON_LEFT;
		break;
	case Button2:
		mb = PLATFORM_MOUSE_BUTTON_MIDDLE;
		break;
	case Button3:
		mb = PLATFORM_MOUSE_BUTTON_RIGHT;
		break;
	case Button4:
		mb = PLATFORM_MOUSE_BUTTON_WHEEL_UP;
		break;
	case Button5:
		mb = PLATFORM_MOUSE_BUTTON_WHEEL_DOWN;
		break;
	}
	return mb;
}

static void setup_keycodes(void)
{
	XkbDescPtr xkb;
	int min_kc;
	int max_kc;

	memset(keycodes, 0, sizeof(keycodes));
	xkb = XkbGetMap(x11_state.display, 0, XkbUseCoreKbd);
	if (!xkb) {
		return;
	}
	if (XkbGetNames(x11_state.display, XkbKeyNamesMask, xkb) != Success ||
			!xkb->names || !xkb->names->keys) {
		XkbFreeKeyboard(xkb, 0, True);
		return;
	}
	min_kc = xkb->min_key_code;
	max_kc = xkb->max_key_code;
	if (min_kc < 0) {
		min_kc = 0;
	}
	if (max_kc > MAX_NUM_KEYCODES - 1) {
		max_kc = MAX_NUM_KEYCODES - 1;
	}
	for (int i = min_kc; i <= max_kc; i++) {
		const char *name = xkb->names->keys[i].name;

		if (name[0] != '\0') {
			printf("Keycode %d: [%.4s]\n", i, name);
		}
		for (size_t j = 0; j < sizeof(keymap) / sizeof(keymap[0]); j++) {
			if (memcmp(keymap[j].xkb_key_name, name, 4) == 0) {
				keycodes[i] = keymap[j].keycode;
				break;
			}
		}
	}
	XkbFreeKeyboard(xkb, 0, True);
}

bool platform_start(void)
{
	XPixmapFormatValues *formats;
	int format_cnt = -1;
	int supported_bpp = -1;

	x11_state.display = XOpenDisplay(NULL);
	if (!x11_state.display) {
		fprintf(stderr, "platform_start() failed: "
				"XOpenDisplay() returned NULL.\n");
		return false;
	}
	/* TODO: Allow for multiple screens. */
	x11_state.screen = DefaultScreen(x11_state.display);
	x11_state.root_window = RootWindow(x11_state.display, x11_state.screen);
	x11_state.visual = DefaultVisual(x11_state.display, x11_state.screen);
	x11_state.depth = DefaultDepth(x11_state.display, x11_state.screen);
	formats = XListPixmapFormats(x11_state.display, &format_cnt);
	for (int i = 0; i < format_cnt; i++) {
		if (formats[i].depth == x11_state.depth) {
			supported_bpp = formats[i].bits_per_pixel;
			break;
		}
	}
	XFree(formats);
	if (supported_bpp != 32) {
		XCloseDisplay(x11_state.display);
		fprintf(stderr, "platform_start() failed: "
				"32-bit Pixmap format not supported.\n");
		return false;
	}
	/* TODO: Inline this function as it is only used here. */
	setup_keycodes();
	memset(&event_queue, 0, sizeof(event_queue));
	return true;
}

void platform_shutdown(void)
{
	XCloseDisplay(x11_state.display);
	/* TODO: See if there is any other cleanup required. */
	memset(&x11_state, 0, sizeof(x11_state));
	memset(&event_queue, 0, sizeof(event_queue));
}

struct platform_window *platform_window_create(struct platform_window_desc window_desc)
{
	struct platform_window *window;
	XSetWindowAttributes attribs;

	window = calloc(1, sizeof(struct platform_window));
	if (window == NULL) {
		return NULL;
	}
	/* NOTE: We place all created windows on the default screen. */
	attribs.border_pixel = BlackPixel(x11_state.display, x11_state.screen);
	attribs.background_pixel = BlackPixel(x11_state.display, x11_state.screen);
	attribs.backing_store = NotUseful;
	window->handle = XCreateWindow(x11_state.display, x11_state.root_window,
			window_desc.x, window_desc.y,
			window_desc.width, window_desc.height, 0,
			x11_state.depth, InputOutput, x11_state.visual,
			CWBackPixel | CWBorderPixel | CWBackingStore,
			&attribs);
	XStoreName(x11_state.display, window->handle, window_desc.title);
	XSelectInput(x11_state.display, window->handle, KeyPressMask
			| KeyReleaseMask | ButtonPressMask | ButtonReleaseMask
			| PointerMotionMask);
	window->gc = XCreateGC(x11_state.display, window->handle, 0, NULL);
	if (!window->gc) {
		XDestroyWindow(x11_state.display, window->handle);
		return NULL;
	}
	XMapWindow(x11_state.display, window->handle);
	XFlush(x11_state.display);
	return window;
}

void platform_window_destroy(struct platform_window *window)
{
	XDestroyWindow(x11_state.display, window->handle);
	XFreeGC(x11_state.display, window->gc);
	free(window);
}

void *platform_window_get_native_handle(struct platform_window *window)
{
	if (window->handle) {
		return (void *)window->handle;
	} else {
		return NULL;
	}
}

struct platform_window_desc platform_window_get_desc(struct platform_window *window)
{
	struct platform_window_desc wd = {0};
	char *title;
	int x, y;
	unsigned int w, h;
	Window dummy;
	int sink;

	XGetGeometry(x11_state.display, window->handle, &dummy, &sink, &sink,
			&w, &h, (unsigned int *)&sink, (unsigned int *)&sink);
	XTranslateCoordinates(x11_state.display, window->handle, x11_state.root_window,
			0, 0, &x, &y, &dummy);
	wd.x = clamp_and_cast_int_to_int32(x);
	wd.y = clamp_and_cast_int_to_int32(y);
	wd.width = clamp_and_cast_unsigned_to_int32(w);
	wd.height = clamp_and_cast_unsigned_to_int32(h);
	if (XFetchName(x11_state.display, window->handle, &title)) {
		size_t len = strlen(title);

		if (len > PLATFORM_WINDOW_TITLE_MAX_LEN - 1) {
			len = PLATFORM_WINDOW_TITLE_MAX_LEN - 1;
		}
		memcpy(wd.title, title, len);
		wd.title[len] = '\0';
	}
	return wd;
}

struct platform_event platform_poll_event(void)
{
	struct platform_event event = {
		.type = PLATFORM_EVENT_TYPE_NONE
	};

	if (event_queue.read_idx == event_queue.write_idx) {
		while (XPending(x11_state.display) != 0) {
			XEvent x_event;
			struct platform_event tmp_event = {
				.type = PLATFORM_EVENT_TYPE_NONE
			};

			XNextEvent(x11_state.display, &x_event);
			switch (x_event.type) {
			case KeyPress:
			case KeyRelease:
				if (x_event.type == KeyPress) {
					tmp_event.type = PLATFORM_EVENT_TYPE_KEY_PRESS;
				} else if (x_event.type == KeyRelease) {
					tmp_event.type = PLATFORM_EVENT_TYPE_KEY_RELEASE;
				}
				tmp_event.detail.key.keycode = keycodes[x_event.xkey.keycode & 0xFFu];
				break;
			case ButtonPress:
			case ButtonRelease:
				/* TODO: Add proper mouse scroll and motion support. */
				if (x_event.type == ButtonPress) {
					tmp_event.type = PLATFORM_EVENT_TYPE_MOUSE_BUTTON_PRESS;
				} else if (x_event.type == ButtonRelease) {
					tmp_event.type = PLATFORM_EVENT_TYPE_MOUSE_BUTTON_RELEASE;
				}
				tmp_event.detail.mouse_button = translate_mouse_button(x_event.xbutton.button);
				break;
			/* TODO: Implement missing events. */
			case MotionNotify:
			case ConfigureNotify:
				break;
			case EnterNotify:
			case LeaveNotify:
				break;
			case FocusIn:
			case FocusOut:
				break;
			case DestroyNotify:
				break;
			case ClientMessage:
				break;
			}
			if (tmp_event.type != PLATFORM_EVENT_TYPE_NONE) {
				event_queue.buffer[event_queue.write_idx] = tmp_event;
				if (event_queue.write_idx >= MAX_NUM_EVENTS - 1) {
					event_queue.write_idx = 0;
				} else {
					event_queue.write_idx++;
				}
			}
		}
	}
	if (event_queue.read_idx != event_queue.write_idx) {
		event = event_queue.buffer[event_queue.read_idx];
		if (event_queue.read_idx >= MAX_NUM_EVENTS - 1) {
			event_queue.read_idx = 0;
		} else {
			event_queue.read_idx++;
		}
	}
	return event;
}

void platform_window_present_bgra8_buffer(struct platform_window *window,
		struct platform_bgra8_buffer_desc buffer_desc)
{
	(void)window;
	(void)buffer_desc;
}
