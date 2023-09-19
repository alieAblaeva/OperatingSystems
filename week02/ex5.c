#include <stdio.h>

int tribonacci(int n){
	int tn = 0;
	int tn_1 = 1;
	int tn_2 = 1;
	int tn_3 = 0;
	for(int i = 0; i<n-2; i++){
		tn = tn_1 + tn_2 +tn_3;
		tn_3 = tn_2;
		tn_2 = tn_1;
		tn_1 = tn;
	}
	return tn;
}

int main(){
	printf("%d\n%d\n", tribonacci(4), tribonacci(36));
	return 0;
}
