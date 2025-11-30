#ifndef INTERFACE_H
#define INTERFACE_H

#include <pthread.h>
#include <ncurses.h>
#include "fruta.h" 

// Estrutura de estado da interface
typedef struct {
    int frutas_concluidas;           // Total de frutas finalizadas
    pthread_mutex_t tela_mutex;      // Proteção da tela ncurses
    int interface_bloqueada;         // Evita conflitos com menus

    // Controle de tempo visual por etapa
    float tempo_restante[4];         // Tempo restante de cada etapa
    pthread_mutex_t tempo_mutex;     // Mutex do tempo
} StatusInterface;

// Variável global (declarada no interface.c)
extern StatusInterface status_interface;

// Ciclo de vida da interface
void init_interface();
void cleanup_interface();
void* thread_atualizador_interface(void* arg);

// Controle de bloqueio (para menus)
void bloquear_interface();
void desbloquear_interface();
int interface_esta_bloqueada();

// Função principal de desenho 
void desenhar_interface_completa();

//  Chamadas vindas das threads das etapas 
void iniciar_processamento_etapa(int etapa, float tempo_total);
void atualizar_tempo_etapa(int etapa, float tempo_decorrido);
void finalizar_processamento_etapa(int etapa);
void fruta_concluida(Fruta* f);

// HUD (informações fixas na tela)
void desenhar_hud();

#endif
