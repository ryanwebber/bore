
NAME := bore

CC := g++-9

SRCDIR := src
BUILDDIR := build
INCDIR := include
TARGET := bin/$(NAME)
 
LUA_SOURCES := pkg/core.lua $(shell find pkg/rules -type f -name "*.lua")
LUA_BUNDLE := $(BUILDDIR)/bundle.lua
LUA_OBJECT := $(BUILDDIR)/__lua_embed.o

SRCEXT := cpp
HEXT := h
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o)) $(LUA_OBJECT)
CFLAGS := -g -std=c++17 -Wall
LIB := -llua -lm -ldl 
INC := -I $(INCDIR)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p bin
	$(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

$(LUA_BUNDLE): $(LUA_SOURCES)
	@mkdir -p $(BUILDDIR)
	cat $^ > $@

$(LUA_OBJECT): $(LUA_BUNDLE)
	@mkdir -p $(BUILDDIR)
	ld -r -b binary -o $@ $<

clean:
	$(RM) -r $(BUILDDIR) $(TARGET)

test:
	@echo "Testing not supported"

.PHONY: clean all

