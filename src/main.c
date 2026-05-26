#include <stdio.h>
#include <stdlib.h>
#include "../include/server.h"

#define PORT       "8080"

int main() {
    //Inicializa el socket en el puerto 8080
    int listenfd = initServer(PORT);
    if (listenfd < 0) {
        fprintf(stderr, "Error al iniciar el servidor\n");
        exit(EXIT_FAILURE);
    }

    printf("Servidor escuchando en puerto %s\n", PORT);

    //Ejecuta el loop principal con epoll
    runServer(listenfd);

    return 0;
}