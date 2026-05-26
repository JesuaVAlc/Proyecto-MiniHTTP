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

## Compilar y correr

```bash
make
./minihttpd
```

El servidor escucha en el puerto 8080. Para probarlo:

```bash
# request normal
curl -v http://localhost:8080/

# probar 404
curl -v http://localhost:8080/noexiste.html

# probar directory traversal (debe responder 403)
curl -v http://localhost:8080/../../etc/passwd

# probar metodo invalido (debe responder 405)
curl -X POST http://localhost:8080/
```

## Funcionalidades

- Metodo GET con parsing de request line y headers
- Archivos estaticos desde el directorio `www/`
- Multiples clientes simultaneos con epoll
- Tipos MIME: html, css, js, png, jpg
- Codigos de estado: 200, 400, 403, 404, 405, 500
- Proteccion contra directory traversal con `realpath()`
- Sin uso de `strcpy`, `sprintf` ni `gets`

## Dependencias

- Linux (o WSL)
- gcc
- Make

## Archivos base

`passivesock.c` y `passiveTCP.c` fueron tomados de los ejemplos
de clase.