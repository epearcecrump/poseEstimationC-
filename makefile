main: main.cpp posepairs.h colourmap.h bodyparts.h
	g++ `pkg-config --cflags --libs opencv4` -std=c++11 main.cpp -o main
