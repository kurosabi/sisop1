#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>

#define MAX_LINE 80 /* The maximum length command */


char historial[10][MAX_LINE];
int argCount = 0;

// Agraga los comandos ingresados al arreglo historial que es manejado como una pila
void add_historial(char *string){
	
	for (int i = 9; i > 0; --i){

    	strcpy(historial[i], historial[i - 1]);
    }  

	strcpy(historial[0], string);
	++argCount;
}

// Organiza los comandos ingresados separados para facilitar la revision de la correctitud de los comandos escritos
// mete los comando en una lista de strings
bool purgarInput(char *string, char **aux){
	
	int i = 0;
	char *comando[41];
	comando[0] = strtok(string, " ");
	
	while(comando[i] != NULL){
		
		aux[i] = comando[i];
		comando[++i] = strtok(NULL, " ");
	}
	
	char arr1[10];
	strcpy(arr1, aux[i - 1]);
	comando[i] = strtok(aux[i - 1], "&");
	
	if(strcmp(comando[i], arr1) == 0){
		
		aux[i] = NULL;
		return false;
	}
	
	else{
		
		aux[i - 1] = comando[i];
		aux[i] = NULL;
		return true;
	}
}

// Muestra el historial de comandos 
void mostrar_historial(){
	
	if(argCount == 0){

		printf("No command has been entered so far.\n");
    }
	
	else if(argCount < 11){

		for(int i = 0; i < argCount; ++i){

			printf("%d\t%s\n", argCount - i, historial[i]);
        }
    }

	else{

		for(int i = 0; i < 10; ++i){
            
			printf("%d\t%s\n", argCount - i, historial[i]);
        }
    }
}



int main(void){
	 
	char *args[MAX_LINE/2 + 1]; /* Command line arguments */
	int should_run = 1; /* Flag to determine when to exit program */
	
	while (should_run){

		char cwd[PATH_MAX];
		getcwd(cwd, sizeof(cwd));
		char entrada[50];
		char *arg;
		
		printf("\n%s\n",cwd);
		printf("osh>");
		fflush(stdout);
		scanf("%[^\n]", entrada);
        getchar();
        
		
		bool ultimoAmpersand;

		// Muestra el historial de comando si se escribe el comando history
		if(strcmp(entrada, "historial") == 0){
			
			mostrar_historial(); 
			continue; 
		}

		// trae el comando ejecutado mas reciente
		if(entrada[0] == '!'){
			
			if(argCount == 0){
				
				printf("No recent arguments to execute\n");
				continue;
			}

            // trae el primer comando mas raciente con !!
			if(entrada[1] == '!'){ 

				add_historial(historial[0]);
				char temp[MAX_LINE];
				strcpy(temp,historial[0]);
				ultimoAmpersand = purgarInput(temp, args);
				char temp2[MAX_LINE];
				strcpy(temp2,historial[0]);
				ultimoAmpersand = purgarInput(temp2, args);
			}
			
            // trae n-esimo comando mas reciente con !#
			else{
				
				int n = entrada[1] - 48;
				
				if(n >= 11 || n <= 0){

				    printf("Only 10 commands can be displayed\n");
					continue;
				}

				add_historial(historial[n]);
				char temp[MAX_LINE];
				strcpy(temp,historial[n]);
				ultimoAmpersand = purgarInput(temp, args);
				char temp2[MAX_LINE];
				strcpy(temp2,historial[n]);
				ultimoAmpersand = purgarInput(temp2, args);
			}
		}
		
        // recibe un comando normal
		else{	

			add_historial(entrada);
			ultimoAmpersand = purgarInput(entrada, args);
		}

		// Crea un proceso hijo 
		pid_t childPid;
		childPid = fork();

		// para salir del programa
		if(strcmp(*(args), "exit") == 0){

			exit(0);
        }

        // para entrar en una ubicacion de una carpeta
		if(strcmp(args[0], "cd") == 0){

			char *loc_string = args[1];
			int found = chdir(loc_string);

			if(found < 0){

				printf("Location not found.\n");
            }

			continue;
		}		
		
        // ejecutan los comandos recibidos en el proceso hijo
		if(childPid == 0){
						
			printf("\n");
			execvp(args[0], args);
			fprintf (stderr,"\nCommand not recognised.\n");
			abort();
		}

		// si no encuentra el ampersand el proceso padre espera al proceso hijo para continuar la ejecucion.
		if(ultimoAmpersand == false){
			wait(NULL);
        }
	}
    
	return 0;
}