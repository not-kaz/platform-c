#include "platform_window.h"
#include "platform_window_internal.h"

struct platform_window *platform_window_create(struct platform *parent, 
		struct platform_window_desc window_desc)
{
	struct platform_window *window;
	struct platform_allocator *alloc = platform->allocator;

	if (!platform) {
		return NULL;
	}
	window = allocator->allocate(allocator->user_state, sizeof(struct platform_window));
	if (!window) {
		return NULL;
	}
	window->native_handle = platform->backend->window_create(window_desc);
	if (!window->native_handle) {
		allocator->free(allocator->user_state, window);
		return NULL;
	}
	return window;
}

void platform_window_destroy(struct platform_window *window)
{
	if (window) {
		window->parent_platform->backend->window_destroy(window->native_handle);
		window->parent_platform->allocator->free(window);
	}
}
