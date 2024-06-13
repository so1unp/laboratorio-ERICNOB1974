#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TAM_MEM_VIRTUAL 64
#define TAM_MEM_FISICA 32
#define TAM_PAGINA 4
#define TAM_MEM_SECUNDARIA 64

#define MEM_VIRTUAL (TAM_MEM_VIRTUAL / TAM_PAGINA)
#define MEM_FISICA (TAM_MEM_FISICA / TAM_PAGINA)
#define MEM_SECUNDARIA (TAM_MEM_SECUNDARIA / TAM_PAGINA)
#define CANT_PROCESOS 10

typedef struct {
    int pid;
    int page[MEM_VIRTUAL];
} proceso;

typedef struct {
    int pidProceso;
    int page;
} paginaFisica;

paginaFisica memFisica[MEM_FISICA];
paginaFisica memSecundaria[MEM_SECUNDARIA];
proceso procesos[CANT_PROCESOS];

int tiempo[MEM_FISICA];
int fifoIndex = 0;
int tiempoGlobal = 0;

int buscarMarcoLibre() {
    int i;
    for (i = 0; i < MEM_FISICA; i++) {
        if (memFisica[i].pidProceso == -1) {
            return i;
        }
    }
    return -1;
}

void imprimirTablas() {
    printf("Tabla de páginas:\n");
    int i,j;
    for (i = 0; i < CANT_PROCESOS; i++) {
        if (procesos[i].pid != -1) {
            printf("Proceso %d: ", procesos[i].pid);
            for (j = 0; j < MEM_VIRTUAL; j++) {
                if (procesos[i].page[j] != -1) {
                    printf("%d ", procesos[i].page[j]);
                } else {
                    printf("- ");
                }
            }
            printf("\n");
        }
    }
}

void imprimirMemoriaFisica() {
    printf("Memoria física:\n");
    int i;
    for (i = 0; i < MEM_FISICA; i++) {
        if (memFisica[i].pidProceso != -1) {
            printf("%d.%d ", memFisica[i].pidProceso, memFisica[i].page);
        } else {
            printf("- ");
        }
    }
    printf("\n");
}

void imprimirMemoriaSecundaria() {
    printf("Almacenamiento secundario:\n");
    int i;
    for (i = 0; i < MEM_SECUNDARIA; i++) {
        if (memSecundaria[i].pidProceso != -1) {
            printf("%d.%d ", memSecundaria[i].pidProceso, memSecundaria[i].page);
        } else {
            printf("- ");
        }
    }
    printf("\n");
}

int reemplazarFIFO() {
    int marco = fifoIndex;
    fifoIndex = (fifoIndex + 1) % MEM_FISICA;
    return marco;
}

int reemplazarLRU() {
    int lruIndex = 0;
    int i;
    for (i = 1; i < MEM_FISICA; i++) {
        if (tiempo[i] < tiempo[lruIndex]) {
            lruIndex = i;
        }
    }
    return lruIndex;
}

int main(int argc, char* argv[]) {

    if (argc != 2 || (strcmp(argv[1], "-f") != 0 && strcmp(argv[1], "-l") != 0)) {
        fprintf(stderr, "Usage: %s -f (FIFO) | -l (LRU)\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    //Inicializar
    char algoritmoReemplazo = argv[1][1];
    int i, j;
    for (i = 0; i < MEM_FISICA; i++) {
        memFisica[i].pidProceso = -1;
        memFisica[i].page = -1;
        tiempo[i] = 0;
    }
    for (i = 0; i < MEM_SECUNDARIA; i++) {
        memSecundaria[i].pidProceso = -1;
        memSecundaria[i].page = -1;
    }
    for (i = 0; i < CANT_PROCESOS; i++) {
        procesos[i].pid = -1;
        for (j = 0; j < MEM_VIRTUAL; j++) {
            procesos[i].page[j] = -1;
        }
    }

    int pid, pagina;
    while (scanf("%d\n%d\n", &pid, &pagina) != EOF) {
        int indiceProceso = -1;
        int i;
        for (i = 0; i < CANT_PROCESOS; i++) {
            if (procesos[i].pid == pid) {
                indiceProceso = i;
                break;
            }
            if (procesos[i].pid == -1) {
                indiceProceso = i;
                procesos[i].pid = pid;
                break;
            }
        }

        if (indiceProceso == -1) {
            fprintf(stderr, "Error! No pueden ser mas de 10 procesos.\n");
            return EXIT_FAILURE;
        }

        if (procesos[indiceProceso].page[pagina] != -1) {
            tiempo[procesos[indiceProceso].page[pagina]] = tiempoGlobal++;
            continue; // La pag ya está en memoria, actualiza el tiempo y pasa al siguiente
        }

        int marco = buscarMarcoLibre();

        if (marco == -1) {
            if (algoritmoReemplazo == 'f') {
                marco = reemplazarFIFO();
            } 
            if (algoritmoReemplazo == 'l') {
                marco = reemplazarLRU();
            }
            int pidAntiguo = memFisica[marco].pidProceso;
            int paginaAntigua = memFisica[marco].page;
            procesos[pidAntiguo - 1].page[paginaAntigua] = -1;

            int marcoSecundario = -1;
            for (i = 0; i < MEM_SECUNDARIA; i++) {
                if (memSecundaria[i].pidProceso == -1) {
                    memSecundaria[i].pidProceso = pidAntiguo;
                    memSecundaria[i].page = paginaAntigua;
                    marcoSecundario = i;
                    break;
                }
            }

            if (marcoSecundario == -1) {
                fprintf(stderr, "Error! Memoria secundaria llena.\n");
                return EXIT_FAILURE;
            }
        }

        memFisica[marco].pidProceso = pid;
        memFisica[marco].page = pagina;
        procesos[indiceProceso].page[pagina] = marco;
        tiempo[marco] = tiempoGlobal++;
    }

    imprimirTablas();
    imprimirMemoriaFisica();
    imprimirMemoriaSecundaria();

    return EXIT_SUCCESS;

}