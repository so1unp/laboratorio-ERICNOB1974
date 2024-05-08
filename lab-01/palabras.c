#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Agregar código aquí.
    char c = 0;
    printf("Ingrese una frase (O control + c para salir)\n");
    while(1){
        if ((c = getchar()) != EOF){
            if (c != ' '){
                printf("%c",c);	
            }
            else{
                printf("\n");
            }
        }
    }
    
    // Termina la ejecución del programa.
    exit(EXIT_SUCCESS);
}
