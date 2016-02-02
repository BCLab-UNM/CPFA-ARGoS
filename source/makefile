include makevars

all: lib example

lib:
	cd ga-mpi; $(MAKE)

example: example.c
	$(CXX) -g -Wall -I. -lm -std=c++11 -c example.c -o example.o
	$(CXX) example.o -o example -L./ga-mpi -lga-mpi -lm

