#
# Makefile
#
# Simple logging library.
#
# https://github.com/e03213ac/liblog
#
# This work belongs to the Public Domain. Everyone is free to use, modify,
# republish, sell or give away this work without prior consent from
# anybody.
#
# This software is provided on an "AS IS" basis, without warranty of any
# kind. Use at your own risk! Under no circumstances shall the author(s)
# or contributor(s) be liable for damages resulting directly or indirectly
# from the use or non-use of this documentation.
#

include Makefile.in

CFLAGS  = -Wall -Wextra -Werror
CFLAGS += -Wmissing-prototypes
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-missing-field-initializers
CFLAGS += -fno-delete-null-pointer-checks
 
BUILD   ?= default
ifeq ($(BUILD),default)
CFLAGS  += -O2 -ggdb
CFLAGS  += -fstack-protector-all -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2
else ifeq ($(BUILD),dbg)
CFLAGS  += -O0 -ggdb
else ifeq ($(BUILD),opt)
CFLAGS  += -O3
CFLAGS  += -fstack-protector-all -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2
CFLAGS  += -Wno-unused-result
else
$(error Invalid BUILD type)
endif
 
.DEFAULT_GOAL = all
.PHONY: all
all: $(LIBLOG)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
