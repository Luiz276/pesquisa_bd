CC=gcc
files := context_process.c context_thread.c
BUILD_DIR := ./executable_files

make: context_process.c context_thread.c
	$(CC) -o context_process context_process.c -fopenmp -lhiredis
	$(CC) -o context_thread context_thread.c -fopenmp -lhiredis

clean:
	rm -f context_process
	rm -f context_thread