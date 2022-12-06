#Not working properly, displays following error:
#	makefile:2: *** faltando o separador.  Pare.

BUILD_DIR := ./executable_files
SOURCE_DIR := ./src

make: redis socket

redis:
	$(MAKE) -C SOURCE_DIR/redis_code

socket:
	$(MAKE) -C SOURCE_DIR/socket_programing