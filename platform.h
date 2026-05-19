#ifndef PLATFORM_H
#define PLATFORM_H
#include <stdbool.h>
#include <stdint.h>
#include "platform_event.h"
#include "platform_input.h"

#define PLATFORM_WINDOW_TITLE_MAX_LEN 64

/* Foward declarations, implemented by backend. */
struct platform_window;

struct platform_window_desc {
	int32_t x;
	int32_t y;
	int32_t width;
	int32_t height;
	char title[PLATFORM_WINDOW_TITLE_MAX_LEN];
};

struct platform_rgba8_buffer_desc {
	const void *buffer;
	int32_t destination_x;
	int32_t destination_y;
	int32_t width;
	int32_t height;
};

bool platform_start(void);

void platform_shutdown(void);

struct platform_event platform_poll_event(void);

struct platform_window *platform_window_create(struct platform_window_desc window_desc);

void platform_window_destroy(struct platform_window *window);

struct platform_window_desc platform_window_get_desc(struct platform_window *window);

bool platform_window_should_close(struct platform_window *window);

bool platform_window_is_active(struct platform_window *window);

void platform_window_present_rgba8_buffer(struct platform_window *window,
		struct platform_rgba8_buffer_desc buffer_desc);

#endif /* PLATFORM_H */
