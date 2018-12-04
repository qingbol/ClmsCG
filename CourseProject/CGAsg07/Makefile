.SUFFIXES: .c .o .cpp .cc .cxx .C

UNAME = $(shell uname)
PLATFORM = $(shell uname -p)

CC = gcc
C++ = g++
#COPTS = -g -Wall
#COPTS = -g -Wno-deprecated-declarations -std=c++11
COPTS = -g -std=c++11 -DGL_GLEXT_PROTOTYPES -Wno-deprecated-declarations

ifeq ("$(shell uname)", "Linux")
INC = \
 -I/usr/local/include #./glad/include
LIBDIR = \
  -L/usr/lib -L/usr/local/lib -L/usr/X11R6/lib
LIBS = \
  -lpthread -lglfw -lGLU -lGL -lXi -lXext -lX11 -lm -ldl #-lXmu
# what learnopengl.com suggests
#LIBS = \
#  -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl
else
ifeq ("$(shell uname)", "Darwin")
INC = \
 -I/usr/local/include
LIBDIR = \
  -L/usr/local/lib
LIBS = \
  -framework OpenGL -framework Foundation -lstdc++ -lglfw
endif
endif

# .c.o:
%.o: %.c
	$(CC) -c $(INC) -g -o $@ $<

%.o: %.cpp
# .cpp.o:
	$(C++) -c $(INC) $(COPTS) -o $@ $<

all : hello 

OBJECTS = \
obj_loader.o texture.o shader.o

hello : $(OBJECTS) hello.o 
	$(C++) -o $@ $(INC) $(COPTS) $(OBJECTS) $@.o $(LIBDIR) $(LIBS)

hello.o: hello.cpp
texture.o: texture.cpp
shader.o: shader.cpp

clean :
	rm -f *.o
	rm -rf hello
