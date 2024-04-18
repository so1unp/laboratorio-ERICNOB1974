#include "types.h"
#include "user.h"
#include "date.h"

int main() {

    printf(1, "Numero de procesos antes del fork: %d\n", pscnt());

    int i;
    int pid = 0;

    for (i = 0; i < 1; i++) {
        if ((pid = fork()) == 0) { 
            exit();
        }
    }

    printf(1, "Numero de procesos despues del fork: %d\n", pscnt());

    if (pid != 0){
        wait();
    }

    exit();
}
