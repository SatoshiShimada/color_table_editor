
CXX=g++
CFLAGS=-std=c++17 `pkg-config --cflags opencv`
LIBS=`pkg-config --libs opencv` -lm

SRC=main.o convert_color.o color_table.o detector_tool.o

all: color_table_editor
#all: test mouse color_table_editor

test: $(SRC)
	$(CXX) $(CFLAGS) -o $@ $^ $(LIBS)

mouse: mouse.o
	$(CXX) $(CFLAGS) -o $@ $^ $(LIBS)
	
color_table_editor: color_table.o convert_color.o detector_tool.o color_table_editor.o
	$(CXX) $(CFLAGS) -o $@ $^ $(LIBS)
	
%.o: %.cpp
	$(CXX) $(CFLAGS) -c $^ $(LIBS)

