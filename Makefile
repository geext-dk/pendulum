CC = gcc
CPP = g++
CXXFLAGS = -g -Wall -O0 -march=native
CFLAGS = $(CXXFLAGS)
LIBS = -lGL -lglfw -ldl
INC= -I./include

IDIR = include
SDIR = src
ODIR = $(SDIR)/obj

_DEPS = shader.h camera.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = glad.o shader.o main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: Pendulum

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS) $(INC)

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CPP) -c -o $@ $< $(CXXFLAGS) $(INC)

Pendulum: $(OBJ)
	$(CPP) -o $@ $^ $(CXXFLAGS) $(LIBS) $(INC)

.PHONY: clean all

clean:
	rm -rf $(ODIR)/*.o Pendulum
