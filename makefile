SHELL = /bin/sh
VPATH = ./src:./src/headers

CXX = g++
CXXFLAGS = -g -Wall -Wextra -std=c++11 -Iheaders 
LDFLAGS = pkg-config --cflags --libs opencv4
HEADERS = posepairs.h colourmap.h bodyparts.h
RM = rm -f
PROGRAM = main

.PHONY: all clean

$(PROGRAM): $(PROGRAM).cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) `$(LDFLAGS)` $< -o $@

clean:
	-$(RM) $(PROGRAM)
