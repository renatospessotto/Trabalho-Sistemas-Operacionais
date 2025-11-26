#include "economia.h"
#include "config.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Necessário para leitura de string

SistemaEconomico sistema_economico;

// Tabelas de multiplicadores
static const float reducoes_velocidade[] = {0.0f, 0.25f, 0.37f, 0.50f};
static const float bonus_qualidade_colheita[] = {0.0f, 0.50f, 1.00f, 1.50f};
static const float bonus_qualidade_lavagem[] = {0.0f, 0.30f, 0.60f, 1.00f};
static const float bonus_qualidade_extracao[] = {0.0f, 0.75f, 1.25f, 2.00f};
static const float bonus_qualidade_embalagem[] = {0.0f, 1.00f, 2.00f, 3.00f};

static const float* tabelas_qualidade[4] = {
    bonus_qualidade_colheita,
    bonus_qualidade_lavagem,
    bonus_qualidade_extracao,
    bonus_qualidade_embalagem
};

// Declaração antecipada para usar dentro da inicialização
void carregar_progresso(const char* arquivo);

void inicializar_economia() {
    pthread_mutex_init(&sistema_economico.dinheiro_mutex, NULL);
    sistema_economico.dinheiro_total = 0.0;
    
    // Configuração inicial (Valores Base)
    float tempos_base[4] = { TEMPO_BASE_COLHEITA, TEMPO_BASE_LAVAGEM, TEMPO_BASE_EXTRACAO, TEMPO_BASE_EMBALAGEM };
    int custos_vel[4] = { CUSTO_INICIAL_VEL_COLHEITA, CUSTO_INICIAL_VEL_LAVAGEM, CUSTO_INICIAL_VEL_EXTRACAO, CUSTO_INICIAL_VEL_EMBALAGEM };
    int custos_qual[4] = { CUSTO_INICIAL_QUAL_COLHEITA, CUSTO_INICIAL_QUAL_LAVAGEM, CUSTO_INICIAL_QUAL_EXTRACAO, CUSTO_INICIAL_QUAL_EMBALAGEM };
    
    for(int i = 0; i < 4; i++) {
        sistema_economico.etapas[i].nivel_velocidade = 0;
        sistema_economico.etapas[i].nivel_qualidade = 0;
        sistema_economico.etapas[i].custo_velocidade = custos_vel[i];
        sistema_economico.etapas[i].custo_qualidade = custos_qual[i];
        sistema_economico.etapas[i].tempo_base = tempos_base[i];
        sistema_economico.etapas[i].tempo_atual = tempos_base[i]; // Sem upgrade = tempo base
        sistema_economico.etapas[i].bonus_qualidade = 0.0f;
    }

    // TENTA CARREGAR O SAVE AUTOMATICAMENTE
    carregar_progresso("savegame.txt");
}

void finalizar_economia() {
    pthread_mutex_destroy(&sistema_economico.dinheiro_mutex);
}

void adicionar_receita() {
    float valor = calcular_valor_produto();
    pthread_mutex_lock(&sistema_economico.dinheiro_mutex);
    sistema_economico.dinheiro_total += (double)valor;
    pthread_mutex_unlock(&sistema_economico.dinheiro_mutex);
}

double obter_dinheiro() {
    pthread_mutex_lock(&sistema_economico.dinheiro_mutex);
    double valor = sistema_economico.dinheiro_total;
    pthread_mutex_unlock(&sistema_economico.dinheiro_mutex);
    return valor;
}

float calcular_tempo_etapa(int etapa) {
    if(etapa < 0 || etapa >= 4) return 0.0f;
    // Retorna o tempo já pré-calculado e armazenado na struct
    // Isso evita recalcular toda vez, pois atualizamos o tempo ao comprar/carregar
    return sistema_economico.etapas[etapa].tempo_atual;
}

float calcular_valor_produto() {
    float valor_total = RECEITA_BASE;
    for(int i = 0; i < 4; i++) {
        valor_total += sistema_economico.etapas[i].bonus_qualidade;
    }
    return valor_total;
}

float obter_bonus_qualidade(int etapa) {
    if(etapa < 0 || etapa >= 4) return 0.0f;
    int nivel = sistema_economico.etapas[etapa].nivel_qualidade;
    return tabelas_qualidade[etapa][nivel];
}

int comprar_upgrade_velocidade(int etapa) {
    if(etapa < 0 || etapa >= 4) return 0;
    EtapaUpgrade* e = &sistema_economico.etapas[etapa];
    if(e->nivel_velocidade >= 3) return 0;
    
    pthread_mutex_lock(&sistema_economico.dinheiro_mutex);
    if(sistema_economico.dinheiro_total >= e->custo_velocidade) {
        sistema_economico.dinheiro_total -= e->custo_velocidade;
        pthread_mutex_unlock(&sistema_economico.dinheiro_mutex);
        
        e->nivel_velocidade++;
        
        // Recalcular stats
        float reducao = reducoes_velocidade[e->nivel_velocidade];
        e->tempo_atual = e->tempo_base * (1.0f - reducao);
        
        e->custo_velocidade = (int)(e->custo_velocidade * MULTIPLICADOR_CUSTO);
        return 1;
    }
    pthread_mutex_unlock(&sistema_economico.dinheiro_mutex);
    return 0;
}

