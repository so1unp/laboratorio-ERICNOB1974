#include "types.h"
#include "user.h"
#include "date.h"

int main() {

    int pid = fork();
    
    if (pid == 0){
        printf(1,"Soy el hijo %d, mi padre es: %d\n", getpid(),getppid());
    }

    if (pid != 0){
        wait();
    }

    exit();
}