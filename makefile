#Not working properly, displays following error:
#	makefile:2: *** faltando o separador.  Pare.

CC=gcc
SOURCE_DIR := ./src
REDIS := $(SOURCE_DIR)/redis_code

make:
	$(CC) -o context_process $(REDIS)/context_process.c -fopenmp -lhiredis
	$(CC) -o context_thread $(REDIS)/context_thread.c -fopenmp -lhiredis
	mkdir -p bin
	mv context_process context_thread ./bin