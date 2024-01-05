#define _GNU_SOURCE
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

#define _GNU_SOURCE
#ifndef NARGS
#define NARGS 4
#endif
#define TAM_ARRAY (NARGS + 2)
int const ARGS_MINIMOS = 2;
int const ERROR = -1;

void
liberar_args(char *args[TAM_ARRAY], int tope)
{
	for (int i = 1; i < (tope - 1);
	     i++) {  // rgs[tope] = NULL. Por eso libero hasta tope-1
		free(args[i]);
		args[i] = NULL;
	}
}
int
ejecutar_comando(char *args[TAM_ARRAY], int *tope)
{
	args[NARGS + 1] = NULL;
	(*tope)++;

	int f = fork();
	if (f < 0) {
		printf("Error al crear el proceso hijo");
		exit(ERROR);
	}
	if (f == 0) {
		if (execvp(args[0], args) == ERROR) {
			printf("Error al hacer execvp\n");
			return ERROR;
		}
	}
	if (f > 0) {
		wait(NULL);
	}
	return 0;
}

int
main(int argc, char *argv[])
{
	if (argc < ARGS_MINIMOS) {
		printf("Error al leer la línea\n");
		exit(ERROR);
	}

	size_t tam = 0;
	char *linea = NULL;
	char *args[TAM_ARRAY] = {
		argv[1]
	};  // TAM_ARRAY = 6 porque en la última posición va NULL y en la primera va argv[1] -> ls
	int tope = 1;

	while (getline(&linea, &tam, stdin) != ERROR) {
		linea[strlen(linea) - 1] = '\0';  // Elimino el /n
		args[tope] = linea;
		tope++;
		linea = NULL;

		if (tope == (NARGS + 1)) {
			// Se ejecuta el comando una vez que se leyeron 4 argumentos
			if (ejecutar_comando(args, &tope) == ERROR) {
				exit(ERROR);
			}
			liberar_args(args, tope);
			tope = 1;
		}
	}
	if (tope > 0) {
		// Si quedan argumentos sin ser ejecutados, se ejecutan
		if (ejecutar_comando(args, &tope) == ERROR)
			exit(ERROR);
		liberar_args(args, tope);
	}
	free(linea);
	exit(0);
}
