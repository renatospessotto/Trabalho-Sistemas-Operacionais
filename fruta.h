#ifndef FRUTA_H
#define FRUTA_H

#include <time.h>

typedef struct {
    int id;
    time_t timestamp_criacao;
    // Adicione aqui stats futuros (ex: qualidade_acumulada)
} Fruta;

Fruta* criar_fruta(int id);
void destruir_fruta(Fruta* f);

#endif