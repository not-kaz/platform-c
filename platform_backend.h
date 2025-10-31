#ifndef PLATFORM_BACKEND_H
#define PLATFORM_BACKEND_H
#include "platform_window.h"

struct platform_backend {
	void (*init)(void);
	void (*shutdown)(void);
	void *(*create_window)(struct platform_window_desc *window_desc);
	void (*destroy_window)(void *window);
	void (*poll_window_events)(void *window);
	void *(*get_native_platform_handle)(void);
	void *(*get_native_window_handle)(void);
};
