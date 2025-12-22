#ifndef PLATFORM_H
#define PLATFORM_H
#include <stdbool.h>
#include <stdint.h>

struct platform_window_desc {
	int32_t x;
	int32_t y;
	int32_t width;
	int32_t height;
	const char *title;
};

struct platform_window {
	uintptr_t handle;
};

struct platform_present_buffer_info {
	const void *buffer;
	int32_t width;
	int32_t height;
	int32_t pitch;
};

bool platform_start(void);
void platform_shutdown(void);
void platform_window_init(struct platform_window *window,
		struct platform_window_desc window_desc);
void platform_present_rgba8_buffer(struct platform_window *window,
		struct platform_present_buffer_info buffer_info);

#endif /* PLATFORM_H */
