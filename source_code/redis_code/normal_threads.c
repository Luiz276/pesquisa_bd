#include <stdio.h>
#include <hiredis/hiredis.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>

void *main_loop(int n_reqs, int n_chaves, int chance_req, int chance_write);

void *throughput_thread(int n_reqs, int chance_write, struct timeval ti);

int main (int argc, char *argv[]) {
    // Lidar com argc e argv

    // Abrir .csv

    // Criar threads
    
    // Esperar execução
    
    // Fechar .csv
    
    // Terminar programa

    return 0;
}