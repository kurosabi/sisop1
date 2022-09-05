#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LINE 80 /* The maximum length command */

int main(void){

	char *args; /* command line arguments */
	int should_run = 1; /* flag to determine when to exit program */
	int chars;
	int size = MAX_LINE;
	


	while (should_run){

		args = (char*) malloc(size);

		printf("osh>");

		//chars = getline(&args, &size, stdin);
		
		fflush(stdin);
        scanf("%s", &args);

		printf("%s\n", &args);
		

		/**
		* After reading user input, the steps are:
		* (1) fork a child process using fork()
		* (2) the child process will invoke execvp()
		* (3) parent will invoke wait() unless command included &
		*/

		int pid = fork();
		int returnStatus;

		if (pid == 0){
            // hace algo

            if(strcmp(&args, "exit") == 0){
				should_run = 0;
			}

			exit(0);

        }
        waitpid(pid, &returnStatus, 0);				
	}

	return 0;

}