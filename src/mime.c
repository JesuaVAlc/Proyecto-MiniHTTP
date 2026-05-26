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
    {NULL,    NULL}  // centinela para saber donde termina la tabla
};

const char *getMIME(const char *path) {
    // buscar el ultimo punto en el path para obtener la extension
    const char *dot = strrchr(path, '.');
    if (dot == NULL)
        return "application/octet-stream"; // sin extension, tipo generico

    // recorrer la tabla hasta encontrar la extension
    for (int i = 0; mime_map[i].ext != NULL; i++) {
        if (strcmp(dot, mime_map[i].ext) == 0)
            return mime_map[i].type;
    }

    return "application/octet-stream"; // extension desconocida
}