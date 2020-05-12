# Build Executable
SHELL=/bin/bash
# Overrides command line arguments
np = 0
size = 0

  all: 
	mpicc -g -std=c99 -Wall -o ./build/manager manager.c

	mpicc -g -std=c99 -Wall -o ./build/worker worker.c
  clean: 
	$(RM) ./build/ejercicio*
  
  3A:
	mpirun -np $(np) ./build/ejercicio3A $(size)
  3B:
	mpirun -np $(np) ./build/ejercicio3B $(size)

