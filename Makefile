CC=g++

CFLAGS=-c -std=c++0x -O3 -Wall
LDFLAGS=-lGLEW -lGL -lglut -lXi -L/usr/X11R6/lib
SOURCES=main.cpp OpenGLTools/ShaderTools.cpp MathTools/Mat4f.cpp MathTools/OpenGLMatrixTools.cpp MathTools/Vec3f.cpp OpenGLTools/GLCurve.cpp OpenGLTools/GLData.cpp Helpers/FileHelper.cpp
OBJECTS=$(SOURCES:.cpp=.o)
BIN=bin/
OBJ=$(BIN)obj/
EXECUTABLE=$(BIN)Rollercoaster

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	mv */*.o $(OBJ)
	mv *.o $(OBJ) 
	

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJ)*.o $(EXECUTABLE)
