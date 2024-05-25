#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>   
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

#define HEIGHT  25  // Altura en caracteres de la pizarra
#define WIDTH   25  // Ancho en caracteres de la pizarra

struct canvas {
    char canvas[HEIGHT*WIDTH];
};

typedef struct canvas canvas_t;

void usage(char *argv[])
{
    fprintf(stderr, "Uso: %s comando parametro\n", argv[0]);
    fprintf(stderr, "Comandos:\n");
    fprintf(stderr, "\t-w canvas mensaje x y [h|v]: escribe el mensaje en el canvas indicado en la posición (x,y) de manera [h]orizontal o [v]ertical.\n");
    fprintf(stderr, "\t-p canvas: imprime el canvas indicado.\n");
    fprintf(stderr, "\t-c canvas: crea un canvas con el nombre indicado.\n");
    fprintf(stderr, "\t-d canvas: elimina el canvas indicado.\n");
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

    int descriptor,i,j,pos = 0;

    canvas_t* miMapa;

    switch(option) {
        case 'w':
            descriptor = shm_open(argv[2], O_RDWR, 0666);
            if (descriptor == -1) {
                perror("Error al abrir el canvas");
                exit(EXIT_FAILURE);
            }
            miMapa = (canvas_t *) mmap(NULL, sizeof(canvas_t), PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);
            if (miMapa == MAP_FAILED) {
                perror("Error en syscall mmap");
                close(descriptor);
                exit(EXIT_FAILURE);
            }

            size_t tamanioMensaje =  strlen(argv[3]);
            
            if (argv[6][0] == 'h') {
                for (i = 0; i < tamanioMensaje; i++) {
                    pos = (atoi(argv[4]) * WIDTH + (atoi(argv[5]) + i)) % (HEIGHT * WIDTH);
                    miMapa->canvas[pos] = argv[3][i];
                } 
            } 
            if (argv[6][0] == 'v') {
                for (i = 0; i < tamanioMensaje; i++) {
                    pos = ((atoi(argv[4]) + i) % HEIGHT) * WIDTH + atoi(argv[5]);
                    miMapa->canvas[pos] = argv[3][i];
                }
            }
            munmap(miMapa, sizeof(canvas_t));
            printf("Escribe %s en el canvas %s en la posición (%d, %d) de manera %c.\n", argv[3], argv[2], atoi(argv[4]), atoi(argv[5]), argv[6][0]);
            close(descriptor);
            break;
        case 'p':
            descriptor = shm_open(argv[2], O_RDWR, 0666);
            if (descriptor == -1) {
                perror("Error al abrir el canvas");
                exit(EXIT_FAILURE);
            }
            miMapa = (canvas_t*) mmap(NULL, sizeof(canvas_t), PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);
            if (miMapa == MAP_FAILED) {
                perror("Error en syscall mmap");
                close(descriptor);
                exit(EXIT_FAILURE);
            }
            for (i = 0; i < HEIGHT; i++) {
               for (j = 0; j < WIDTH; j++) {
                    putchar(miMapa->canvas[i * WIDTH + j]);
               }
               putchar('\n');
            }
            munmap(miMapa, sizeof(canvas_t));
            close(descriptor);
            break;
        case 'c':
            descriptor = shm_open(argv[2], O_CREAT | O_RDWR, 0666);
            if (descriptor == -1) {
                perror("Error al crear el canvas");
                exit(EXIT_FAILURE);
            }
            ftruncate(descriptor, sizeof(canvas_t));
            miMapa = (canvas_t*) mmap(argv[2], sizeof(canvas_t), PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);
            memset(miMapa->canvas,'-',sizeof(canvas_t));
            printf("Crea canvas.\n");
            close(descriptor);
            break;
        case 'd':
            descriptor = shm_open(argv[2], O_RDWR, 0666);
            if (descriptor == -1) {
                perror("Error al abrir el canvas");
                exit(EXIT_FAILURE);
            }
            shm_unlink(argv[2]);
            printf("Borra canvas.\n");
            close(descriptor);
            break;
        case 'h':
            usage(argv);
            break;
        default:
            fprintf(stderr, "Comando desconocido\n");
            exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
