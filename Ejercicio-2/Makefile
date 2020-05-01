# Build Executable
SHELL=/bin/bash
# Overrides command line arguments
np = 0
laps = 1

  all: 
	mpicc -g -std=c99 -Wall -o ./build/ejercicio2 ejercicio2.c

  clean: 
	$(RM) ./build/ejercicio*
  EJ2:
	mpirun -np $(np) ./build/ejercicio2 $(laps)

