#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>   
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>

#define ITEMS       15
#define MAX_WORD    50

struct wordstack {
    int free;
    int items;
    int max_word;
    pthread_mutex_t mutex;
    sem_t full;
    sem_t empty;
    char heap[ITEMS][MAX_WORD];
};

typedef struct wordstack wordstack_t;

void usage(char *argv[])
{
    fprintf(stderr, "Uso: %s comando parametro\n", argv[0]);
    fprintf(stderr, "Comandos:\n");
    fprintf(stderr, "\t-w pila palabra: agrega palabra en la pila\n");
    fprintf(stderr, "\t-r pila pos: elimina la palabra de la pila.\n");
    fprintf(stderr, "\t-p pila: imprime la pila de palabras.\n");
    fprintf(stderr, "\t-c pila: crea una zona de memoria compartida con el nombre indicado donde almacena la pila.\n");
    fprintf(stderr, "\t-d pila: elimina la pila indicada.\n");
    fprintf(stderr, "\t-h imprime este mensaje.\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        usage(argv);
        exit(EXIT_FAILURE);
    }

    if (argv[1][0] != '-') {
        usage(argv);
        exit(EXIT_FAILURE);
    }

    char option = argv[1][1];

    int descriptor,i = 0;

    wordstack_t* miMapa;

    char word[MAX_WORD];

    switch(option) {
        case 'w':
            if (argc < 4) {
                fprintf(stderr, "Falta especificar la palabra a agregar a la pila.\n");
                usage(argv);
                exit(EXIT_FAILURE);
            }
            strcpy(word, argv[3]);
            descriptor = shm_open(argv[2], O_RDWR, 0666);
            if (descriptor == -1) {
                perror("Error al abrir la memoria compartida");
                exit(EXIT_FAILURE);
            }
            miMapa = (wordstack_t *)mmap(NULL, sizeof(wordstack_t), PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);
            if (miMapa == MAP_FAILED) {
                perror("Error en syscall mmap");
                close(descriptor);
                exit(EXIT_FAILURE);
            }
            sem_wait(&miMapa->empty);
                pthread_mutex_lock(&miMapa->mutex);
                strcpy(miMapa->heap[miMapa->items], word);
                miMapa->items++;
                pthread_mutex_unlock(&miMapa->mutex);
            sem_post(&miMapa->full);
            printf("Palabra '%s' agregada a la pila.\n", word);
            close(descriptor);
            break;
        case 'r':
            descriptor = shm_open(argv[2], O_RDWR, 0666);
            if (descriptor == -1) {
                perror("Error al abrir la memoria compartida");
                exit(EXIT_FAILURE);
            }
            miMapa = (wordstack_t *)mmap(NULL, sizeof(wordstack_t), PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);
            if (miMapa == MAP_FAILED) {
                perror("Error en syscall mmap");
                close(descriptor);
                exit(EXIT_FAILURE);
            }
            sem_wait(&miMapa->full);
                pthread_mutex_lock(&miMapa->mutex);
                strcpy(word, miMapa->heap[miMapa->items - 1]);
                miMapa->items--;
                pthread_mutex_unlock(&miMapa->mutex);
            sem_post(&miMapa->empty);
            printf("Palabra recuperada y eliminada de la pila: %s\n", word);
            close(descriptor);
            break;
        case 'p':
            descriptor = shm_open(argv[2], O_RDWR, 0666);
            if (descriptor == -1) {
                perror("Error al abrir la memoria compartida");
                exit(EXIT_FAILURE);
            }
            miMapa = (wordstack_t *)mmap(NULL, sizeof(wordstack_t), PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);
            if (miMapa == MAP_FAILED) {
                perror("Error en syscall mmap");
                close(descriptor);
                exit(EXIT_FAILURE);
            }
            printf("Pila compartida:\n");
            pthread_mutex_lock(&miMapa->mutex);
            for (i = miMapa->items; i >= 0 ; i--) {
                printf("%s\n", miMapa->heap[i]);
            }
            pthread_mutex_unlock(&miMapa->mutex);
            close(descriptor);
            break;
        case 'c':
            descriptor = shm_open(argv[2], O_CREAT | O_RDWR, 0666);
            if (descriptor == -1) {
                perror("Error al abrir la memoria compartida");
                exit(EXIT_FAILURE);
            }
            ftruncate(descriptor, sizeof(wordstack_t));
            miMapa = (wordstack_t*) mmap(NULL, sizeof(wordstack_t), PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);
            if (miMapa == MAP_FAILED) {
                perror("Error en syscall mmap");
                close(descriptor);
                exit(EXIT_FAILURE);
            }
            if (pthread_mutex_init(&miMapa->mutex, NULL) != 0) {
                perror("Error al inicializar el mutex");
                close(descriptor);
                exit(EXIT_FAILURE);
            }
            if (sem_init(&miMapa->full,1, 0) != 0) {
                perror("Error al inicializar el semáforo full");
                close(descriptor);
                exit(EXIT_FAILURE);
            }
            if (sem_init(&miMapa->empty, 1, ITEMS) != 0) {
                perror("Error al inicializar el semáforo empty");
                close(descriptor);
                exit(EXIT_FAILURE);
            }
            printf("Crea la nueva pila compartida %s\n", argv[2]);
            close(descriptor);
            break;
        case 'd':
            descriptor = shm_open(argv[2], O_RDWR, 0666);
            if (descriptor == -1) {
                perror("Error al abrir la memoria compartida");
                exit(EXIT_FAILURE);
            }
            miMapa = (wordstack_t*) mmap(NULL, sizeof(wordstack_t), PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);
            if (miMapa == MAP_FAILED) {
                perror("Error en syscall mmap");
                close(descriptor);
                exit(EXIT_FAILURE);
            }
            pthread_mutex_destroy(&miMapa->mutex);
            sem_destroy(&miMapa->full);
            sem_destroy(&miMapa->empty);
            shm_unlink(argv[2]);
            printf("Borra pila compartida %s.\n", argv[2]);
            close(descriptor);
            break;
        case 'h':
            usage(argv);
            break;
        default:
            fprintf(stderr, "-%c: opción desconocida.\n", option);
            exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
}
