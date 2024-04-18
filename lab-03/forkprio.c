#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>     
#include <unistd.h>
#include <signal.h>
#include <sys/resource.h>


int busywork(void)
{
    struct tms buf;
    for (;;) {
        times(&buf);
    }
}

void sigHandlerHijo()
{
    struct rusage usage;
    getrusage(RUSAGE_SELF,&usage);
    printf("Child %d (nice %2d):\t%3li\n", getpid(), getpriority(PRIO_PROCESS,(id_t)getpid()), usage.ru_stime.tv_sec + usage.ru_utime.tv_sec);
    exit(EXIT_SUCCESS);
}

void sigHandlerPadre()
{
}

int main(int argc, char *argv[])
{
    if (argc != 4){
        perror("Error! Debe ingresar 3 parametros: <numeroHijos>,<cantSegundos> y <prioridad>");
        exit(EXIT_FAILURE);
    }
    int cantSegundos = atoi(argv[2]);
    int numeroHijos = atoi(argv[1]);
    int prioridad = atoi(argv[3]);
    int pids[numeroHijos-1];
    int i = 0;
    int pid = 0;

    struct sigaction saH;
    saH.sa_handler = sigHandlerHijo;
    saH.sa_flags = 0;
    sigemptyset(&saH.sa_mask);

    struct sigaction saP;
    saP.sa_handler = sigHandlerPadre;
    saP.sa_flags = 0;
    sigemptyset(&saP.sa_mask);

    for (i = 0; i < numeroHijos; i++){
        pid = fork();
        pids[i] = pid;
        if (pid == 0){
            if (prioridad != 0){
                setpriority(PRIO_PROCESS,0,i+1);
            }
            if (sigaction(SIGTERM, &saH, NULL) == -1) {
                perror("Error al configurar el manejador de señales para el proceso hijo");
                exit(EXIT_FAILURE);
            }
            if (sigaction(SIGINT, &saH, NULL) == -1) {
                perror("Error al configurar el manejador de señales para el proceso hijo");
                exit(EXIT_FAILURE);
            }
            busywork();
        } else {
            if (pid > 0){
                if (sigaction(SIGINT, &saP, NULL) == -1) {
                    perror("Error al configurar el manejador de señales para el proceso padre");
                    exit(EXIT_FAILURE);
                }
            } else {
                perror("Error al crear un proceso");
                exit(EXIT_FAILURE);
            }
            
        }
    }

    if(cantSegundos > 0){
        sleep((unsigned int)cantSegundos);
    } else {
        pause();
    }
    for (i = 0; i < numeroHijos;i++){
        kill(pids[i],SIGTERM);
    }

    exit(EXIT_SUCCESS);
}