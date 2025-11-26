#ifndef INTERFACE_H
#define INTERFACE_H

#include <pthread.h>
#include <ncurses.h>
#include "fruta.h" 

// Estrutura de estado da interface
typedef struct {
    int frutas_concluidas;      
    pthread_mutex_t tela_mutex; 
    int interface_bloqueada;    
    
    // Controle de tempo visual
    float tempo_restante[4];    
    pthread_mutex_t tempo_mutex; 
} StatusInterface;

extern StatusInterface status_interface;

// Funções de ciclo de vida
void init_interface();
void cleanup_interface();
void* thread_atualizador_interface(void* arg);

// Controle de bloqueio (Menus)
void bloquear_interface();
void desbloquear_interface();
int interface_esta_bloqueada();

// Função principal de desenho
void desenhar_interface_completa();

// Funções chamadas pelos Workers (etapas.c)
void iniciar_processamento_etapa(int etapa, float tempo_total);
void atualizar_tempo_etapa(int etapa, float tempo_decorrido);
void finalizar_processamento_etapa(int etapa);
void fruta_concluida(Fruta* f);

#endif