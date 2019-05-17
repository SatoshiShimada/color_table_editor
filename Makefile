
CXX=g++
CFLAGS=-std=c++17 `pkg-config --cflags opencv`
LIBS=`pkg-config --libs opencv`

SRC=main.cpp convert_color.cpp color_table.cpp detector_tool.cpp

all: test mouse

test: $(SRC)
	$(CXX) $(CFLAGS) -o $@ $^ $(LIBS)

mouse: mouse.cpp
	$(CXX) $(CFLAGS) -o $@ $^ $(LIBS)
	
