
NAME := bore

CC := g++

SRCDIR := src
BUILDDIR := build
INCDIR := include
TARGET := bin/$(NAME)
 
SRCEXT := cpp
HEXT := h
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -std=c++11 -Wall 
LIB := -llua -lm -ldl 
INC := -I $(INCDIR)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	$(RM) -r $(BUILDDIR) $(TARGET)

test:
	@echo "Testing not supported"

.PHONY: clean all

