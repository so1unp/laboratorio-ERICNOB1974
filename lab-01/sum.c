#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Agregar código aquí.

    int i=1;
    int sum=0;
    for (i;i<argc;i++){
        if ((*argv[i] >= '0') && (*argv[i] <= '9')){
            sum += atoi(argv[i]);
        }
        else {
            printf("Debe ingresar unicamente numeros\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("El resultado de la suma es: %d\n",sum);

    // Termina la ejecución del programa.
    exit(EXIT_SUCCESS);
}
