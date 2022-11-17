#Not working properly, displays following error:
#	makefile:2: *** faltando o separador.  Pare.
all:
    $(MAKE) -C source_code/redis_code
	$(MAKE) -C source_code/socket_programing

redis:
	$(MAKE) -C source_code/redis_code

socket:
	$(MAKE) -C source_code/socket_programing