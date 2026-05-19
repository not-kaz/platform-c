#ifndef PLATFORM_EVENT_H
#define PLATFORM_EVENT_H
#include "platform_input.h"

enum platform_event_type {
	PLATFORM_EVENT_TYPE_NONE,
	PLATFORM_EVENT_TYPE_KEY_PRESS,
	PLATFORM_EVENT_TYPE_KEY_RELEASE,
	PLATFORM_EVENT_TYPE_MOUSE_BUTTON_PRESS,
	PLATFORM_EVENT_TYPE_MOUSE_BUTTON_RELEASE
	/* TODO: Add missing event types." */
};

struct platform_event {
	enum platform_event_type type;
	union {
		enum platform_mouse_button mouse_button;
		struct platform_key key;
	} detail;
	/* TODO: Finish implementing. */
};

#endif
