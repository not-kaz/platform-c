#ifndef PLATFORM_WINDOW_H
#define PLATFORM_WINDOW_H
#include <stdint.h>
#include <inttypes.h>

#define	PLATFROM_WINDOW_HIDDEN     UINT32_C(1) << 0
#define	PLATFORM_WINDOW_RESIZABLE  UINT32_C(1) << 1
#define PLATFORM_WINDOW_FULLSCREEN UINT32_C(1) << 2
#define PLATFORM_WINDOW_BORDERLESS UINT32_C(1) << 3
#define PLATFORM_WINDOW_MINIMIZED  UINT32_C(1) << 4
#define PLATFORM_WINDOW_MAXIMIZED  UINT32_C(1) << 5
#define PLATFORM_WINDOW_OPENGL     UINT32_C(1) << 6
#define PLATFORM_WINDOW_VULKAN     UINT32_C(1) << 7

struct platform_window;

struct platform_window_desc {
	uint32_t width;
	uint32_t height;
	uint32_t features_flag;
};

struct plaform_window *platform_window_create(struct platform *parent, struct platform_window_desc window_desc);
void platform_window_destroy(struct platform_window *window);

#endif
