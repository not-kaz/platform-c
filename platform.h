#ifndef PLATFORM_H
#define PLATFORM_H
#include <stdbool.h>
#include <stdint.h>
#include "platform_event.h"
#include "platform_input.h"

#define PLATFORM_WINDOW_TITLE_MAX_LEN 64

/* Foward declarations, implemented by backend. */
struct platform_window;

#define PLATFORM_WINDOW_PROPERTY_HIDDEN UINT64_C(0x0000000000000001)
#define PLATFORM_WINDOW_PROPERTY_MINIMIZED UINT64_C(0x0000000000000002)
#define PLATFORM_WINDOW_PROPERTY_MAXIMIZED UINT64_C(0x0000000000000004)
#define PLATFORM_WINDOW_PROPERTY_FULLSCREEN UINT64_C(0x0000000000000008)
#define PLATFORM_WINDOW_PROPERTY_WINDOWED UINT64_C(0x0000000000000010)
#define PLATFORM_WINDOW_PROPERTY_BORDERLESS UINT64_C(0x0000000000000020)
#define PLATFORM_WINDOW_PROPERTY_OPENGL UINT64_C(0x0000000000000080)
#define PLATFORM_WINDOW_PROPERTY_VULKAN UINT64_C(0x0000000000000100)

struct platform_window_desc {
	int32_t x;
	int32_t y;
	int32_t width;
	int32_t height;
	char title[PLATFORM_WINDOW_TITLE_MAX_LEN];
	uint8_t properties_flag;
};

struct platform_bgra8_buffer_desc {
	struct {
		uint8_t b, g, r, a;
	} *pixels;
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

void *platform_window_get_native_handle(struct platform_window *window);

struct platform_window_desc platform_window_get_desc(struct platform_window *window);

bool platform_window_should_close(struct platform_window *window);

bool platform_window_is_active(struct platform_window *window);

void platform_window_present_bgra8_buffer(struct platform_window *window, 
		struct platform_bgra8_buffer_desc buffer_desc);

#endif /* PLATFORM_H */
