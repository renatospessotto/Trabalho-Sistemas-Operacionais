#include "etapas.h"
#include "interface.h"
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_MAQUINAS 10
#define MIN_MAQUINAS 1

static sem_t sem_etapas[4];
static int num_maquinas[4] = {2, 1, 1, 1}; // Valores iniciais
static pthread_mutex_t maquinas_mutex = PTHREAD_MUTEX_INITIALIZER;

void init_etapas() {
    sem_init(&sem_etapas[LAVAR], 0, num_maquinas[LAVAR]);
    sem_init(&sem_etapas[CORTAR], 0, num_maquinas[CORTAR]);
    sem_init(&sem_etapas[EXTRAIR], 0, num_maquinas[EXTRAIR]);
    sem_init(&sem_etapas[EMBALAR], 0, num_maquinas[EMBALAR]);
}

void destroy_etapas() {
    for (int i = 0; i < 4; i++) {
        sem_destroy(&sem_etapas[i]);
    }
    pthread_mutex_destroy(&maquinas_mutex);
}

void processar_etapa(Fruta* f, Etapa etapa) {
    // Marcar que a fruta está tentando entrar na etapa (para mostrar na fila)
    marcar_fruta_entrando_etapa(f->id, etapa);
    
    sem_wait(&sem_etapas[etapa]);
    
    // Simular tempo de processamento (1-3 segundos aleatório)
    int tempo_processamento = 1 + (rand() % 3);
    sleep(tempo_processamento);
    
    marcar_fruta_saindo_etapa(f->id, etapa);
    sem_post(&sem_etapas[etapa]);
}

void incrementar_maquinas_etapa(Etapa etapa) {
    pthread_mutex_lock(&maquinas_mutex);
    
    if (num_maquinas[etapa] < MAX_MAQUINAS) {
        num_maquinas[etapa]++;
        
        // Atualizar o semáforo - incrementar o valor
        sem_post(&sem_etapas[etapa]);
        
        // Atualizar interface
        atualizar_maquinas_etapa(etapa, 1);
    }
    
    pthread_mutex_unlock(&maquinas_mutex);
}

void decrementar_maquinas_etapa(Etapa etapa) {
    pthread_mutex_lock(&maquinas_mutex);
    
    if (num_maquinas[etapa] > MIN_MAQUINAS) {
        // Verificar se podemos decrementar sem bloquear threads
        int sem_value;
        sem_getvalue(&sem_etapas[etapa], &sem_value);
        
        if (sem_value > 0) {
            num_maquinas[etapa]--;
            
            // Decrementar o semáforo
            sem_wait(&sem_etapas[etapa]);
            
            // Atualizar interface
            atualizar_maquinas_etapa(etapa, -1);
        }
    }
    
    pthread_mutex_unlock(&maquinas_mutex);
}

int get_num_maquinas_etapa(Etapa etapa) {
    pthread_mutex_lock(&maquinas_mutex);
    int valor = num_maquinas[etapa];
    pthread_mutex_unlock(&maquinas_mutex);
    return valor;
}
