
CXX=g++
CFLAGS=-std=c++17 `pkg-config --cflags opencv`
LIBS=`pkg-config --libs opencv`

SRC=main.cpp convert_color.cpp color_table.cpp detector_tool.cpp

all: color_table_editor
#all: test mouse color_table_editor

test: $(SRC)
	$(CXX) $(CFLAGS) -o $@ $^ $(LIBS)

mouse: mouse.cpp
	$(CXX) $(CFLAGS) -o $@ $^ $(LIBS)
	
color_table_editor: color_table_editor.cpp color_table.cpp convert_color.cpp detector_tool.cpp
	$(CXX) $(CFLAGS) -o $@ $^ $(LIBS)
	
