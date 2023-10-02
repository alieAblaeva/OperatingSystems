#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>

void* aggregate(void* base, size_t size, int n, void* initial_value, void* (*opr)(const void*, const void*)){
    void* result = initial_value;
    for(int i = 0; i<n; i++){
        result = opr(result, base + i*size);
    }
    return result;
}

void* addition_int(const void* a, const void* b){
    int* result = malloc(sizeof(int));
    *result = *(int*)a + *(int*)b;
    return result;
}

void* addition_double(const void* a, const void* b){
    double* result = malloc(sizeof(double));
    *result = *(double*)a+*(double*)b;
    return result;
}

void* multiplication_int(const void* a, const void* b){
    int* result = malloc(sizeof(int));
    *result = *(int*)a * *(int*)b;
    return result;
}

void* multiplication_double(const void* a, const void* b){
    double* result = malloc(sizeof(double));
    *result = *(double*)a * *(double*)b;
    return result;
}

void* max_int(const void* a, const void* b){
    if(*(int*)a > *(int*)b)
        return (int*)a;
    else
        return (int*)b;
}

void* max_double(const void* a, const void* b){
    if(*(double*)a > *(double*)b)
        return (int*)a;
    else
        return (int*)b;
}

int main(){
    int base_sum = 0;
    int base_mult = 1;
    double base_multd = 1;
    int base_max_int = INT_MIN;
    double base_max_double = DBL_MIN;
    int integers[] = {1, 2, 3, 4, 5};
    double doubles[] = {1.5, 2.5, 3.5, 4.5, 5.5};
    int sumi = *(int*)aggregate(integers, sizeof(int), 5, &base_sum, &addition_int);
    double sumd = *(double*) aggregate(doubles, sizeof(double), 5, &base_sum, &addition_double);
    int multi = *(int*)aggregate(integers, sizeof(int), 5, &base_mult, &multiplication_int);
    double multd = *(double*) aggregate(doubles, sizeof(double), 5, &base_multd, &multiplication_double);
    int maxi = *(int*)aggregate(integers, sizeof(int), 5, &base_max_int, &max_int);
    double maxd = *(double*)aggregate(doubles, sizeof(double), 5, &base_max_double, &max_double);

    printf("array of integers:\n");
    for(int i = 0; i<5; i++){
        printf("%d ", integers[i]);
    }
    printf("\narray of doubles:\n");
    for(int i = 0; i<5; i++){
        printf("%f ", doubles[i]);
    }

    printf("\nsum of integers: %d\n", sumi);
    printf("sum of doubles: %f\n", sumd);
    printf("multiplication of integers: %d\n", multi);
    printf("multiplication of doubles: %f\n", multd);
    printf("max of integers: %d\n", maxi);
    printf("max of doubles: %f\n", maxd);
    return 0;
}
