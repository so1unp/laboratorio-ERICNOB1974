#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int tiempoMaxParaDormir;

void *thread_function(void *p) {
    long id = (long)p;
    int tiempoADormir = rand() % (tiempoMaxParaDormir + 1);
    printf("Hilo %ld: dormiré %d\n", id, tiempoADormir);
    sleep(tiempoADormir);
    pthread_exit((void*) (intptr_t)tiempoADormir);
}

int main(int argc, char* argv[])
{

    if (argc != 3) {
        fprintf(stderr, "Error. Uso correcto: %s <número de hilos a crear> <tiempo máximo para dormir>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);

    tiempoMaxParaDormir = atoi(argv[2]);

    long i;

    int retorno;

    pthread_t hilos[n];

    srand(123);

    for (i = 0; i < n; i++){
        retorno = pthread_create(&hilos[i], NULL, thread_function, (void *)i);
        if (retorno != 0) {
            fprintf(stderr, "Error al crear el hilo\n");
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < n; i++) {
        void *retorno_hilo;
        pthread_join(hilos[i], &retorno_hilo);
        printf("Hilo %ld terminó: %ld segundos\n", i, ((long) retorno_hilo));
    }

    exit(EXIT_SUCCESS);

}