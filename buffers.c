#include "buffers.h"
#include <stdio.h>
#include <stdlib.h>

// Instâncias globais dos buffers
BufferCircular buffer_colheita_lavagem;
BufferCircular buffer_lavagem_corte;
BufferCircular buffer_corte_extracao;
BufferCircular buffer_extracao_embalagem;

void init_buffer(BufferCircular* b) {
    b->in = 0;
    b->out = 0;
    b->count = 0;
    
    // SEMÁFORO VAZIO: Começa com TAMANHO_BUFFER (todas as vagas livres)
    // Controla quantos espaços existem para ESCREVER
    sem_init(&b->sem_vazio, 0, TAMANHO_BUFFER);
    
    // SEMÁFORO CHEIO: Começa com 0 (nenhuma fruta pronta)
    // Controla quantas frutas existem para LER
    sem_init(&b->sem_cheio, 0, 0);
    
    // Mutex apenas para proteger a manipulação dos índices do array
    pthread_mutex_init(&b->mutex, NULL);
}

void destroy_buffer(BufferCircular* b) {
    sem_destroy(&b->sem_vazio);
    sem_destroy(&b->sem_cheio);
    pthread_mutex_destroy(&b->mutex);
}

void depositar_fruta(BufferCircular* b, Fruta* f) {
    // 1. BLOQUEIO RÍGIDO: Checa se tem espaço.
    // Se count == TAMANHO_BUFFER, o semáforo é 0 e a thread DORME AQUI.
    // Ela só acorda quando alguém retirar uma fruta lá na frente.
    sem_wait(&b->sem_vazio);
    
    // 2. Região Crítica (Mexe no array com segurança)
    pthread_mutex_lock(&b->mutex);
    
    // Salvaguarda extra (debug): Nunca deve acontecer se o semáforo funcionar
    if (b->count < TAMANHO_BUFFER) {
        b->buffer[b->in] = f;
        b->in = (b->in + 1) % TAMANHO_BUFFER;
        b->count++;
    }
    
    pthread_mutex_unlock(&b->mutex);
    
    // 3. Avisa o próximo estágio que tem fruta nova
    sem_post(&b->sem_cheio);
}

Fruta* retirar_fruta(BufferCircular* b) {
    Fruta* f = NULL;
    
    // 1. BLOQUEIO RÍGIDO: Checa se tem fruta.
    // Se count == 0, o semáforo é 0 e a thread DORME AQUI.
    sem_wait(&b->sem_cheio);
    
    // 2. Região Crítica
    pthread_mutex_lock(&b->mutex);
    
    if (b->count > 0) {
        f = b->buffer[b->out];
        b->out = (b->out + 1) % TAMANHO_BUFFER;
        b->count--;
    }
    
    pthread_mutex_unlock(&b->mutex);
    
    // 3. SINAL DE DESBLOQUEIO: Avisa a etapa anterior que abriu uma vaga!
    // É isso que faz a etapa anterior (que estava dormindo no depositar) acordar.
    sem_post(&b->sem_vazio);
    
    return f;
}

int obter_tamanho_fila(BufferCircular* b) {
    int valor;
    pthread_mutex_lock(&b->mutex);
    valor = b->count;
    pthread_mutex_unlock(&b->mutex);
    return valor;
}

int fila_ficou_cheia(BufferCircular* b) {
    int valor;
    pthread_mutex_lock(&b->mutex);
    valor = (b->count >= TAMANHO_BUFFER);
    pthread_mutex_unlock(&b->mutex);
    return valor;
}