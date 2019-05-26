CXX=g++
CFLAGS=-std=c++17 `pkg-config --cflags opencv`
LIBS=`pkg-config --libs opencv` -lm

SRCS=color_table.o convert_color.o detector_tool.o color_table_editor.o

all: color_table_editor

color_table_editor: $(SRCS)
	$(CXX) $(CFLAGS) -o $@ $^ $(LIBS)
	
%.o: %.cpp
	$(CXX) $(CFLAGS) -c $^ $(LIBS)

.PHONY: clean
clean:
	rm *.o

