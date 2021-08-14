# Makefile for the fx-CG 50 / Graph 90+E version of CGDoom.
# Two target platforms: fx-CG and native (SDL2).

### Project structure.

NAME := CGDoom
SRC := $(wildcard cgdoom/*.c)

CFLAGS := -I cgdoom
# Extra optimization passes just in case
CFLAGS += -O3 -fgcse-sm -fgcse-las -fgcse-after-reload -fmerge-all-constants
# We remove some warnings that come up a lot in Doom
CFLAGS += -Wall -Wextra -Wno-sign-compare -Wno-unused-but-set-variable -Wno-unused-but-set-parameter

LDFLAGS :=

### fx-CG 50 build.

CG_CC      := sh-elf-gcc
CG_OBJCOPY := sh-elf-objcopy

# The libfxcg install folder (adjust to your needs)
LIBFXCG_ROOT := ../..

# Platform selection
CG_CFLAGS := $(CFLAGS) -I src-cg -m4a-nofpu -mb -mhitachi
# Enforce strict access size in volatile bitfields, needed for PRAM mainly
CG_CFLAGS += -fstrict-volatile-bitfields
# We use libfxcg and libc within the libfxcg repository
CG_CFLAGS += -I $(LIBFXCG_ROOT)/include -D_FXCG_MINICOMPAT

# Libraries specific to libfxcg and the cross-compiler
CG_LDFLAGS := $(LDFLAGS) -L $(LIBFXCG_ROOT)/lib -lfxcg -lgcc
# The build is mostly free-standing, with the libfxcg linker script
CG_LDFLAGS += -nostartfiles -T $(LIBFXCG_ROOT)/toolchain/prizm.x
# Generate a map of every function and global variable for debugging
CG_LDFLAGS += -Wl,-Map=build-cg/$(NAME).map

CG_SRC := $(SRC) $(wildcard src-cg/*.c src-cg/*.s)
CG_OBJ := $(CG_SRC:cgdoom/%=build-cg/%.o)

### Native SDL2 build.

CC ?= gcc

SDL2_CFLAGS := $(CFLAGS) -I src-sdl2 -DCG_EMULATOR $(pkg-config sdl2 --cflags)
SDL2_LDFLAGS := $(LDFLAGS) $(pkg-config sdl2 --libs)

SDL2_SRC := $(SRC) $(wildcard src-sdl2/*.c)
SDL2_OBJ := $(SDL2_SRC:cgdoom/%=build-sdl2/%.o)

### Build rules.

all: all-cg50

#~

all-cg50: $(NAME).g3a

$(NAME).g3a: build-cg/$(NAME).bin
	mkg3a -n : -i uns:CGDOOM_UNSELECTED.png -i sel:CGDOOM_SELECTED.png $< $@

build-cg/%.s.o: cgdoom/%.s | build-cg/
	$(CG_CC) $(CG_CFLAGS) -c $< -o $@

build-cg/%.c.o: cgdoom/%.c | build-cg/
	$(CG_CC) $(CG_CFLAGS) -c $< -o $@

build-cg/%.c.o: src-cg/%.c | build-cg/
	$(CG_CC) $(CG_CFLAGS) -c $< -o $@

build-cg/$(NAME).elf: $(CG_OBJ)
	$(CG_CC) $(CG_CFLAGS) $(CG_LDFLAGS) $^ -o $@

build-cg/$(NAME).bin: build-cg/$(NAME).elf
	$(CG_OBJCOPY) -O binary $< $@

#~

all-sdl2: $(NAME)

$(NAME): $(SDL2_OBJ)
	$(CC) $(SDL2_CFLAGS) $(SDL2_LDFLAGS) $^ -o $@

build-sdl2/%.c.o: cgdoom/%.c | build-sdl2/
	$(CC) $(SDL2_CFLAGS) -c $< -o $@

build-sdl2/%.c.o: src-sdl2/%.c | build-sdl2/
	$(CC) $(SDL2_CFLAGS) -c $< -o $@

#~

clean:
	rm -rf build-cg/ build-sdl2/

%/:
	@ mkdir -p $@

.PRECIOUS: %/ build-cg/%.d build-sdl2/%.d

include $(wildcard build-cg/*.d build-sdl2/*.d)
