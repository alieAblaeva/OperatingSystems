#include <stdio.h>
#include <string.h>
int main(){
	char string[256];
	char symbol;
	int counter=0;
	while((symbol = getc(stdin))!='.')
	{
		string[counter] = symbol;
		counter++;
	}
	int size = strlen(string);
	char reversed[size+1];
	for(int i = size-1; i>=0; i--){
		reversed[size-i-1] = string[i];
	}
	reversed[size] = '\0';
	printf("%s\n", reversed);
	return 0;
}
