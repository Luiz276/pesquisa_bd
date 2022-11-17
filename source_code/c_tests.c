#include <stdio.h>
#include <sys/time.h>
/* int main() {
    char str[256] = "";
    snprintf(str, 15, "abcde\n");
    printf("1: %s", str);
    snprintf(str, 15, "\n\0");
    printf("2: %s", str);
    return 0;
} */

int main() {
    struct timeval t1;
    gettimeofday(&t1, NULL);
    printf("%ld, %ld", t1.tv_sec, t1.tv_usec);
    return 0;
}