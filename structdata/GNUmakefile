CLANG = /opt/local/bin/clang-mp-3.2

CC = $(CLANG)
LD = $(CLANG)

CFLAGS = $(shell pkg-config --cflags glib-2.0) -Wall -Wextra -g -I./include
LDFLAGS = $(shell pkg-config --libs glib-2.0) -dynamiclib

.PHONY: all
all: libstructdata.dylib libstructdata.a

libstructdata.dylib: $(patsubst %.c,%.o,$(wildcard ./*.c))
	$(LD) $(LDFLAGS) $^ -o $@

libstructdata.a: $(patsubst %.c,%.o,$(wildcard ./*.c))
	$(AR) $(ARFLAGS) $@ $^
