#ifndef BUFFERS_H
#define BUFFERS_H

#include <pthread.h>
#include <semaphore.h>
#include "fruta.h"
#include "config.h"

// Tamanho fixo do buffer (deve ser definido em config.h)
#ifndef TAMANHO_BUFFER
#define TAMANHO_BUFFER 10 
#endif

// Estrutura do Buffer Circular
typedef struct {
    Fruta* buffer[TAMANHO_BUFFER]; // O array de frutas
    int in;                        // Índice de inserção (próxima posição livre)
    int out;                       // Índice de remoção (próxima fruta a ser lida)
    int count;                     // Número de itens no buffer
    sem_t sem_vazio;               // Semáforo para controlar vagas livres
    sem_t sem_cheio;               // Semáforo para controlar frutas prontas
    pthread_mutex_t mutex;         // Mutex para proteger a manipulação dos índices
} BufferCircular;

// Instâncias globais dos buffers
extern BufferCircular buffer_colheita_lavagem;
extern BufferCircular buffer_lavagem_corte;
extern BufferCircular buffer_corte_extracao;
extern BufferCircular buffer_extracao_embalagem;

// Funções do Buffer
void init_buffer(BufferCircular* b);
void destroy_buffer(BufferCircular* b);
void depositar_fruta(BufferCircular* b, Fruta* f);
Fruta* retirar_fruta(BufferCircular* b);
int obter_tamanho_fila(BufferCircular* b);
int fila_ficou_cheia(BufferCircular* b);

// NOVO: Função para a Interface conseguir o ponteiro do array de Frutas
Fruta** obter_array_frutas(BufferCircular* b); 

#endif