#include <stdio.h>
#include <string.h>

void count(char string[], char letter){
	int counter = 0;
	for (int i = 0; i<strlen(string); i++){
		if (string[i] == letter || string[i]+32 == letter || string[i]-32==letter)
			counter++;
	}
	printf("%c:%d", letter, counter);
}

void countAll(char string[]){
	for(int i = 0; i<strlen(string); i++){
		count(string, string[i]);
		if(i!= strlen(string)-1)
			printf(", ");
	}
}

int main(){
	char string[256];
	scanf("%s", string);
	countAll(string);
	return 0;
}
