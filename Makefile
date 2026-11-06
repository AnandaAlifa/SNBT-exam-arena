CC ?= gcc
PKG_CONFIG ?= pkg-config

RAYLIB_CFLAGS := $(shell $(PKG_CONFIG) --cflags raylib 2>/dev/null)
RAYLIB_LIBS := $(shell $(PKG_CONFIG) --libs raylib 2>/dev/null)

ifeq ($(strip $(RAYLIB_LIBS)),)
    RAYLIB_CFLAGS :=
    ifeq ($(OS),Windows_NT)
        RAYLIB_LIBS := -lraylib -lopengl32 -lgdi32 -lwinmm -luser32 -lm
    else
        RAYLIB_LIBS := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    endif
endif

CFLAGS ?= -std=c99 -Wall -Wextra -Wpedantic -O2
CFLAGS += -Isrc $(RAYLIB_CFLAGS)
LDLIBS += $(RAYLIB_LIBS)

ifeq ($(OS),Windows_NT)
    TARGET := utbk_tactics.exe
else
    TARGET := utbk_tactics
endif

SRC := src/main.c src/game.c src/ui.c src/screens.c src/exam.c src/demo_questions.c
OBJ := $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDLIBS)

src/%.o: src/%.c src/game.h src/ui.h src/screens.h src/exam.h src/demo_questions.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	$(RM) $(OBJ) $(TARGET)

.PHONY: all run clean
