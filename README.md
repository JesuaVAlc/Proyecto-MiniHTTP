# Proyecto Primer Bimestre - Computación Distribuida GR2CC - MiniHTTPd
## Jesua Villacis

Servidor HTTP/1.1 basico implementado en C para Linux. Sirve archivos estaticos usando un event loop
con epoll, sin librerias HTTP externas.

## Estructura
minihttpd/
├── Makefile
├── include/
│   ├── server.h
│   ├── http.h
│   ├── files.h
│   └── mime.h
├── src/
│   ├── main.c
│   ├── server.c
│   ├── http.c
│   ├── files.c
│   ├── mime.c
│   ├── passivesock.c
│   └── passiveTCP.c
└── www/
├── index.html
├── style.css
└── image.png