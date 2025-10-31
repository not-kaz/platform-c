#ifndef PLATFORM_WINDOW_INTERNAL_H
#define PLATFORM_WINDOW_INTERNAL_H

struct platform_window {
	uint32_t width;
	uint32_t height;
	uint32_t features_flag;
	void *native_handle;
	struct platform *platform;
};

#endif