int comprar_upgrade_qualidade(int etapa) {
    if(etapa < 0 || etapa >= 4) return 0;
    EtapaUpgrade* e = &sistema_economico.etapas[etapa];
    if(e->nivel_qualidade >= 3) return 0;
    
    pthread_mutex_lock(&sistema_economico.dinheiro_mutex);
    if(sistema_economico.dinheiro_total >= e->custo_qualidade) {
        sistema_economico.dinheiro_total -= e->custo_qualidade;
        pthread_mutex_unlock(&sistema_economico.dinheiro_mutex);
        
        e->nivel_qualidade++;
        
        // Recalcular stats
        e->bonus_qualidade = obter_bonus_qualidade(etapa);
        
        e->custo_qualidade = (int)(e->custo_qualidade * MULTIPLICADOR_CUSTO);
        return 1;
    }
    pthread_mutex_unlock(&sistema_economico.dinheiro_mutex);
    return 0;
}

// Getters
int obter_nivel_velocidade(int etapa) { return (etapa >=0 && etapa <4) ? sistema_economico.etapas[etapa].nivel_velocidade : 0; }
int obter_nivel_qualidade(int etapa) { return (etapa >=0 && etapa <4) ? sistema_economico.etapas[etapa].nivel_qualidade : 0; }
int obter_custo_velocidade(int etapa) { return (etapa >=0 && etapa <4) ? sistema_economico.etapas[etapa].custo_velocidade : 0; }
int obter_custo_qualidade(int etapa) { return (etapa >=0 && etapa <4) ? sistema_economico.etapas[etapa].custo_qualidade : 0; }

void salvar_progresso(const char* arquivo) {
    FILE* file = fopen(arquivo, "w");
    if(!file) return;
    
    // Salva apenas os dados essenciais (Dinheiro e Níveis)
    fprintf(file, "dinheiro=%.2f\n", sistema_economico.dinheiro_total);
    for(int i = 0; i < 4; i++) {
        fprintf(file, "etapa%d_vel=%d\n", i, sistema_economico.etapas[i].nivel_velocidade);
        fprintf(file, "etapa%d_qual=%d\n", i, sistema_economico.etapas[i].nivel_qualidade);
    }
    fclose(file);
}

void carregar_progresso(const char* arquivo) {
    FILE* file = fopen(arquivo, "r");
    if(!file) return; // Se não existe save, começa do zero (já inicializado)
    
    char linha[100];
    double dinheiro_load = 0;
    
    // Leitura linha a linha
    while(fgets(linha, sizeof(linha), file)) {
        // Tenta ler dinheiro
        if(strncmp(linha, "dinheiro=", 9) == 0) {
            sscanf(linha, "dinheiro=%lf", &dinheiro_load);
            sistema_economico.dinheiro_total = dinheiro_load;
        }
        else {
            // Tenta ler upgrades
            int etapa, valor;
            
            // Velocidade
            if(sscanf(linha, "etapa%d_vel=%d", &etapa, &valor) == 2) {
                if(etapa >= 0 && etapa < 4) {
                    sistema_economico.etapas[etapa].nivel_velocidade = valor;
                    
                    // RECALCULAR TEMPO E CUSTO BASEADO NO NÍVEL CARREGADO
                    // Simula a inflação de preço dos níveis anteriores
                    for(int n = 0; n < valor; n++) {
                        sistema_economico.etapas[etapa].custo_velocidade = 
                            (int)(sistema_economico.etapas[etapa].custo_velocidade * MULTIPLICADOR_CUSTO);
                    }
                    // Atualiza tempo
                    float reducao = reducoes_velocidade[valor];
                    sistema_economico.etapas[etapa].tempo_atual = 
                        sistema_economico.etapas[etapa].tempo_base * (1.0f - reducao);
                }
            }
            
            // Qualidade
            else if(sscanf(linha, "etapa%d_qual=%d", &etapa, &valor) == 2) {
                if(etapa >= 0 && etapa < 4) {
                    sistema_economico.etapas[etapa].nivel_qualidade = valor;
                    
                    // RECALCULAR CUSTO E BÔNUS
                    for(int n = 0; n < valor; n++) {
                        sistema_economico.etapas[etapa].custo_qualidade = 
                            (int)(sistema_economico.etapas[etapa].custo_qualidade * MULTIPLICADOR_CUSTO);
                    }
                    sistema_economico.etapas[etapa].bonus_qualidade = obter_bonus_qualidade(etapa);
                }
            }
        }
    }
    fclose(file);
}