#ifndef ETAPAS_H
#define ETAPAS_H

// Funções das threads operárias
void* thread_colheita(void* arg); // Produtor inicial
void* thread_lavar(void* arg);
void* thread_cortar(void* arg);
void* thread_extrair(void* arg);
void* thread_embalar(void* arg);  // Consumidor final

// Enumeração para facilitar índices
typedef enum {
    ETAPA_LAVAR = 0,
    ETAPA_CORTAR = 1,
    ETAPA_EXTRAIR = 2,
    ETAPA_EMBALAR = 3
} EtapaID;

#endif