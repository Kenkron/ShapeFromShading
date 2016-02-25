COMPILER = g++
DEBUG = -g
INCDIRS = lib/eigen/
CFLAGS += $(addprefix -I, $(INCDIRS))


all: Test

Test:
	$(COMPILER) $(CFLAGS) $(DEBUG) -o test src/test.cpp
