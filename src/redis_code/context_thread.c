#include <stdio.h>
#include <hiredis/hiredis.h>
#include <sys/time.h>
#include <omp.h>
#include <unistd.h>

// Fazer cada thread ter seu próprio context

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
        #-Fazer cada thread ter seu próprio context
*/

int main (int argc, char *argv[]) {
    /*
    Referência para parâmetros:
    1° - número de threads do programa
    2° - número de chaves a serem usadas
    3° - chance de ocorrer uma requisição GET
    4° - chance de ocorrer o registro da latência da requisição em um .txt
    5° - número de requisições a serem realizadas
    Exemplo: ./context_thread 4 256 50 30 100
    */
    if (argc == 1) {
        printf("Não se esqueça de incluir os argumentos no comando\n");
        return 1;
    }
    char *ptr;
    int n_threads = strtol(argv[1], &ptr, 10);
    redisReply *reply[n_threads];
    redisContext *c[n_threads];
    int n_chaves = strtol(argv[2], &ptr, 10);
    int get_chance = strtol(argv[3], &ptr, 10), write_file_chance = strtol(argv[4], &ptr, 10);
    struct timeval t1, t2, ts, ti;    // ts e ti sao usados para timestamp e tempo inicial
    //double tf;
    char key[5], value[5];
    FILE *fptr_lat, *fptr_tp;   // um arquivo para armazenar latência e outro para armazenar throughput
    int n_reqs = strtol(argv[5], &ptr, 10);
    int reqs_env = 0, reqs_env_antigas = 0;

    gettimeofday(&t1, NULL);
    srand(t1.tv_sec);

    c[0] = redisConnect("127.0.0.1", 6379);
    if (c[0]->err) {
        printf("error: %s\n", c[0]->errstr);
        return 1;
    }
    /* PINGs */
    /*
    reply[0] = redisCommand(c,"PING %s", "Hello World");
    printf("RESPONSE: %s\n", reply[0]->str);
    freeReplyObject(reply[0]);
    */

    // limpando o banco de qualquer chave pré-existente
    reply[0] = redisCommand(c[0],"FLUSHALL");
    freeReplyObject(reply[0]);

    omp_set_num_threads(n_threads+1);   // é adicionado 1 ao número de threads pois uma thread sempre será responsável por medir vazão
    //printf("n threads parallel region = %d\n", omp_get_max_threads());
    //printf("n threads variable = %d\n", n_threads);

    //#pragma omp parallel for private(key,value)
    for (int i=0; i< n_chaves; i++) {
        snprintf(key, n_chaves, "%d", i);
        snprintf(value, 5, "%d", rand()%1000);
        reply[0] = redisCommand(c[0],"SET %s %s",key, value);
        freeReplyObject(reply[0]);
    }

    redisFree(c[0]);

    fptr_lat = fopen("./cont_thread_lat.csv", "w");   // formato: timestamp,latencia,op,info
    fptr_tp = fopen("./cont_thread_tp.csv", "w");     // formato: timestamp,throughput

    #pragma omp parallel firstprivate(ti,ts,reply,c) shared(reqs_env, reqs_env_antigas)
    {
        double tf, tfs;
        gettimeofday(&ti, NULL);    // tempo inicial

        if (omp_get_thread_num() == 0) {
            // código da thread de vazão
            printf("n_reqs = %d\n", n_reqs);
            do {
                //sleep(1);    // sleep de 1 segundo
                //if (rand()%100 < write_file_chance) {
                if (1) {    // o bloco de código sempre será executado, somente para testes
                    //#pragma omp critical
                    gettimeofday(&ts, NULL);
                    tfs = (ts.tv_sec -ti.tv_sec) + ((ts.tv_usec -ti.tv_usec)/1000000.0);
                    fprintf(fptr_tp, "%f,%d\n", tfs, (reqs_env - reqs_env_antigas));
                    printf("reqs_env = %d | reqs_env_antigas = %d\n", reqs_env, reqs_env_antigas);
                }
                reqs_env_antigas = reqs_env;
                sleep(1);    // sleep de 1 segundo
            } while (reqs_env < n_reqs);
        }
        else {
            //redisContext *c;
            c[omp_get_thread_num()] = redisConnect("127.0.0.1", 6379);
            if (c[omp_get_thread_num()]->err) {
                printf("thread %d error: %s\n", omp_get_thread_num(),c[omp_get_thread_num()]->errstr);
            }

            #pragma omp for firstprivate(t1,t2) schedule(dynamic) nowait
            for (int i=0; i<n_reqs; i+=1) {
                if (rand() % 100 > get_chance) {
                    usleep(rand()%200);
                    snprintf(key, 5, "%d", rand()%n_chaves);
                    snprintf(value, 5, "%d", rand()%1000);
                    //#pragma omp critical
                    gettimeofday(&t1, NULL);
                    reply[omp_get_thread_num()] = redisCommand(c[omp_get_thread_num()],"SET %s %s",key, value);
                    freeReplyObject(reply[omp_get_thread_num()]);
                    #pragma omp critical
                    {
                        reqs_env++;
                    }
                    gettimeofday(&t2, NULL);
                    gettimeofday(&ts, NULL);
                    tf = (t2.tv_sec -t1.tv_sec) + ((t2.tv_usec -t1.tv_usec)/1000000.0);
                    tfs = (ts.tv_sec -ti.tv_sec) + ((ts.tv_usec -ti.tv_usec)/1000000.0);
                    if (rand()%100 < write_file_chance) {
                        fprintf(fptr_lat, "%f,%f,SET\n", tfs, tf);
                    }
                } else {
                    usleep(rand()%200);
                    snprintf(key, 5, "%d", rand()%n_chaves);
                    //#pragma omp critical
                    gettimeofday(&t1, NULL);
                    reply[omp_get_thread_num()] = redisCommand(c[omp_get_thread_num()],"GET %s",key);
                    freeReplyObject(reply[omp_get_thread_num()]);
                    #pragma omp critical
                    {
                        reqs_env++;
                    }
                    gettimeofday(&t2, NULL);
                    gettimeofday(&ts, NULL);
                    tf = (t2.tv_sec -t1.tv_sec) + ((t2.tv_usec -t1.tv_usec)/1000000.0);
                    tfs = (ts.tv_sec -ti.tv_sec) + ((ts.tv_usec -ti.tv_usec)/1000000.0);
                    if (rand()%100 < write_file_chance) {
                        if (reply[omp_get_thread_num()]->len > 0) {
                            fprintf(fptr_lat, "%f,%f,GET\n", tfs, tf);
                        } else {
                            fprintf(fptr_lat, "%f,%f,GET,invalid\n", tfs, tf);
                        }
                    }
                }
            }
            redisFree(c[omp_get_thread_num()]);
        }
        printf("Thread %d finalizada\n", omp_get_thread_num());
    }

    fclose(fptr_tp);
    fclose(fptr_lat);

    //redisFree(c);
    return 0;
}
