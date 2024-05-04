#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Variable global que incrementan los hilos.
static long glob = 0;

void *increment_glob(void* p)
{
    long loc, j;

    long loops = (long)p;
    
    // incrementa glob
    for (j = 0; j < loops; j++) {
        loc = glob;
        loc++;
        glob = loc;
    }

    pthread_exit(NULL);

}

int main(int argc, char *argv[])
{
    long loops;
    int cantidadHilos = 2;

    // Controla numero de argumentos.
    if (argc != 2) {
        fprintf(stderr, "Uso: %s ciclos\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    loops = atoi(argv[1]);

    // Verifica argumentos.
    if (loops < 1) {
        fprintf(stderr, "Error: ciclos debe ser mayor a cero.\n");
        exit(EXIT_FAILURE);
    }

    int retorno;
    pthread_t hilos[cantidadHilos];
    long i;

    for (i = 0; i< cantidadHilos;i++){
        retorno = pthread_create(&hilos[i], NULL, increment_glob, (void *) loops);
        if (retorno != 0) {
            fprintf(stderr, "Error al crear el hilo\n");
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < cantidadHilos;i++){
        retorno = pthread_join(hilos[i], NULL);
        if (retorno != 0) {
            fprintf(stderr, "Error al esperar al hilo \n");
            exit(EXIT_FAILURE);
        }
    }

    printf("%ld\n", glob);

    exit(EXIT_SUCCESS);
}
