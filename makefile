#Not working properly, displays following error:
#	makefile:2: *** faltando o separador.  Pare.

CC=gcc
BUILD_DIR := ./executable_files
SOURCE_DIR := ./src
REDIS := SOURCE_DIR/redis_code

make:
	$(CC) -o context_process ./src/redis_code/context_process.c -fopenmp -lhiredis
	$(CC) -o context_thread ./src/redis_code/context_thread.c -fopenmp -lhiredis
	mv context_process context_thread ./bin