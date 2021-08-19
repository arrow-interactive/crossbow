ifeq ($(OS),Windows_NT)
target_ext = .exe
endif

target = test$(target_ext)

outdir = bin
srcdir = src
objdir = obj

srcs = $(wildcard $(srcdir)/*.c)
objs = $(patsubst $(srcdir)/%,$(objdir)/%,$(srcs:.c=.o))

cflags = -std=c99 -Os -O3 -Icrossbow_core/include
ldflags = -s -Wall -Wextra -Lcrossbow_core/lib -lcrossbow_core -lSDL2 -lSDL2main

.PHONY: clean

all: crossbow_core/libcrossbow_core.so $(outdir)/$(target)

crossbow_core/libcrossbow_core.so:
	make -C crossbow_core

# Compile
$(objdir)/%.o: $(srcdir)/%.c
	$(CC) -c $< $(cflags) -o $@

# Link
$(outdir)/$(target): $(objs)
	$(CC) $(objs) $(ldflags) -o $@

clean:
	rm -f $(objs) bin/$(target)
	make -C crossbow_core clean
