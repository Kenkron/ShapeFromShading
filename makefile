COMPILER = g++
DEBUG = -O2
INCDIRS = lib/eigen/ lib/CImg
LIBS = m X11
CFLAGS += -pthread -L/usr/X11R6/lib -fopenmp
CFLAGS += $(addprefix -l, $(LIBS))
CFLAGS += $(addprefix -I, $(INCDIRS))

all: DEBUG = -O2
all: Test

debug: DEBUG = -g -Og
debug: Test

Test: WoodhamData.o
	$(COMPILER) $(DEBUG) -o test.exe src/test.cpp src/WoodhamData.cpp $(CFLAGS)

WoodhamData.o:
	$(COMPILER) $(DEBUG) -c src/WoodhamData.cpp $(CFLAGS)
