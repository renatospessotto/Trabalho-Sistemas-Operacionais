#ifndef INTERFACE_H
#define INTERFACE_H

#include "etapas.h"
#include <pthread.h>

// Estrutura para controlar o estado da interface
typedef struct {
    int frutas_em_etapa[4];
    int num_maquinas[4];
    int frutas_concluidas;
    int total_frutas;
    pthread_mutex_t tela_mutex;
} StatusInterface;

extern StatusInterface status_interface;

void iniciar_tela();
void encerrar_tela();
void desenhar_interface_completa();
void marcar_fruta_entrando_etapa(int fruta_id, Etapa etapa);
void marcar_fruta_saindo_etapa(int fruta_id, Etapa etapa);
void fruta_concluida(Fruta* fruta);
void atualizar_maquinas_etapa(Etapa etapa, int delta);
void desenhar_fila_espera(Etapa etapa, int tamanho_fila);
void atualizar_status();

#endif
