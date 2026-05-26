# Proyecto Primer Bimestre - Computación Distribuida GR2CC - MiniHTTPd
## Jesua Villacis

Servidor HTTP/1.1 basico implementado en C para Linux. Sirve archivos estaticos usando un event loop
con epoll, sin librerias HTTP externas.

## Características
- Servir archivos estaticos y definir el tipo de contenido mediante formato de extensión MIME.
- Analizar y ejecutar peticiones GET con cabeceras HTTP1.1 básicas.
- Manejo concurrentes de clientes con epoll.
- Códigos de estado HTTP : `200`, `400`, `403`, `404`, `405`, `500`.
- Robustez ante vulnerabilidades (directory traversal, buffer overflow)


## Estructura
```text
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
```

## Como compilarlo y ejecutarlo
1. Entrar al directorio del servidor y ejecutar el siguiente comando para compilar el códig y generar un ejecutable:
```bash
make
```
2. Para levantar el servidor ejecutar:
```bash
./minihttpd
```
El servidor estará escuchando las conexiones TCP en el puerto 8080

## Como conectarse con un cliente
- Ejecutar con curl:
```bash
curl http://localhost:8080/
```
## Extensiones MIME 
| Extension | MIME Type                |
|-----------|--------------------------|
| `.html`   | `text/html`              |
| `.css`    | `text/css`               |
| `.js`     | `application/javascript` |
| `.png`    | `image/png`              |
| `.jpg`    | `image/jpeg`             |
## Rutas y códigos de estado posibles
| Ruta | Códigos posibles |
|------|-----------------|
| `GET /` | 200, 404, 500 |
| `GET /index.html` | 200, 404, 500 |
| `GET /style.css` | 200, 404, 500 |
| `GET /image.png` | 200, 404, 500 |
| `GET /archivo.js` | 200, 404, 500 |
| `GET /no-existe` | 404 |
| `GET /../../etc/passwd` | 403 |
| `GET /directorio/` | 403 |
| `GET /archivo-sin-permisos` | 403 |
| `POST /`, `PUT /`, `DELETE /` | 405 |
| Request malformado / URI > 2048 | 400 |
| Fallo interno del servidor | 500 |

## Archivos Básicos
Se uso los archivos `passivesock.c` y `passiveTCP.c` de los recurso del aula para aumentar la modularidad del proyecto. 
