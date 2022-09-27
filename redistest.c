#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hiredis/hiredis.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>
#include <unistd.h>
/*
TODO:   #-Adicionar medição de tempo mais precisa como mostrado na última reunião
        -Adicionar possibilidade de utilização de parâmetros argc e argv
        -Adicionar possibilidade de dar set e get em chaves aleatórias
        #-Mudar calculo da chance entre get e set
        #-Paralelizar o programa
        -Medir vazão com uma thread específica e salvar em um txt ou csv
        -Refatorar if e else dentro do for para usar uma string e redisCommand ser genérico
*/

int main (int argc, char **argv) {
redisReply *reply[4];
redisContext *c;
int n_reqs = 100;
int get_chance = 50;
struct timeval t1, t2;
char key[5], value[5], array_chaves[256][5];
int n_chaves = 256;

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

reply[0] = redisCommand(c,"SET %s %s","foo","bar");
freeReplyObject(reply[0]);

#pragma omp parallel for private(t1,t2,reply)
for (int i=0; i<10; i++) {
    printf("%d\n", i);
}

omp_set_num_threads(4);

#pragma omp parallel for private(key)
for (int i=0; i< n_chaves; i++) {
    snprintf(key, 4, "%d", rand()%1000);
    strcpy(array_chaves[i][0], key);
}

#pragma omp parallel for firstprivate(t1,t2,reply)
for (int i=0; i<n_reqs; i++) {
   //gettimeofday(&t1, NULL);
    if (rand() % 100 > get_chance) {
        usleep(rand()%200);
        //strcpy(key, (char)rand()%1000);
        snprintf(key, 5, "%d", rand()%1000);
        //strcpy(value, (char)rand()%1000);
        snprintf(value, 5, "%d", rand()%1000);
        gettimeofday(&t1, NULL);
        #pragma omp critical
        reply[omp_get_thread_num()] = redisCommand(c,"SET %s %s",key, value);
        freeReplyObject(reply[omp_get_thread_num()]);
        gettimeofday(&t2, NULL);
        //printf("%d GET\n", i);
        printf("Tempo tomado para operação SET: %f\n", (t2.tv_sec -t1.tv_sec) + ((t2.tv_usec -t1.tv_usec)/1000000.0));
    } else {
        usleep(rand()%200);
        gettimeofday(&t1, NULL);
        #pragma omp critical
        reply[omp_get_thread_num()] = redisCommand(c,"GET %s %s",array_chaves[rand()%256]);
        //printf("%s\n",reply->str);
        freeReplyObject(reply[omp_get_thread_num()]);
        //printf("%d SET\n", i);
        gettimeofday(&t2, NULL);
        printf("Tempo tomado para operação GET: %f\n", (t2.tv_sec -t1.tv_sec) + ((t2.tv_usec -t1.tv_usec)/1000000.0));
    }
}


redisFree(c);
}
