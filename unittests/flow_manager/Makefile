#
# Copyright (C) 2011 NEC Corporation
#

TREMA = ../..
SRCS_CMOCKERY = /usr/local/src/cmockery-0.1.2/src/google
OBJS_CMOCKERY = /usr/local/lib
FLOW_MANAGER = $(TREMA)/src/flow_manager
FLOW_MANAGER_EXAMPLE =$(TREMA)/src/flow_manager/examples

CC = gcc
AR = ar
RANLIB = ranlib

CFLAGS = $(shell $(TREMA)/trema-config --cflags) -I$(FLOW_MANAGER) -I$(SRCS_CMOCKERY) -I$(FLOW_MANAGER_EXAMPLES) -std=gnu99 -D_GNU_SOURCE -g -fno-strict-aliasing -Werror -Wall -Wextra -Wformat=2 -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wfloat-equal -Wpointer-arith
LDFLAGS = -L$(FLOW_MANAGER) -L$(OBJS_CMOCKERY) -lpath -lcmockery $(shell $(TREMA)/trema-config --libs)

TARGET_LIB = libpath.a
SRCS_LIB = libpath.c flow_manager_interface.c
OBJS_LIB = $(SRCS_LIB:.c=.o)

TARGET_TEST = lib_path_test
SRCS_TEST = lib_path_test.c
OBJS_TEST = $(SRCS_TEST:.c=.o)

TARGETS = $(TARGET_TEST)
SRCS = $(SRCS_TEST)
OBJS = $(OBJS_TEST)

DEPENDS = .depends

.PHONY: all clean

.SUFFIXES: .c .o

all: depend $(TARGETS)

$(TARGET_TEST): $(OBJS_TEST)
	$(CC) $(OBJS_TEST) $(LDFLAGS) -o $@

$(TARGET_LIB): $(OBJS_LIB)
	$(AR) -cq $@ $(OBJS_LIB)
	$(RANLIB) $@

.c.o:
	$(CC) $(CFLAGS) -c $<

depend:
	$(CC) -MM $(CFLAGS) $(SRCS) > $(DEPENDS)

clean:
	@rm -rf $(DEPENDS) $(OBJS) $(TARGETS) *~

-include $(DEPENDS)
