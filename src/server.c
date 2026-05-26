#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../include/server.h"
#include "../include/http.h"

#define MAX_EVENTS  1024
#define BUF_SIZE    8192
#define WWW_ROOT   "./www"

int passiveTCP(const char *service, int qlen);

// Configurar los descriptores como no bloqueantes
static void set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// Inicializar la escucha de conexiones
int initServer(const char *port)
{
    // Crear un socket pasivo
    int listenfd = passiveTCP(port, SOMAXCONN);
    if (listenfd < 0)
        return -1;

    set_nonblocking(listenfd);
    return listenfd;
}

// Ejecutar el servidor
void runServer(int listenfd)
{
    // Inicializar la cola de epoll
    int epfd = epoll_create1(0);
    if (epfd < 0)
    {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    // Registrar los sockets a escuchar
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = listenfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

    struct epoll_event events[MAX_EVENTS];

    // bucle principal
    while (1)
    {
        int nready = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if (nready < 0)
        {
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < nready; i++)
        {
            int fd = events[i].data.fd;

            if (fd == listenfd)
            {
                // Aceptar nuevas conexiones
                int clientfd = accept(listenfd, NULL, NULL);
                if (clientfd < 0)
                {
                    perror("accept");
                    continue;
                }

                set_nonblocking(clientfd);

                // Agregar el descriptor a la cola de eventos
                ev.events = EPOLLIN;
                ev.data.fd = clientfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev);
                printf("\nCliente FD %d conectado.\n", clientfd);
            }
            else
            {
                // Recepcion de datos del cliente
                char buf[BUF_SIZE];
                int n = read(fd, buf, sizeof(buf) - 1);

                if (n <= 0)
                {   //Si el cliente cerro la conexion o error
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                    printf("Cliente FD %d desconectado.\n", fd);
                }
                else
                {
                    buf[n] = '\0';

                    //Analizar y parsear peticiones entrantes
                    HttpRequest req;
                    int ok = parseHTTP(buf, &req);

                    if (ok == 0)
                    {
                        printf("PETICIÓN FD %d: %s %s | UA: %s\n", fd, req.method, req.uri, req.user_agent);
                        httpRes(fd, &req);
                        //Solo cerrar si nO es keep-alive
                        if (!req.keep_alive)
                        {
                            epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                            close(fd);
                        }
                    }
                    else
                    {
                        //Enviar un header con error si la peticion es erronea
                        const char *bad = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
                        write(fd, bad, strlen(bad));
                        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                        close(fd);
                    }
                }
            }
        }
    }

    close(epfd);
}