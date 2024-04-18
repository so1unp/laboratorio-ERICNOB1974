#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char *argv[])
{

    const int NUM_ESPERADO_ARGC = 3;

    if (argc != NUM_ESPERADO_ARGC){
        perror("Formato invalido. Debe ingresar: ./sig_sender 'Numero de senal' 'PID'");
        exit(EXIT_FAILURE);
    }

    kill(atoi(argv[2]),atoi(argv[1]));

    exit(EXIT_SUCCESS);

}