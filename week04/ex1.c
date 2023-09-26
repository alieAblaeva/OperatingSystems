#include <stdio.h>
#include <stdlib.h>

int const_tri(int* p, int n){
	if(n==2 || n==1)
		return 1;
	if(n==0)
		return 0;
	*p = 0;
	*(p+2) = 1;
	n -= 2;
	int temp;
	for (;n;n--){
		temp = *p + *(p+1) + *(p+2);
		*p = *(p+1);
		*(p+1) = *(p+2);
		*(p+2) = temp;
	}
	return temp;
}

int main(){
	const int x = 1;
	const int* q = &x;
	int* const p = (int*)malloc(3*sizeof(int));
	*p = x;
	*(p+1) = x;
	*(p+2) = 2*x;
	printf("%p  %p  %p\n", p, p+1, p+2);
	printf("%d", const_tri(p, 6));
	free(p);
	return 0;
}
