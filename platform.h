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
struct platform_surface;

struct platform_window_desc {
	int32_t x;
	int32_t y;
	int32_t width;
	int32_t height;
	char title[PLATFORM_WINDOW_TITLE_MAX_LEN];
	uint8_t properties_flag;
};

enum platform_surface_format {
	PLATFORM_SURFACE_FORMAT_UNSUPPORTED = 0,
	PLATFORM_SURFACE_FORMAT_ABGR8888,
	PLATFORM_SURFACE_FORMAT_ARGB8888,
	PLATFORM_SURFACE_FORMAT_BGRA8888,
	PLATFORM_SURFACE_FORMAT_BGRX8888,
	PLATFORM_SURFACE_FORMAT_RGBA8888,
	PLATFORM_SURFACE_FORMAT_XRGB8888
};

struct platform_surface_desc {
	void *pixels;
	int32_t width;
	int32_t height;
	int32_t pitch;
	enum platform_surface_format format;
};

bool platform_start(void);

void platform_shutdown(void);

struct platform_event platform_poll_event(void);

struct platform_window *platform_window_create(struct platform_window_desc *window_desc);

void platform_window_destroy(struct platform_window *window);

void *platform_window_get_native_handle(struct platform_window *window);

struct platform_window_desc platform_window_get_desc(struct platform_window *window);

bool platform_window_should_close(struct platform_window *window);

bool platform_window_is_active(struct platform_window *window);

void platform_window_present_surface(struct platform_window *window, struct platform_surface *surface);

struct platform_surface *platform_surface_create(struct platform_surface_desc *surface_desc);

struct platform_surface_desc platform_surface_get_desc(struct platform_surface *surface);

#endif /* PLATFORM_H */
