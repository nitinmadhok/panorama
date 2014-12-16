CC	= g++

ifeq ("$(shell whoami)", "nitinmadhok")
  CFLAGS      = -g -Wall -Wno-deprecated -I/usr/local/Cellar/openimageio/1.4.8_1/include 
  LDFLAGS     = -L/usr/local/Cellar/openimageio/1.4.8_1/lib/ -framework Foundation -framework GLUT -framework OpenGL -lOpenImageIO -lm

else
  CFLAGS      = -g -I/group/dpa/include
  LDFLAGS     = -L/usr/lib64/ -L/group/dpa/lib -lglut -lGL -lGLU -lOpenImageIO -lm
endif

PROJECT	= panorama
OBJECTS = panorama.o Transformation.o Matrix.o Utility.o Vector.o 

${PROJECT}: ${OBJECTS}
	${CC} ${LFLAGS} -o ${PROJECT} ${OBJECTS} ${LDFLAGS}

%.o: %.cpp
	${CC} -c ${CFLAGS} *.cpp

clean:
	rm -f core.* *.o *~ ${PROJECT}
