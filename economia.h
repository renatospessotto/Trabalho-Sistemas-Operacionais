#ifndef ECONOMIA_H
#define ECONOMIA_H

#include <pthread.h>
#include "config.h" // Pega as constantes de custo/tempo

typedef struct {
    int nivel_velocidade;
    int nivel_qualidade;
    int custo_velocidade;
    int custo_qualidade;
    float tempo_base;
    float tempo_atual;
    float bonus_qualidade;
} EtapaUpgrade;

typedef struct {
    EtapaUpgrade etapas[4];
    double dinheiro_total;
    pthread_mutex_t dinheiro_mutex;
} SistemaEconomico;

// Sistema econômico global
extern SistemaEconomico sistema_economico;

// Inicialização
void inicializar_economia();
void finalizar_economia();

// Transações
void adicionar_receita();
double obter_dinheiro();

// Upgrades
int comprar_upgrade_velocidade(int etapa);
int comprar_upgrade_qualidade(int etapa);

// Cálculos e Getters
float calcular_tempo_etapa(int etapa);
float calcular_valor_produto();
float obter_bonus_qualidade(int etapa);

// Getters de Info (para Interface)
int obter_nivel_velocidade(int etapa);
int obter_nivel_qualidade(int etapa);
int obter_custo_velocidade(int etapa);
int obter_custo_qualidade(int etapa);

// Persistência
void salvar_progresso(const char* arquivo);
void carregar_progresso(const char* arquivo);

#endif