CC=gcc

make: context_process.c context_thread.c
	$(CC) -o context_process context_process.c -fopenmp -lhiredis
	$(CC) -o context_thread context_thread.c -fopenmp -lhiredis

clean:
	rm context_process
	rm context_thread