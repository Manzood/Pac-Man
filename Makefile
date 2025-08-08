OBJS = Pac-Man-v2.c
OBJ_NAME = PacMan

# Prefer pkg-config; fallback to sdl2-config on systems lacking pkg-config metadata
SDL2_CFLAGS := $(shell pkg-config --cflags sdl2 SDL2_image SDL2_ttf 2>/dev/null)
SDL2_LIBS := $(shell pkg-config --libs sdl2 SDL2_image SDL2_ttf 2>/dev/null)
ifeq ($(strip $(SDL2_CFLAGS)$(SDL2_LIBS)),)
SDL2_CFLAGS := $(shell sdl2-config --cflags 2>/dev/null)
SDL2_LIBS := $(shell sdl2-config --libs 2>/dev/null) -lSDL2_image -lSDL2_ttf
endif

CFLAGS := -Wall -Wextra -Wno-deprecated-declarations $(SDL2_CFLAGS)
LDFLAGS := $(SDL2_LIBS)

all: $(OBJ_NAME)

$(OBJ_NAME): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

.PHONY: clean run
clean:
	rm -f $(OBJ_NAME)

run: $(OBJ_NAME)
	./$(OBJ_NAME)
