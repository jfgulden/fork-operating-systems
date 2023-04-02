#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>


#define ERROR -1
#define ARGS_MIN 2

void mostrar_error_pipe(){
    printf("Error al crear el pipe\n");
}
void mostrar_error_fork(){
    printf("Error al crear el proceso hijo\n");
}
void mostrar_error_write(){
    printf("Error al escribir en el pipe\n");
}
void mostrar_error_read(){
    printf("Error al lerr en el pipe\n");
}


int buscar_primos(int p_lectura){
    int w, r = 0;
    int primo, valor_a_enviar = 0;
    r = read(p_lectura, &primo, sizeof(primo));
    if (r < 0){
        mostrar_error_read();
        exit(ERROR);
    }
    if (r == 0){  
        //Se llegó al EOF  
        close(p_lectura);
        return 0;
    }
    if (r > 0){
        //Entra si pudo leer
        printf("primo %i\n", primo);
        int fds[2];
        int p = pipe(fds);
        if (p < 0){
            mostrar_error_pipe();
            exit(ERROR);
        }
        int f = fork();
        if (f < 0){
            mostrar_error_fork();
            exit(ERROR);
        }
        if (f == 0){
            close(p_lectura);
            close(fds[1]);
            int resultado_busqueda = buscar_primos(fds[0]);
            if (resultado_busqueda == ERROR){
                return ERROR;
            }
            close(fds[0]);
        }
        if (f > 0){
            close(fds[0]);
            while ((r = read(p_lectura, &valor_a_enviar, sizeof(valor_a_enviar))) != 0){
                if (valor_a_enviar % primo != 0){
                    //Escribe sobre el extremo del nuevo pipe
                    w = write(fds[1], &valor_a_enviar, sizeof(valor_a_enviar));
                    if (w < 0){
                        mostrar_error_write();
                        exit(ERROR);
                    }

                }
            }
            if (r < 0){
                mostrar_error_read();
                exit(ERROR);
            }
            //Cierra todos los fds que le quedaron abiertos una vez que sale del while
            close(fds[1]);
            close(p_lectura);
            //Espera a que el proceso hijo termine
            wait(NULL);        
        }
        
    }
    return 0;
    
}
int main(int argc, char *argv[]){
    int fds[2];
    int w = 0;
    int resultado_busqueda = 0;
    if (argc != ARGS_MIN) {
        printf("Debe proporcionar un número entero en la línea de ejecución.\n");
        exit (ERROR);
    }
    int numero = atoi(argv[1]);
    int p = pipe(fds);
    if (p < 0){
        mostrar_error_pipe();
        exit(ERROR);
    }
    int f = fork();
    if (f < 0){
        mostrar_error_fork();
        exit(ERROR);
    }
    if (f > 0){
        //Cierro lo que no voy a usar
        close(fds[0]);
        for (int i = 2; i <= numero; i++){
            w = write(fds[1], &i, sizeof(i));
            if (w < 0){
                mostrar_error_write();
                exit(ERROR);
            }
        }
        close(fds[1]);
        wait(NULL);
        return 0;
    }
    if (f == 0){
        //Cierro lo que no voy a usar
        close(fds[1]);
        resultado_busqueda = buscar_primos(fds[0]);
        if (resultado_busqueda == ERROR){
            printf("Error al intentar buscar los números primos de %i\n", numero);
            exit(ERROR);
        }
    }



    return 0;
}