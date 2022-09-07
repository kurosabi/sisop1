#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
// CONSTANTS
#define MAX_LINE 80 // The maximum length command

// Array which will save the last 10 commands
char historial[10][MAX_LINE];
int argCount = 0;

// Agraga los comandos ingresados al arreglo historial que es manejado como una pila
void add_into_arr(char *string) {
	
	for (int i = 9; i > 0; --i)
    	strcpy(historial[i], historial[i - 1]);
        
	strcpy(historial[0], string);
	++argCount;
}

// Organiza los comandos ingresados separados para facilitar la revision de la correctitud de los comandos escritos
// mete los comando en una lista de strings
bool tokenize(char *string, char **ret) {
	
	int i = 0;
	char *token[41];
	token[0] = strtok(string, " ");
	
	while(token[i] != NULL) {
		
		ret[i] = token[i];
		token[++i] = strtok(NULL, " ");
	}
	
	char arr1[10];
	strcpy(arr1, ret[i - 1]);
	token[i] = strtok(ret[i - 1], "&");
	
	if(strcmp(token[i], arr1) == 0) {
		
		ret[i] = NULL;
		return false;
	}
	
	else {
		
		ret[i - 1] = token[i];
		ret[i] = NULL;
		return true;
	}
}

// Muestra el historial de comandos 
void show_history() {
	
	if(argCount == 0)
		printf("No command has been entered so far.\n");
	
	else if(argCount < 11)
		for(int i = 0; i < argCount; ++i)
			printf("%d\t%s\n", argCount - i, historial[i]);
	
	else
		for(int i = 0; i < 10; ++i)
			printf("%d\t%s\n", argCount - i, historial[i]);
}

// Main Function 
int main(void){
	 

	char *args[MAX_LINE/2 + 1]; // Command line arguments
	int should_run = 1; // Flag to determine when to exit program
	
	while (should_run){

		char cwd[PATH_MAX];
		getcwd(cwd, sizeof(cwd));
		char scanned_input[50];
		char *arg;
		
		printf("\n%s\n",cwd);
		printf("osh>");
		fflush(stdout);
		scanf("%[^\n]", scanned_input); // Command + Arguments input stream
        getchar();
        
		
		bool ultimoAmpersand;

		// Muestra el historial de comando si se escribe el comando history
		if(strcmp(scanned_input, "history") == 0) {
			
			show_history(); 
			continue; 
		}

		// trae el comando ejecutado mas reciente
		if(scanned_input[0] == '!') {
			
			if(argCount == 0) {
				
				printf("No recent arguments to execute\n");
				continue;
			}

            // trae el primer comando mas raciente con !!
			if(scanned_input[1] == '!') { 

				add_into_arr(historial[0]);
				
				char s1[MAX_LINE];
				strcpy(s1,historial[0]);
				ultimoAmpersand = tokenize(s1, args);
				
				char com[MAX_LINE];
				strcpy(com,historial[0]);
				ultimoAmpersand = tokenize(com, args);
			}
			
            // trae n-esimo comando mas reciente con !#
			else {
				
				int n = scanned_input[1] - 48;
				
				if(n >= 11 || n <= 0) {
				printf("Only 10 commands can be displayed\n");
					continue;
				}

				add_into_arr(historial[n]);
				
				char s1[MAX_LINE];
				strcpy(s1,historial[n]);
				ultimoAmpersand = tokenize(s1, args);
				
				char com[MAX_LINE];
				strcpy(com,historial[n]);
				ultimoAmpersand = tokenize(com, args);
			}
		}
		
        // recibe un comando normal
		else {	

			add_into_arr(scanned_input);
			ultimoAmpersand = tokenize(scanned_input, args);
		}

		// Crea un proceso hijo 
		pid_t child_pid;
		child_pid = fork();

		// para salir del programa
		if(strcmp(*(args), "exit") == 0)			
			exit(0);
		
        // para entrar en una ubicacion de una carpeta
		if(strcmp(args[0], "cd") == 0) {
			char *loc_string = args[1];
			int found = chdir(loc_string);
			if(found < 0)
				printf("Location not found.\n");
			
			continue;
		}		
		
        // ejecutan los comandos recibidos en el proceso hijo
		if(child_pid == 0) {
			
			
			printf("\n");
			execvp(args[0], args);
			fprintf (stderr,"\nCommand not recognised.\n");
			exit(0);
		}

		// si no encuentra el ampersand el proceso padre espera al proceso hijo para continuar la ejecucion.
		if(ultimoAmpersand == false)
			wait(NULL);
	}
	return 0;
}