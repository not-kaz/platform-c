#ifndef PLATFORM_H
#define PLATFORM_H

enum platform_type {
	PLATFORM_TYPE_DUMMY,
	PLATFORM_TYPE_X11
};

enum platform_native_override_type {
	PLATFORM_OVERRIDE_TYPE_NONE,
	PLATFORM_OVERRIDE_TYPE_AUGMENT,
	PLATFORM_OVERRIDE_TYPE_REPLACE
};

struct platform_native_overrides {
	enum platform_override_type override_type;
	void (*bootstrap_init)(void *native_ctx, void *user_state);
	void (*bootstrap_shutdown)(void *native_ctx, void *user_data);
	uint32_t set_flag;
	uint32_t clear_flag;
	void *user_state;
};

struct platform_allocator {
	(void *)(*allocate)(void *user_state, size_t size);
	(void)(*free)(void *user_state, void *ptr);
	void *user_state;
};

struct platform_desc {
	enum platform_type type;
	struct platform_allocator *allocator;
	struct platform_native_overrides *overrides;
};

struct platform *platform_create(struct platform *platform_desc);
void platform_destroy(struct platform *platform);
void *platform_get_native_ctx_handle(struct platform *platform);
void *platform_get_native_window_handle(struct platform *platform);

#endif
