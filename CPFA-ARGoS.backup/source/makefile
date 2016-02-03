include makevars

lib:
	cd ga-mpi; $(MAKE)

all: lib example

clean:
	-rm example
	-rm *.o
	-cd ga-mpi
	-rm *.o 
	- rm *.a

example: example.c
	$(CXX) -g -Wall -I. -lm -std=c++11 -c example.c -o example.o
	$(CXX) example.o -o example -L./ga-mpi -lga-mpi -lm

