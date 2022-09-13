#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hiredis/hiredis.h>
#include <time.h>

// TODO:    -adicionar medição de tempo mais precisa como mostrado na última reunião
//          -Adicionar possibilidade de utilização de parâmetros argc e argv
//          -Adicionar possibilidade de dar set e get em chaves aleatórias

int main (int argc, char **argv) {
redisReply *reply;
redisContext *c;
int n_reqs = 100;
double get_chance = 0.5;

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

for (int i=0; i<n_reqs; i++) {
    if (((double)rand() / (double)RAND_MAX) > get_chance) {
        reply = redisCommand(c,"SET %s %s","foo","bar");
        freeReplyObject(reply);
        printf("set");
    } else {
        reply = redisCommand(c,"GET %s","foo");
        printf("%s\n",reply->str);
        freeReplyObject(reply);
    }
}

// time_t start = time(NULL);

// reply = redisCommand(c,"SET %s %s","foo","bar");
// freeReplyObject(reply);

// printf("Time taken by SET command: %.5f\n", time(NULL)-start);

// start = time(NULL);

// reply = redisCommand(c,"GET %s","foo");
// printf("%s\n",reply->str);
// freeReplyObject(reply);

// printf("Time taken by GET command: %.5f\n", time(NULL)-start);

redisFree(c);
}