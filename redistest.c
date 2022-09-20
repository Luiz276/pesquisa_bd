#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hiredis/hiredis.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>
/*
TODO:   -Adicionar medição de tempo mais precisa como mostrado na última reunião
        -Adicionar possibilidade de utilização de parâmetros argc e argv
        -Adicionar possibilidade de dar set e get em chaves aleatórias
        -Mudar calculo da chance entre get e set
        -Paralelizar o programa
*/

int main (int argc, char **argv) {
redisReply *reply;
redisContext *c;
int n_reqs = 100;
double get_chance = 0.5;
struct timeval t1, t2;

srand(time(NULL));

c = redisConnect("127.0.0.1", 6379);
if (c->err) {
    printf("error: %s\n", c->errstr);
    return 1;
}

/* PINGs */
reply = redisCommand(c,"PING %s", "Hello World");
printf("RESPONSE: %s\n", reply->str);
freeReplyObject(reply);

// Comandos de SET e GET

reply = redisCommand(c,"SET %s %s","foo","bar");
freeReplyObject(reply);

//#pragma omp parallel{ 
for (int i=0; i<n_reqs; i++) {
    gettimeofday(&t1, NULL);
    if (((double)rand() / (double)RAND_MAX) > get_chance) {
        reply = redisCommand(c,"SET %s %s","foo","bar");
        freeReplyObject(reply);
        gettimeofday(&t2, NULL);
        printf("Tempo tomado para operação SET: %f\n", (t2.tv_sec -t1.tv_sec) + ((t2.tv_usec -t1.tv_usec)/1000000.0));
    } else {
        reply = redisCommand(c,"GET %s","foo");
        //printf("%s\n",reply->str);
        freeReplyObject(reply);
        gettimeofday(&t2, NULL);
        printf("Tempo tomado para operação GET: %f\n", (t2.tv_sec -t1.tv_sec) + ((t2.tv_usec -t1.tv_usec)/1000000.0));
    }
}
//}

redisFree(c);
}
