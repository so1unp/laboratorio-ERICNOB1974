#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    printf("Cadena pasada por el usario: ");
    // Agregar código aquí.
    int i=1;
    for (i;i<argc;i++){
        printf("%s ", argv[i]);
    }
    printf("\nCantidad de divisiones del array de punteros: %d\n",argc);
    // Termina la ejecución del programa.
    exit(EXIT_SUCCESS);
}
