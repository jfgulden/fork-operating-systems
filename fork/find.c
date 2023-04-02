#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define _GNU_SOURCE
#define ARGS_MINIMOS 2
#define DIR_ACTUAL "."
#define DIR_SUPERIOR ".."
#define CASE_FLAG "-i"
#define ERROR -1

char *strcasestr(const char *haystack, const char *needle);

bool
es_directorio_actual_especial(char *nombre)
{
	return (strcmp(nombre, DIR_ACTUAL) == 0 ||
	        strcmp(nombre, DIR_SUPERIOR) == 0);
}

int
buscar_archivo(DIR *dir,
               char path_actual[],
               char archivo_buscado[],
               char *(*comparar_substring)(const char *, const char *) )
{
	struct dirent *acceso_dir = readdir(dir);
	int fd_acceso_dir = dirfd(dir);
	if (fd_acceso_dir == ERROR) {
		printf("Error al intentar obtener el file descriptor del "
		       "directorio\n");
		return ERROR;
	}


	while (acceso_dir != NULL) {
		if (es_directorio_actual_especial(acceso_dir->d_name)) {
			acceso_dir = readdir(dir);
			continue;
		}
		if (comparar_substring(acceso_dir->d_name, archivo_buscado) !=
		    NULL) {
			printf("%s%s\n", path_actual, acceso_dir->d_name);
		}
		if (acceso_dir->d_type == DT_DIR) {
			char subdir_path[PATH_MAX] = "\0";
			strcpy(subdir_path, path_actual);
			strcat(subdir_path, acceso_dir->d_name);
			strcat(subdir_path, "/");

			int fd_acceso_subdir = openat(fd_acceso_dir,
			                              acceso_dir->d_name,
			                              O_DIRECTORY);
			if (fd_acceso_dir == ERROR) {
				printf("Error al intentar abrir el "
				       "subdirectorio\n");
				return ERROR;
			}
			DIR *subdir = fdopendir(fd_acceso_subdir);
			if (!subdir) {
				printf("Error al intentar obtener acceso al "
				       "subdirectorio\n");
				return ERROR;
			}
			int resultado_busqueda =
			        buscar_archivo(subdir,
			                       subdir_path,
			                       archivo_buscado,
			                       comparar_substring);
			if (resultado_busqueda == ERROR)
				return ERROR;

			closedir(subdir);
		}
		acceso_dir = readdir(dir);
	}
	return 0;
}
int
main(int argc, char *argv[])
{
	if (argc < ARGS_MINIMOS) {
		printf("Error al leer la línea\n");
		exit(-1);
	}
	char path[PATH_MAX] = "\0";
	char *archivo_buscado = NULL;
	char *(*comparar_substring)(const char *, const char *) = NULL;
	DIR *dir = opendir(DIR_ACTUAL);
	int resultado_busqueda = 0;

	if (!dir) {
		printf("Error al abrir el directorio\n");
		exit(-1);
	}

	if (argc == 3 && strcmp(argv[1], CASE_FLAG) == 0) {
		archivo_buscado = argv[2];
		comparar_substring = strcasestr;
	}
	if (argc == 2) {
		archivo_buscado = argv[1];
		comparar_substring = strstr;
	}
	resultado_busqueda =
	        buscar_archivo(dir, path, archivo_buscado, comparar_substring);

	closedir(dir);
	if (resultado_busqueda == ERROR) {
		printf("Error al intentar buscar el archivo\n");
		return ERROR;
	}

	return 0;
}
