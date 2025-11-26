#ifndef BUFFERS_H
#define BUFFERS_H

#include <pthread.h>
#include <semaphore.h>
#include "config.h"
#include "fruta.h" // Precisaremos da struct Fruta definida aqui

typedef struct {
    Fruta* buffer[TAMANHO_BUFFER]; // Array de ponteiros para frutas
    int in;     // Índice de inserção (produtor)
    int out;    // Índice de remoção (consumidor)
    int count;  // Quantidade atual de itens (para leitura da Interface)
    
    // Sincronização
    sem_t sem_vazio;       // Conta espaços livres
    sem_t sem_cheio;       // Conta itens disponíveis
    pthread_mutex_t mutex; // Proteção para acesso aos índices/count
} BufferCircular;

// Declaração dos buffers globais (serão definidos no main.c ou buffers.c)
extern BufferCircular buffer_colheita_lavagem;
extern BufferCircular buffer_lavagem_corte;
extern BufferCircular buffer_corte_extracao;
extern BufferCircular buffer_extracao_embalagem;

// Funções de manipulação
void init_buffer(BufferCircular* b);
void destroy_buffer(BufferCircular* b);
void depositar_fruta(BufferCircular* b, Fruta* f); // Bloqueante se cheio
Fruta* retirar_fruta(BufferCircular* b);           // Bloqueante se vazio

// Função segura para interface ler tamanho
int obter_tamanho_fila(BufferCircular* b);

#endif