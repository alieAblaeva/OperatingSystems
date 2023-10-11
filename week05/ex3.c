#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

bool is_prime(int n) {
    if (n <= 1) return false;
    int i = 2;
    for (; i * i <= n; i++) {
        if (n % i == 0)
            return false;
    }
    return true;
}

typedef struct prime_request {
    int a, b;
} prime_request;

void *prime_counter(void *arg) {
    prime_request *req = (prime_request *)arg;
    int count = 0;
    
    for (int i = req->a; i < req->b; i++) {
        if (is_prime(i))
            count++;
    }
    
    int *result = (int *)malloc(sizeof(int));
    *result = count;
    return (void *)result;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s n m\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);

    pthread_t threads[m];
    prime_request requests[m];
    int thread_results[m];
    
    int interval_size = n / m;
    int remainder = n % m;
    int start = 0;
    int end = interval_size;

    for (int i = 0; i < m; i++) {
        if (i == m - 1) {
            end += remainder; 
        }
        requests[i].a = start;
        requests[i].b = end;
        start = end;
        end += interval_size;
        pthread_create(&threads[i], NULL, prime_counter, (void *)&requests[i]);
    }

    int total_primes = 0;
    for (int i = 0; i < m; i++) {
        void *result;
        pthread_join(threads[i], &result);
        total_primes += *((int *)result);
        free(result);
    }

    printf("Total primes in [0, %d) = %d\n", n, total_primes);

    return 0;
}

