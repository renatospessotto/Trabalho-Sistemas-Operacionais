#ifndef ETAPAS_H
#define ETAPAS_H

#include "fruta.h"

typedef enum { LAVAR, CORTAR, EXTRAIR, EMBALAR } Etapa;

void init_etapas();
void destroy_etapas();
void processar_etapa(Fruta* fruta, Etapa etapa);
void incrementar_maquinas_etapa(Etapa etapa);
void decrementar_maquinas_etapa(Etapa etapa);
int get_num_maquinas_etapa(Etapa etapa);

#endif
