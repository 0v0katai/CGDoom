CC=sh-elf-gcc
CPP=sh-elf-g++
OBJCOPY=sh-elf-objcopy
MKG3A=mkg3a
RM=rm
CFLAGS=-m4a-nofpu -mb -fgcse-sm -fgcse-las -fgcse-after-reload -Isrc -O3 -fmerge-all-constants -mhitachi -fuse-linker-plugin -fstrict-volatile-bitfields -Wall -Wextra -Wno-sign-compare -Wno-unused-but-set-variable -Wno-unused-but-set-parameter -I../../../../include -lgcc -L../../../../lib -I./ -I../../cgdoom -D_FXCG_MINICOMPAT
LDFLAGS=$(CFLAGS) -nostartfiles -T../../../../toolchain/prizm.x -Wl,-static -lfxcg -lgcc -Wl,-Map=$(PROJ_NAME).map
CSOURCES=$(wildcard ../../cgdoom/*.c)
ASMSOURCES=$(wildcard ../../cgdoom/*.s)
OBJECTS=$(CSOURCES:.c=.o) $(ASMSOURCES:.s=.o)
PROJ_NAME=CG_Doom
BIN=$(PROJ_NAME).bin
ELF=$(PROJ_NAME).elf
ADDIN=$(PROJ_NAME).g3a
 
all: $(ADDIN)
 
 
$(ADDIN): $(BIN)
	$(MKG3A) -n : -i uns:CGDOOM_UNSELECTED.png -i sel:CGDOOM_SELECTED.png $< $@
 
.s.o:
	$(CC) -c $(CFLAGS) $< -o $@
 
.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
	
.cpp.o:
	$(CC) -c $(CFLAGS) $< -o $@
	
.cc.o:
	$(CC) -c $(CFLAGS) $< -o $@

$(ELF): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
$(BIN): $(ELF)
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(BIN)

clean:
	rm -f $(OBJECTS) $(PROJ_NAME).bin $(PROJ_NAME).elf $(ADDIN)
