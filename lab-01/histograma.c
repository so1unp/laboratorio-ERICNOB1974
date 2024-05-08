#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Agregar código aquí.
    
    printf("Indique el numero maximo de letras del histograma (Maximo 20)\n");
    
    int longitud = 0;

    if (scanf("%d", &longitud) != 1) { // Verificar si se pudo leer un número
        printf("Entrada invalida. Ingrese un numero.\n");
        exit(EXIT_FAILURE);
    }

    if (longitud < 1 || longitud > 20) {
        printf("Numero fuera de rango. Ingrese un numero del 1 al 20.\n");
        exit(EXIT_FAILURE);
    }

    printf("Ingrese frases seguido de un enter (Control + D para terminar)\n");	

    char cantLetras[longitud+1];

    int i=0;
	
    for (i; i <= longitud; i++) {
        cantLetras[i] = 0;
    }
    char c=0;
    i=0;
    while ((c = getchar()) != EOF){
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            i++;
        }
        else{
            cantLetras[i]++;
            i=0;
        }
    }

    i=1;

    for (i;i<(sizeof(cantLetras)/sizeof(cantLetras[0]));i++){
        printf("%d ",i);
        int j=0;
        for (j;j<cantLetras[i];j++){
            printf("*");
        }
        printf("\n");
    }

    // Termina la ejecución del programa.
    exit(EXIT_SUCCESS);
}