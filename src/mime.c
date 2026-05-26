#include <string.h>
#include "../include/mime.h"

// tabla de extensiones y sus tipos MIME
typedef struct {
    const char *ext;
    const char *type;
} MimeEntry;

static MimeEntry mime_map[] = {
    {".html", "text/html"},
    {".css",  "text/css"},
    {".js",   "application/javascript"},
    {".png",  "image/png"},
    {".jpg",  "image/jpeg"},
    {".jpeg", "image/jpeg"}, 
    {NULL,    NULL}  //Centinela para saber donde termina la tabla
};

const char *getMIME(const char *path) {
    //Buscar el ultimo punto en el path para obtener la extension
    const char *dot = strrchr(path, '.');
    if (dot == NULL)
        return "application/octet-stream"; //Sin extencion, tipo generico

    //Recorrer la tabla hasta encontrar el tipo
    for (int i = 0; mime_map[i].ext != NULL; i++) {
        if (strcmp(dot, mime_map[i].ext) == 0)
            return mime_map[i].type;
    }

    return "application/octet-stream"; //Extension desconocida
}