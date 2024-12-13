# Makefile for the IN330 port of CGDoom.
# Two target platforms: RISC-V and native (SDL2).

### Project structure.

NAME := CGDoom
SRC := $(wildcard cgdoom/*.c)

CFLAGS := -I cgdoom
# Extra optimization passes just in case
CFLAGS += -O3 -fgcse-sm -fgcse-las -fgcse-after-reload -fmerge-all-constants
# We remove some warnings that come up a lot in Doom
CFLAGS += -Wall -Wextra -Wno-sign-compare -Wno-unused-but-set-variable \
          -Wno-unused-but-set-parameter -Wno-implicit-fallthrough \
          -Wno-misleading-indentation

LDFLAGS :=

### Native SDL2 build.

CC ?= gcc

SDL2_CFLAGS := $(CFLAGS) -I src-sdl2 -DCG_EMULATOR $(pkg-config sdl2 --cflags)
SDL2_CFLAGS += -O0 -g
SDL2_LDFLAGS := $(LDFLAGS) $(shell pkg-config sdl2 --libs)

SDL2_SRC := $(wildcard src-sdl2/*.c)
SDL2_OBJ := $(SRC:cgdoom/%=build-sdl2/%.o) $(SDL2_SRC:src-sdl2/%=build-sdl2/%.o)

### RISC-V build.

RV_CC := riscv64-unknown-elf-gcc

RV_CFLAGS := $(CFLAGS) -march=rv64i -mabi=lp64 -I src-riscv -DCG_EMULATOR -ffreestanding
RV_CFLAGS += -O2
RV_LDFLAGS := $(LDFLAGS)

RV_SRC := $(wildcard src-riscv/*.c)
RV_OBJ := $(SRC:cgdoom/%=build-riscv/%.o) $(SDL2_SRC:src-riscv/%=build-riscv/%.o)


### Build rules.

all: all-riscv

#~

all-sdl2: $(NAME)

$(NAME): $(SDL2_OBJ)
	$(CC) $^ $(SDL2_CFLAGS) $(SDL2_LDFLAGS) -o $@

build-sdl2/%.c.o: cgdoom/%.c | build-sdl2/
	$(CC) $(SDL2_CFLAGS) -c $< -o $@

build-sdl2/%.c.o: src-sdl2/%.c | build-sdl2/
	$(CC) $(SDL2_CFLAGS) -c $< -o $@

#~

all-riscv: $(NAME)-riscv.elf

$(NAME)-riscv.elf: $(RV_OBJ)
	$(RV_CC) $^ $(RV_CFLAGS) $(RV_LDFLAGS) -o $@

build-riscv/%.c.o: cgdoom/%.c | build-riscv/
	$(RV_CC) $(RV_CFLAGS) -c $< -o $@

build-riscv/%.c.o: src-riscv/%.c | build-riscv/
	$(RV_CC) $(RV_CFLAGS) -c $< -o $@

#~

clean:
	rm -rf build-sdl2/ build-riscv/

%/:
	@ mkdir -p $@

.PRECIOUS: %/ build-sdl2/%.d build-riscv/%.d

include $(wildcard build-sdl2/*.d build-riscv/*.d)
