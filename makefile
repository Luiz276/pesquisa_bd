#Not working properly, displays following error:
#	makefile:2: *** faltando o separador.  Pare.

CC=gcc
BUILD_DIR := ./executable_files
SOURCE_DIR := ./src
REDIS := SOURCE_DIR/redis_code

make:
	cd ./src/redis_code; $(CC) -o context_process context_process.c -fopenmp -lhiredis; $(CC) -o context_thread context_thread.c -fopenmp -lhiredis
	mv ./src/redis_code/context_process ./bin
	mv ./src/redis_code/context_thread ./bin