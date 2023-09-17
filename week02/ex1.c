#include <stdio.h>
#include <limits.h>
#include <float.h>
int main(){
	int a = INT_MAX;
	unsigned short int b = USHRT_MAX;
	signed long int c = LONG_MAX;
	float d = FLT_MAX;
	double e = DBL_MAX;
	printf("int: size = %ld, max value = %d\n", sizeof(a), a);
	printf("unsigned short int: size = %ld, max value = %d\n", sizeof(b), b);
	printf("signed long int: size = %ld, max value = %ld\n", sizeof(c), c);
	printf("float: size = %ld, max value = %f\n", sizeof(d), d);
	printf("double: size = %ld, max value = %f\n", sizeof(e), e);

	return 0;
}
