#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include "../include/http.h"
#include "../include/files.h"
#include "../include/mime.h"

#define BUF_SIZE 8192
#define MAX_URI 2048
#define WWW_ROOT "./www"

/*------------------------------------------------------------------------
 * Validar solo peticiones GET
 *------------------------------------------------------------------------
 */
static int isGetMethod(const char *method)
{
    return strncmp(method, "GET", 3) == 0;
}

// Envia una respuesta con el codigo del error
static void sendError(int fd, int code, const char *msg)
{
    char header[256];
    snprintf(header, sizeof(header),
             "HTTP/1.1 %d %s\r\n"
             "Content-Length: 0\r\n"
             "Connection: close\r\n"
             "\r\n",
             code, msg);
    write(fd, header, strlen(header));
}

// Método que analiza la peticion
int parseHTTP(const char *raw, HttpRequest *req)
{
    char buf[BUF_SIZE];
    // Verificar si se ha excedido el tamaño del buffer
    if (strlen(raw) >= BUF_SIZE)
    {
        return -1;
    }

    // Se realiza una copia del buffer
    strncpy(buf, raw, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    // Se extrae la primera linea que sera la que contiene el metodo y la uri
    char *line_end = strstr(buf, "\r\n");
    if (line_end == NULL)
        return -1;
    *line_end = '\0';

    // Se separa el metodo, uri y version
    char *method = strtok(buf, " ");
    char *uri = strtok(NULL, " ");
    char *version = strtok(NULL, " ");
    const char *host = strstr(raw, "Host:");

    if (method == NULL || uri == NULL || version == NULL || host == NULL)
        return -1;

    // Validamos el tamaño de la URI
    if (strlen(uri) >= MAX_URI)
        return -1;

    strncpy(req->method, method, sizeof(req->method) - 1);
    strncpy(req->uri, uri, sizeof(req->uri) - 1);
    req->method[sizeof(req->method) - 1] = '\0';
    req->uri[sizeof(req->uri) - 1] = '\0';

    //Verificar User-Agent del encabezado
    const char *ua_ptr = strstr(raw, "User-Agent:");
    if (ua_ptr != NULL)
    {
        ua_ptr += 11; 
        while (*ua_ptr == ' ')
            ua_ptr++;

        int i = 0;
        while (ua_ptr[i] != '\r' && ua_ptr[i] != '\n' && ua_ptr[i] != '\0' && i < 511)
        {
            req->user_agent[i] = ua_ptr[i];
            i++;
        }
        req->user_agent[i] = '\0';
    }
    else
    {
        strcpy(req->user_agent, "Unknown");
    }

    // Se busca el header de Connection y se verifica el Keep alive
    req->keep_alive = 1;
    const char *conn = strstr(raw, "Connection:");
    if (conn != NULL && strstr(conn, "close") != NULL)
        req->keep_alive = 0;

    return 0;
}

/*------------------------------------------------------------------------
 * Verificaciones de codigos de error o exito
 *------------------------------------------------------------------------
 */

void httpRes(int clientfd, const HttpRequest *req)
{
    // URI vacia o malformada
    if (req->uri[0] == '\0' || req->uri[0] != '/')
    {
        sendError(clientfd, 400, "Bad Request");
        return;
    }

    // Rechazar toda peticion que no es GET
    if (!isGetMethod(req->method))
    {
        sendError(clientfd, 405, "Method Not Allowed");
        return;
    }

    // Se abre el archivo de la petición
    FileInfo fi;
    int result = openFile(req->uri, WWW_ROOT, &fi);

    if (result == -1)
    {
        sendError(clientfd, 404, "Not Found");
        return;
    }
    if (result == -2)
    {
        sendError(clientfd, 403, "Forbidden");
        return;
    }
    if (result == -3)
    {
        sendError(clientfd, 500, "Internal Server Error");
        return;
    }

    // Conseguir el tipo MIME de acuerdo al recurso
    const char *mime = getMIME(fi.path);

    // Eviar el Header de respuesta
    char header[512];
    snprintf(header, sizeof(header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %ld\r\n"
             "Connection: %s\r\n"
             "\r\n",
             mime, fi.size,
             req->keep_alive ? "keep-alive" : "close");

    write(clientfd, header, strlen(header));

    // Se envia el archivo directamnte desde el kernel
    sendfile(clientfd, fi.fd, NULL, fi.size);

    closeFile(&fi);
}