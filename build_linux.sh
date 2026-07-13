# TEST BUILD
gcc -g main.c platform_x11.c -lX11 -Wall -Wextra -std=c99 -pedantic \
	-Wconversion -Wsign-conversion -Wshadow -Wundef -Wformat=2 \
	-Wstrict-prototypes -Wmissing-prototypes \
	-Wpointer-arith -Wcast-qual -Wwrite-strings -Wnull-dereference -fanalyzer
