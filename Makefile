EE_BIN = CaptainTsubasa2.elf

EE_OBJS = \
	src/main.o \
	src/game/engine.o \
	src/game/player.o \
	src/game/battle.o \
	src/game/match.o \
	src/graphics/renderer.o \
	src/input/input.o

EE_LIBS = \
	-lpadx \
	-lgskit \
	-ldmakit \
	-lpatches \
	-lm

EE_INCS = \
	-I$(PS2SDK)/ee/include \
	-I$(PS2SDK)/common/include \
	-Isrc

EE_CFLAGS = -O2 -G0 -Wall

all: $(EE_BIN)

clean:
	rm -f $(EE_BIN) $(EE_OBJS)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
