#include <stdio.h>
#include <omp.h>
/* int main() {
    char str[256] = "";
    snprintf(str, 15, "abcde\n");
    printf("1: %s", str);
    snprintf(str, 15, "\n\0");
    printf("2: %s", str);
    return 0;
} */

int main() {
    #pragma omp parallel num_threads(2)
    {
        printf("outer thread %d\n", omp_get_thread_num());

        #pragma omp parallel for num_threads(10)
        for (int i=0; i<10; i++) {
            printf("inner thread %d\n", omp_get_thread_num());
        }
        printf("outer thread %d\n", omp_get_thread_num());
    }
    return 0;
}