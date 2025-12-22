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

#define PLATFORM_FUNC_CONCAT(prefix, name) platform_##prefix##_##name
#define PLATFORM_FUNC(name) PLATFORM_FUNC_CONCAT(PLATFORM_PREFIX, name)
#include "platform_x11.h"
#define PLATFORM_PREFIX x11
#endif

#define platform_start PLATFORM_FUNC(start)
#define platform_shutdown PLATFORM_FUNC(shutdown) 
#define platform_window_init PLATFORM_FUNC(window_init)
#define platform_window_finish PLATFORM_FUNC(window_finish)
#define platform_present_rgba8_buffer PLATFORM_FUNC(present_rgba8_buffer)

#endif /* PLATFORM_H */
