#include <stdio.h>
#include <stdlib.h>

int main(){
	char *user = getenv("USER"); //couldnt use $USER, used getenv
	printf("Hello, %s\n", user);
	return 0;
}

