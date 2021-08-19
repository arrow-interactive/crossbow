ifeq ($(OS),Windows_NT)
target_ext = .dll
else
target_ext = .so
endif

target = lib/libcrossbow_core$(target_ext)

srcs = $(wildcard *.c)
objs = $(srcs:.c=.o)

cflags = -std=c99 -Os -O3 -Iinclude/crossbow

.PHONY: clean

all: $(target)

# Compile
%.o: %.c
	$(CC) -c $< $(cflags) -o $@

# Link
$(target): $(objs)
	$(AR) rcs $@ $(objs)

clean:
	rm -f $(objs) $(target)
