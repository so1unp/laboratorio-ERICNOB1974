#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <unistd.h>

#define USERNAME_MAXSIZE    15  // Máximo tamaño en caracteres del nombre del remitente.
#define TXT_SIZE            100 // Máximo tamaño del texto del mensaje.

/**
 * Estructura del mensaje:
 * - sender: nombre del usuario que envió el mensaje.
 * - text: texto del mensaje.
 */
struct msg {
    char sender[USERNAME_MAXSIZE];
    char text[TXT_SIZE];
};

typedef struct msg msg_t;

/**
 * Imprime información acerca del uso del programa.
 */
void usage(char *argv[])
{
    fprintf(stderr, "Uso: %s comando parametro\n", argv[0]);
    fprintf(stderr, "Comandos:\n");
    fprintf(stderr, "\t-s queue mensaje: escribe el mensaje en queue.\n");
    fprintf(stderr, "\t-r queue: imprime el primer mensaje en queue.\n");
    fprintf(stderr, "\t-a queue: imprime todos los mensaje en queue.\n");
    fprintf(stderr, "\t-l queue: vigila por mensajes en queue.\n");
    fprintf(stderr, "\t-c queue: crea una cola de mensaje queue.\n");
    fprintf(stderr, "\t-d queue: elimina la cola de mensajes queue.\n");
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

    mqd_t cola;
    msg_t mensaje;
    struct mq_attr attr;
    attr.mq_flags = 0;			 // Flags: 0 or O_NONBLOCK
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(mensaje);
    attr.mq_curmsgs = 0;
    char option = argv[1][1];

    switch(option) {
        case 's':
            cola = mq_open (argv[2], O_RDWR);
            if (cola == -1) { 
                perror("No se puede acceder a la cola de mensajes");
                exit(1);
            }
            strncpy(mensaje.text, argv[3],strlen(argv[3]));
            getlogin_r(mensaje.sender,sizeof(mensaje.sender));
            if ((mq_send(cola,(char*)&mensaje, sizeof(mensaje),0)) == -1) { 
                perror("Error al enviar el mensaje");
                exit(1); 
            }
            printf("Se envió %s a la cola %s\n", argv[3], argv[2]);
            mq_close(cola);
            break;
        case 'r':
            cola = mq_open (argv[2],  O_RDWR | O_NONBLOCK);
            if (cola == -1) { 
                perror("No se puede acceder a la cola de mensajes");
                exit(1);
            }
            if (mq_receive(cola, (char*) &mensaje, sizeof(mensaje),0) == -1) { 
                perror("Error al recibir el mensaje"); 
                exit(1); 
            }
            printf("Se recibió el primer mensaje: %s de: %s\n",mensaje.text,mensaje.sender);
            mq_close(cola);

            break;
        case 'a':
            cola = mq_open (argv[2],  O_RDWR | O_NONBLOCK);
            if (cola == -1) { 
                perror("No se puede acceder a la cola de mensajes");
                exit(1);
            }
            ssize_t bytesLeidos = mq_receive(cola, (char*) &mensaje, sizeof(mensaje),0);
            if (bytesLeidos == -1) { 
                perror("Error al recibir el mensaje"); 
                exit(1); 
            }
            while (bytesLeidos>=0){
                printf("Recibe el mensaje en %s\n",mensaje.text);
                bytesLeidos = mq_receive(cola, (char*) &mensaje, sizeof(mensaje),0);
            }
            mq_close(cola);
            printf("Imprime todos los mensajes en %s\n", argv[2]);
            break;
        case 'l':
            cola = mq_open (argv[2],  O_RDWR);
            if (cola == -1) { 
                perror("No se puede acceder a la cola de mensajes");
                exit(1);
            }
            bytesLeidos = mq_receive(cola, (char*) &mensaje, sizeof(mensaje),0);
            if (bytesLeidos == -1) { 
                perror("Error al recibir el mensaje"); 
                exit(1); 
            }
            while (bytesLeidos>=0){
                printf("Recibe el mensaje en %s\n",mensaje.text);
                bytesLeidos = mq_receive(cola, (char*) &mensaje, sizeof(mensaje),0);
            }
            printf("Escucha indefinidamente por mensajes\n");
            mq_close(cola);
            break;
        case 'c':
            umask(0000);
            cola = mq_open (argv[2],  O_CREAT, 0622, &attr);
            if (cola == -1) {   
                perror("No se puede crear la cola de mensajes");
                exit(1);  
            }
            printf("Crea la cola de mensajes %s\n", argv[2]);
            mq_close(cola);
            break;
        case 'd':
            mq_unlink(argv[2]);
            printf("Borra la cola de mensajes %s\n", argv[2]);
            break;
        case 'h':
            usage(argv);
            break;
        case 'i':
            cola = mq_open (argv[2],  O_RDWR);
            if (cola == -1) { 
                perror("No se puede acceder a la cola de mensajes");
                exit(1);
            }
            if (mq_getattr(cola,&attr) == -1){
                perror("No se puede acceder a los atributos");
                exit(1);
            }
            printf("Cola: %s\nMaximo numero de mensajes: %i\nTamaño de los mensajes: %i\nNumero actual de mensajes en la cola: %i\n",argv[2],(int) attr.mq_maxmsg,(int) attr.mq_msgsize, (int) attr.mq_curmsgs);
            mq_close(cola);

            break;
        default:
            fprintf(stderr, "Comando desconocido: %s\n", argv[1]);
            exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
}
