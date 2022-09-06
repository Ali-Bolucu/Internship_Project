
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


int main() {

	int n;
	char s_string[100];
	bool state;

	
    state = adalm_pluto();

	if (state == false) {
		printf("\n \n 1 \n \n");
		shutdown();
		return 0;
	}

	printf("\n\n");


   TCP_client_pluto();


	printf("Type 'exit' for shutdown pluto : ");
	n = 0;

	while ((s_string[n++] = getchar()) != '\n')
		;

	if ((strncmp(s_string, "exit", 4)) == 0) {
		printf("pluto shutting down...\n");
		shutdown();
	}



    return 0;
}
