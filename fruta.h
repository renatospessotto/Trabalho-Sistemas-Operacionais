#ifndef FRUTA_H
#define FRUTA_H

#include <time.h>

typedef struct {
    int id;
    int pontuacao;
    time_t entrada_etapa;
} Fruta;

void* thread_fruta(void* arg);

#endif
