#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Agregar código aquí.
    int c = 0;
    char palabras[100];
    printf("Ingrese una frase (O control + c para salir)\n");
    while(1){
	scanf("%s",palabras);
	if ((c = getchar()) != EOF){
		printf("%s\n", palabras);   		
        }
    }
    
    // Termina la ejecución del programa.
    exit(EXIT_SUCCESS);
}
