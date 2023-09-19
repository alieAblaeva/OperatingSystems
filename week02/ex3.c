#include <stdio.h>
#include <math.h>

void convert(long long int number, int source, int target){
	int decimal = 0;
	int n_digit = 0;
	int digit;
	while(number>0){
		digit = number%10;
		if(digit>=source){
			printf("cannot convert!");
			return;
		}
		decimal += digit * pow(source, n_digit);
		n_digit++;
		number = number/10;
	}
	char reversed[256];
	int size = 0;
	while(decimal>0){
		reversed[size] = decimal%target + '0';
		decimal /= target;
		size++;
	}
	char final[size+1];
	for(int i = size-1; i>=0; i--){
		final[size -i-1] = reversed[i];
	}
	final[size] = '\0';
	printf("%s", final);
}

int main(){
	long long int number;
	int source, target;
	scanf("%lld %d %d", &number, &source, &target);
	if(source<2 || source > 10 || target<2 ||target>10){
		printf("cannot convert");
		return 0;
	}
	else
		convert(number, source, target);
	return 0;
}
