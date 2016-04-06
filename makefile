COMPILER = g++
DEBUG = -g
INCDIRS = lib/eigen/ lib/CImg
LIBS = m X11
CFLAGS += -pthread -L/usr/X11R6/lib
CFLAGS += $(addprefix -l, $(LIBS))
CFLAGS += $(addprefix -I, $(INCDIRS))
all: Test

Test:
	$(COMPILER) $(DEBUG) -o test.exe src/test.cpp src/WoodhamData.cpp $(CFLAGS)
