#include "etapas.h"
#include "buffers.h"
#include "config.h"
#include "interface.h"
#include "economia.h"
#include <unistd.h>
#include <stdlib.h>

// Função auxiliar para simular trabalho
void simular_trabalho(int etapa_id) {
    float tempo_total = calcular_tempo_etapa(etapa_id);
    
    // Avisa interface: "Estou ocupado"
    iniciar_processamento_etapa(etapa_id, tempo_total);
    
    // Loop de espera visual
    float tempo_restante = tempo_total;
    while(tempo_restante > 0) {
        usleep(100000); // 100ms
        tempo_restante -= 0.1f;
        atualizar_tempo_etapa(etapa_id, tempo_total - tempo_restante);
    }
    
    // Avisa interface: "Terminei, vou tentar passar pra frente"
    finalizar_processamento_etapa(etapa_id);
}

// --- THREAD 1: COLHEITA ---
void* thread_colheita(void* arg) {
    int id_counter = 1;
    while(1) {
        usleep(TEMPO_BASE_COLHEITA * 1000000);
        
        Fruta* f = criar_fruta(id_counter++);
        
        // Tenta colocar na fila. Se LAVAR estiver cheia, TRAVA AQUI.
        depositar_fruta(&buffer_colheita_lavagem, f);
    }
    return NULL;
}

// --- THREAD 2: LAVAR ---
void* thread_lavar(void* arg) {
    while(1) {
        // 1. Tenta pegar. Se não tiver fruta, TRAVA AQUI.
        Fruta* f = retirar_fruta(&buffer_colheita_lavagem);
        
        // 2. Trabalha
        simular_trabalho(ETAPA_LAVAR);
        
        // 3. Deposita normalmente
        depositar_fruta(&buffer_lavagem_corte, f);
        
        // 4. Verifica se a fila ficou cheia após depositar
        if (fila_ficou_cheia(&buffer_lavagem_corte)) {
            marcar_etapa_travada(ETAPA_LAVAR, 1); // Marca como travado
            // Fica travado até alguém retirar da fila
            while (fila_ficou_cheia(&buffer_lavagem_corte)) {
                usleep(50000); // Verifica a cada 50ms
            }
            marcar_etapa_travada(ETAPA_LAVAR, 0); // Desfaz travamento
        }
    }
    return NULL;
}

// --- THREAD 3: CORTAR ---
void* thread_cortar(void* arg) {
    while(1) {
        Fruta* f = retirar_fruta(&buffer_lavagem_corte);
        simular_trabalho(ETAPA_CORTAR);
        
        // Deposita normalmente
        depositar_fruta(&buffer_corte_extracao, f);
        
        // Verifica se a fila ficou cheia após depositar
        if (fila_ficou_cheia(&buffer_corte_extracao)) {
            marcar_etapa_travada(ETAPA_CORTAR, 1);
            while (fila_ficou_cheia(&buffer_corte_extracao)) {
                usleep(50000);
            }
            marcar_etapa_travada(ETAPA_CORTAR, 0);
        }
    }
    return NULL;
}

// --- THREAD 4: EXTRAIR ---
void* thread_extrair(void* arg) {
    while(1) {
        Fruta* f = retirar_fruta(&buffer_corte_extracao);
        simular_trabalho(ETAPA_EXTRAIR);
        
        // Deposita normalmente
        depositar_fruta(&buffer_extracao_embalagem, f);
        
        // Verifica se a fila ficou cheia após depositar
        if (fila_ficou_cheia(&buffer_extracao_embalagem)) {
            marcar_etapa_travada(ETAPA_EXTRAIR, 1);
            while (fila_ficou_cheia(&buffer_extracao_embalagem)) {
                usleep(50000);
            }
            marcar_etapa_travada(ETAPA_EXTRAIR, 0);
        }
    }
    return NULL;
}

// --- THREAD 5: EMBALAR ---
void* thread_embalar(void* arg) {
    while(1) {
        // O último estágio só trava se não tiver fruta chegando
        Fruta* f = retirar_fruta(&buffer_extracao_embalagem);
        
        simular_trabalho(ETAPA_EMBALAR);
        
        adicionar_receita(); 
        fruta_concluida(); 
        
        // A fruta sai do sistema aqui
        destruir_fruta(f);
    }
    return NULL;
}