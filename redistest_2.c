#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hiredis/hiredis.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>
#include <unistd.h>

#define panicAbort(fmt, ...) \
    do { \
        fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__); \
        exit(-1); \
    } while (0)

/*
TODO:   #-Adicionar medição de tempo mais precisa como mostrado na última reunião
        #-Adicionar possibilidade de utilização de parâmetros argc e argv
        #-Adicionar possibilidade de dar set em chaves aleatórias
        #-Get em chaves aleatórias
        #-Mudar calculo da chance entre get e set
        #-Paralelizar o programa
        #-Medir vazão com uma thread específica
        #-salvar medições em um txt ou csv
        #-Latência de cada requisição salva em um txt
        -Refatorar if e else dentro do for para usar uma string e redisCommand ser genérico
        -Adiconar hostname e port como argumentos de linha de comando
*/

/* We aren't actually freeing anything here, but it is included to show that we can
 * have hiredis call our data destructor when freeing the context */
void privdata_dtor(void *privdata) {
    unsigned int *icount = privdata;
    printf("privdata_dtor():  In context privdata dtor (sent_reqs: %u)\n", *icount);
}

void pushReplyHandler(int *sent_reqs, redisReply *r) {

    /* Sanity check on the invalidation reply */
    /*
    if (reply->type != REDIS_REPLY_PUSH || reply->elements != 2 ||
        reply->element[1]->type != REDIS_REPLY_ARRAY ||
        reply->element[1]->element[0]->type != REDIS_REPLY_STRING)
    {
        panicAbort("%s", "Can't parse PUSH message!");
    }
    */

    /* Increment our sent_reqs count */
    sent_reqs += 1;

    //printf("pushReplyHandler(): TYPE OF REQUISITION '%s' (sent_reqs count: %d)\n",
    //       r->element[1]->element[0]->str, sent_reqs);

    freeReplyObject(r);
}

int main (int argc, char *argv[]) {
    /*
    Referência para parâmetros:
    1° - número de threads do programa
    2° - número de chaves a serem usadas (ainda não implementado)
    3° - chance de ocorrer uma requisição GET
    4° - chance de ocorrer o registro da latência da requisição em um .txt
    5° - número de requisições a serem realizadas
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
    const char *hostname = "127.0.0.1";
    int port = 6379;

    srand(time(NULL));

    redisOptions o = {0};
    REDIS_OPTIONS_SET_TCP(&o, hostname, port);
    REDIS_OPTIONS_SET_PRIVDATA(&o, &reqs_env, privdata_dtor);

    o.push_cb = pushReplyHandler;

    c = redisConnectWithOptions(&o);
    if (c == NULL || c->err)
        panicAbort("Connection error:  %s", c ? c->errstr : "OOM");
    
    /* PINGs */
    reply[0] = redisCommand(c,"PING %s", "Hello World");
    printf("RESPONSE: %s\n", reply[0]->str);
    freeReplyObject(reply[0]);
}