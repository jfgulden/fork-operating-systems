#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
//#include <sys/types.h>

int main(void)
{
    int fds[2];
    int r = pipe(fds);
    if (r < 0){
        printf("error\n");
        exit(-1);
    }
    srandom(time(NULL));
    int msg = random();
    int recibido1, recibido2 = 0;
    int pipe1_lectura, pipe1_escritura = 0;
    int w = 0;



    //fds[0] -> Lectura
    //fds[1] -> Escritura

    printf("Hola, soy PID <%d>:\n", getpid());
    printf("  - primer pipe me devuelve: [%d, %d]\n", fds[0], fds[1]);
    pipe1_lectura = fds[0];
    pipe1_escritura = fds[1];
    r = pipe(fds);
    if (r < 0){
        printf("error\n");
        exit(-1);
    }

    printf("  - segundo pipe me devuelve: [%d, %d]\n\n", fds[0], fds[1]);

    int i = fork();
    if (i < 0){
        printf("error\n");
        exit(-1);
    }
    //Para cada proceso cierro los extremos de los pipes que no voy a usar.
    /*
    También podría cerrar todo al final antes de retornar del main,
    pero prefiero seguir esta regla y asegurarme de que cada proceso cierra sus extremos del pipe.
    */
    if (i > 0){
        close(pipe1_lectura);
        close(fds[1]);
        w = write(pipe1_escritura, &msg, sizeof(msg));
        if (w < 0){
            printf("error en write\n");
            exit(-1);
        }
        close(pipe1_escritura);
        printf("Donde fork me devuelve: <%i>\n", i);
        printf("  - getpid me devuelve: <%d>\n", getpid());
        printf("  - getppid me devuelve: <%d>\n", getppid());
        printf("  - random me devuelve: <%i>\n", msg);
        printf("  - envío valor <%i> a través de fd=%d\n\n", msg, pipe1_escritura);
    }
    
    if (i == 0){
        close(pipe1_escritura);
        close(fds[0]);
        r = read(pipe1_lectura, &recibido1, sizeof(recibido1));
        close(pipe1_lectura);
        if (r < 0){
            printf("error en read\n");
            exit(-1);
        }     
        printf("Donde fork me devuelve: <%i>\n", i);
        printf("  - getpid me devuelve: <%d>\n", getpid());
        printf("  - getppid me devuelve: <%d>\n", getppid());
        printf("  - recibo valor <%i> vía fd=%d\n", recibido1, pipe1_lectura);
        w = write(fds[1], &recibido1, sizeof(recibido1));
        if (w < 0){
            printf("error en write\n");
            exit(-1);
        }
        close(fds[1]);
        printf("  - reenvío valor en fd=%d y termino\n\n", fds[1]);
    }
    
    if (i > 0){
        r = read(fds[0], &recibido2, sizeof(recibido2));
        if (r < 0){
            printf("error en read\n");
            exit(-1);
        } 
        close(fds[0]);
        printf("Hola, de nuevo PID <%i>:\n", getpid());
        printf("  - recibí valor <%i> vía fd=%d\n", recibido2, fds[0]);
    }
    return 0;
}

