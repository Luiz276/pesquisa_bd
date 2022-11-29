CC=gcc

make: context_process.c context_thread.c
	$(CC) -o server server.c
	$(CC) -o client client.c 

clean:
	rm server
	rm client