#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hiredis/hiredis.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>
#include <unistd.h>
//#include "redistest.h"
/*
TODO:   #-Adicionar medição de tempo mais precisa como mostrado na última reunião
        #-Adicionar possibilidade de utilização de parâmetros argc e argv
        #-Adicionar possibilidade de dar set em chaves aleatórias
        #-Get em chaves aleatórias
        #-Mudar calculo da chance entre get e set
        #-Paralelizar o programa
        -Medir vazão com uma thread específica
        #-salvar medições em um txt ou csv
        #-Latência de cada requisição salva em um txt
        -Refatorar if e else dentro do for para usar uma string e redisCommand ser genérico
*/

int main (int argc, char *argv[]) {
    /*
    Referência para parâmetros:
    1° - número de threads do programa
    2° - número de chaves a serem usadas (ainda não implementado)
    3° - chance de ocorrer uma requisição GET
    4° - chance de ocorrer o registro da latência da requisição em um .txt
    5° - número de reuisições a serem realizadas
    Exemplo: ./redistest 4 256 50 30 100
    */
    if (argc == 1) {
        printf("Não se esqueça de incluir os argumentos no comando\n");
        return 1;
    }
    char *ptr;
    int n_threads = strtol(argv[1], &ptr, 10);
    n_threads++;    // é adicionado 1 ao número de threads pois uma thread sempre será responsável por medir vazão
    redisReply *reply[n_threads];
    redisContext *c;
    int n_chaves = strtol(argv[2], &ptr, 10);
    int get_chance = strtol(argv[3], &ptr, 10), write_file_chance = strtol(argv[4], &ptr, 10);
    struct timeval t1, t2;
    double tf;
    char key[5], value[5];
    FILE *fptr_lat, *fptr_tp;   // um arquivo para armazenar latência e outro para armazenar throughput
    int n_reqs = strtol(argv[5], &ptr, 10);
    int reqs_env = 0, reqs_env_antigas = 0;

    srand(time(NULL));

    c = redisConnect("127.0.0.1", 6379);
    if (c->err) {
        printf("error: %s\n", c->errstr);
        return 1;
    }

    /* PINGs */
    reply[0] = redisCommand(c,"PING %s", "Hello World");
    printf("RESPONSE: %s\n", reply[0]->str);
    freeReplyObject(reply[0]);

    // Comandos de SET e GET
    //reply[0] = redisCommand(c,"SET %s %s","bar","bar");
    //freeReplyObject(reply[0]);
    /*
    reply[0] = redisCommand(c,"GET %s","ufsc");
    if(reply[0]->elements > 0) {
        printf("%s\n",reply[0]->str);
    }
    freeReplyObject(reply[0]);
    reply[0] = redisCommand(c,"SET %s %s","foo","bar");
    freeReplyObject(reply[0]);
    */


    omp_set_num_threads(n_threads);
    printf("n threads = %d\n", omp_get_max_threads());

    /*
    // Cria uma lista de chaves pré existentes
    #pragma omp parallel for private(key)
    for (int i=0; i< n_chaves; i++) {
        snprintf(key, 4, "%d", rand()%1000);
        strcpy(array_chaves[i][0], key);
    }
    */

    fptr_lat = fopen("./redis_lat.txt", "w");
    fptr_tp = fopen("./redis_tp.txt", "w");

    /*
    #pragma omp parallel num_threads(2)
    {
        #pragma single()
        thread_vazao(n_reqs);

        #pragma single
        loop_multithread(n_threads);
    }
    */

    #pragma omp parallel
    {
        if (omp_get_thread_num() == 0) {
            // código da thread de vazão

            do {
                sleep(1);    // sleep de 1 segundo
                if (rand()%100 < write_file_chance) {
                    #pragma omp critical
                    fprintf(fptr_tp, "%d\n", (reqs_env - reqs_env_antigas));
                    //printf("Tempo tomado para operação SET: %f\n", tf);
                }
                reqs_env_antigas = reqs_env;
                //sleep(1);    // sleep de 1 segundo
            } while (reqs_env < n_reqs);
        }
        else {
            #pragma omp for firstprivate(t1,t2,reply)
            for (int i=0; i<n_reqs; i++) {
                //gettimeofday(&t1, NULL);
                if (rand() % 100 > get_chance) {
                    usleep(rand()%200);
                    snprintf(key, 5, "%d", rand()%n_chaves);
                    snprintf(value, 5, "%d", rand()%1000);
                    gettimeofday(&t1, NULL);
                    #pragma omp critical
                    {
                        reply[omp_get_thread_num()] = redisCommand(c,"SET %s %s",key, value);
                        freeReplyObject(reply[omp_get_thread_num()]);
                        reqs_env++;
                        gettimeofday(&t2, NULL);
                    }
                    tf = (t2.tv_sec -t1.tv_sec) + ((t2.tv_usec -t1.tv_usec)/1000000.0);
                    if (rand()%100 < write_file_chance) {
                        fprintf(fptr_lat, "SET: %f\n", tf);
                        printf("Tempo tomado para operação SET: %f\n", tf);
                    }
                } else {
                    usleep(rand()%200);
                    snprintf(key, 5, "%d", rand()%n_chaves);
                    gettimeofday(&t1, NULL);
                    #pragma omp critical
                    {
                        reply[omp_get_thread_num()] = redisCommand(c,"GET %s",key);
                        freeReplyObject(reply[omp_get_thread_num()]);
                        reqs_env++;
                        gettimeofday(&t2, NULL);
                    }
                    tf = (t2.tv_sec -t1.tv_sec) + ((t2.tv_usec -t1.tv_usec)/1000000.0);
                    if (rand()%100 < write_file_chance) {
                        if (reply[omp_get_thread_num()]->elements > 0) {
                            fprintf(fptr_lat, "GET: %f\n", tf);
                            printf("Tempo tomado para operação GET: %f\n", tf);
                        } else {
                            fprintf(fptr_lat, "GET com chave inválida: %f\n", tf);
                            printf("Tempo tomado para operação GET com chave inválida: %f\n", tf);
                        }
                    }
                }
            }
        }
    }

    fclose(fptr_tp);
    fclose(fptr_lat);

    redisFree(c);
    return 0;
}
