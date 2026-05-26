#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <limits.h>
#include "../include/files.h"
#include "../include/server.h"

/*------------------------------------------------------------------------
 * Gestion de archivos
 *------------------------------------------------------------------------
 */
int openFile(const char *uri, const char *www_root, FileInfo *out)
{
    char full_path[PATH_MAX];
    char resolved[PATH_MAX];

    // Servir el archivo index.html por defecto
    if (strcmp(uri, "/") == 0)
        uri = "/index.html";

    // construir el path completo con la URI
    int written = snprintf(full_path, sizeof(full_path), "%s%s", www_root, uri);

    // Verificacion del tamaño del path
    if (written >= (int)sizeof(full_path))
        return -3;

    // Verificar el path absoluto de la peticion
    if (realpath(full_path, resolved) == NULL)
        return -1; // no existe -> 404

    /*------------------------------------------------------------------------
     * Verificación del path absoluto para evitar directory traversal
     *------------------------------------------------------------------------
     */
    char resolved_root[PATH_MAX];
    if (realpath(www_root, resolved_root) == NULL)
        return -3;
    //Verificar si el path no esta fuera del root
    if (strncmp(resolved, resolved_root, strlen(resolved_root)) != 0)
        return -2; 

    //Verificar que es un archivo regular 
    struct stat st;
    if (stat(resolved, &st) < 0)
        return -3;
    //Verificar que no sea un directorio
    if (!S_ISREG(st.st_mode))
        return -2; 

    //Verificar permisos de lectura
    if (access(resolved, R_OK) != 0)
        return -2; 

    //Abrir el archivo en modo solo lectura
    int fd = open(resolved, O_RDONLY);
    if (fd < 0)
        return -3;

    //Llenar la salida con el contenido
    strncpy(out->path, resolved, sizeof(out->path) - 1);
    out->path[sizeof(out->path) - 1] = '\0';
    out->size = st.st_size;
    out->fd = fd;

    return 0;
}

void closeFile(FileInfo *f)
{   
    //Cerrar el descriptor
    if (f->fd >= 0)
    {
        close(f->fd);
        f->fd = -1;
    }
}
