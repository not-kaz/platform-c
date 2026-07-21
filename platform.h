#ifndef PLATFORM_H
#define PLATFORM_H
#include <stdbool.h>
#include <stdint.h>
#include "platform_event.h"
#include "platform_input.h"

#define PLATFORM_WINDOW_TITLE_MAX_LEN 64
/* WINDOW PROPERTY FLAGS */
#define PLATFORM_WINDOW_PROPERTY_HIDDEN UINT64_C(0x0000000000000001)
#define PLATFORM_WINDOW_PROPERTY_MINIMIZED UINT64_C(0x0000000000000002)
#define PLATFORM_WINDOW_PROPERTY_MAXIMIZED UINT64_C(0x0000000000000004)
#define PLATFORM_WINDOW_PROPERTY_FULLSCREEN UINT64_C(0x0000000000000008)
#define PLATFORM_WINDOW_PROPERTY_WINDOWED UINT64_C(0x0000000000000010)
#define PLATFORM_WINDOW_PROPERTY_BORDERLESS UINT64_C(0x0000000000000020)
#define PLATFORM_WINDOW_PROPERTY_OPENGL UINT64_C(0x0000000000000080)
#define PLATFORM_WINDOW_PROPERTY_VULKAN UINT64_C(0x0000000000000100)

/* Foward declarations, implemented by backend. */
struct platform_window;

enum platform_pixel_format {
	PLATFORM_PIXEL_FORMAT_UNSUPPORTED = 0,
	PLATFORM_PIXEL_FORMAT_XRGB8888,
	PLATFORM_PIXEL_FORMAT_ARGB8888,
	PLATFORM_PIXEL_FORMAT_NUM_OF
};

struct platform_window_desc {
	int32_t x;
	int32_t y;
	int32_t width;
	int32_t height;
	char title[PLATFORM_WINDOW_TITLE_MAX_LEN];
	uint64_t properties_flag;
};

struct platform_desc {
	enum platform_pixel_format pixel_format;
};

struct platform_pixel_buffer_desc {
	void *pixels;
	int32_t width;
	int32_t height;
	int32_t stride;
};

bool platform_start(const struct platform_desc *platform_desc);

void platform_shutdown(void);

struct platform_event platform_poll_event(void);

struct platform_window *platform_window_create(const struct platform_window_desc *window_desc);

void platform_window_destroy(struct platform_window *window);

void *platform_window_get_native_handle(const struct platform_window *window);

struct platform_window_desc platform_window_get_desc(const struct platform_window *window);

bool platform_window_is_active(const struct platform_window *window);

void platform_window_present_buffer(const struct platform_window *window, 
		const struct platform_pixel_buffer_desc *pixel_buffer_desc);

void platform_query_supported_pixel_formats(enum platform_pixel_format *formats, uint8_t *count);

#endif /* PLATFORM_H */
