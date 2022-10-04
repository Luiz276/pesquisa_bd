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

void thread_vazao(int n_reqs) {
    /*
    Thread responsável por medir a vazão
    */
}

void loop(int n_threads, int n_chaves, int get_chance, int write_file_chance) {
    /*
    Loop multithreaded responsável por realizar todas as requisições
    */
    redisReply *reply[n_threads];
    redisContext *c;
    struct timeval t1, t2;
    double tf;
    char key[5], value[5];
    FILE *fptr;

    // estabelecer conexão e abrir arquivo para escrita

    // loop

    // fechar arquivo e terminar conexão
}

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
    char *ptr;
    int n_threads = strtol(argv[1],&ptr,10);
    redisReply *reply[n_threads];
    redisContext *c;
    int n_chaves = strtol(argv[2],&ptr,10);
    int get_chance = strtol(argv[3],&ptr,10), write_file_chance = strtol(argv[4],&ptr,10);
    struct timeval t1, t2;
    double tf;
    char key[5], value[5];
    FILE *fptr;
    int n_reqs = strtol(argv[5],&ptr,10);

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
    reply[0] = redisCommand(c,"GET %s","ufsc");
    if(reply[0]->elements > 0) {
        printf("%s\n",reply[0]->str);
    }
    freeReplyObject(reply[0]);
    reply[0] = redisCommand(c,"SET %s %s","foo","bar");
    freeReplyObject(reply[0]);


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

    fptr = fopen("./redistest.txt", "w");

    /*
    #pragma omp parallel num_threads(2)
    {
        #pragma single()
        thread_vazao(n_reqs);

        #pragma single
        loop_multithread(n_threads);
    }
    */

    #pragma omp parallel for firstprivate(t1,t2,reply)
    for (int i=0; i<n_reqs; i++) {
        //gettimeofday(&t1, NULL);
        if (rand() % 100 > get_chance) {
            usleep(rand()%200);
            snprintf(key, 5, "%d", rand()%1000);
            snprintf(value, 5, "%d", rand()%1000);
            gettimeofday(&t1, NULL);
            #pragma omp critical
            reply[omp_get_thread_num()] = redisCommand(c,"SET %s %s",key, value);
            freeReplyObject(reply[omp_get_thread_num()]);
            gettimeofday(&t2, NULL);
            //printf("%d GET\n", i);
            tf = (t2.tv_sec -t1.tv_sec) + ((t2.tv_usec -t1.tv_usec)/1000000.0);
            if (rand()%100 < write_file_chance) {
                fprintf(fptr, "SET: %f\n", tf);
                printf("Tempo tomado para operação SET: %f\n", tf);
            }
        } else {
            usleep(rand()%200);
            snprintf(key, 5, "%d", rand()%1000);
            gettimeofday(&t1, NULL);
            #pragma omp critical
            reply[omp_get_thread_num()] = redisCommand(c,"GET %s",key);
            //printf("%s\n",reply->str);
            freeReplyObject(reply[omp_get_thread_num()]);
            gettimeofday(&t2, NULL);
            tf = (t2.tv_sec -t1.tv_sec) + ((t2.tv_usec -t1.tv_usec)/1000000.0);
            if (rand()%100 < write_file_chance) {
                if (reply[omp_get_thread_num()]->elements > 0) {
                    fprintf(fptr, "GET: %f\n", tf);
                    printf("Tempo tomado para operação GET: %f\n", tf);
                } else {
                    fprintf(fptr, "GET com chave inválida: %f\n", tf);
                    printf("Tempo tomado para operação GET com chave inválida: %f\n", tf);
                }
            }
        }
    }
    /*
    reply[0] = redisCommand(c,"latency %s %s", "127.0.0.1", "6379");
    printf("%s\n",reply[0]->str);
    */
    fclose(fptr);

    redisFree(c);
}
