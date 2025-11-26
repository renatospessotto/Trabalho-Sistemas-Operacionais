#include "fruta.h"
#include <stdlib.h>

Fruta* criar_fruta(int id) {
    Fruta* f = (Fruta*) malloc(sizeof(Fruta));
    if (f) {
        f->id = id;
        f->timestamp_criacao = time(NULL);
    }
    return f;
}

void destruir_fruta(Fruta* f) {
    if (f) {
        free(f);
    }
}