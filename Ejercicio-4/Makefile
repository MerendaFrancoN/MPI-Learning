# Build Executable
SHELL=/bin/bash
# Overrides command line arguments
np = 0
size = 0

  all: 
	mpicc -g -std=c99 -Wall -o ./build/manager manager.c

	mpicc -g -std=c99 -Wall -o ./build/worker worker.c
  clean: 
	$(RM) ./build/manager ./build/worker


