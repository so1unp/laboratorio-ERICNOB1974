#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

void handler(int signum){
    printf("Senal recibida: %i\n", signum);
    printf("Descripcion: %s\n", strsignal(signum));
}

int main(void)
{

    printf("Numero PID: %d\n",getpid());

    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    int i = 0;
    for (i = 0;i < NSIG; i++){
        sigaction(i, &sa, NULL);
    }

    while (pause()){}

    exit(EXIT_SUCCESS);

}