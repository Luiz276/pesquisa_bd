#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hiredis/hiredis.h>
#include <time.h>

int main (int argc, char **argv) {
redisReply *reply;
redisContext *c;

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

time_t start = time(NULL);

reply = redisCommand(c,"SET %s %s","foo","bar");
freeReplyObject(reply);

printf("Time taken by SET command: %.5f\n", time(NULL)-start);

start = time(NULL);

reply = redisCommand(c,"GET %s","foo");
printf("%s\n",reply->str);
freeReplyObject(reply);

printf("Time taken by GET command: %.5f\n", time(NULL)-start);

redisFree(c);
}