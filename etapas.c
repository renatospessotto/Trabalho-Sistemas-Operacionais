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
        
        // 3. Tenta passar. Se CORTAR estiver cheia, TRAVA AQUI.
        // O status na tela ficará "LIVRE" (verde), mas a fila anterior vai encher
        // porque esta máquina não voltou para o passo 1.
        depositar_fruta(&buffer_lavagem_corte, f);
    }
    return NULL;
}

// --- THREAD 3: CORTAR ---
void* thread_cortar(void* arg) {
    while(1) {
        Fruta* f = retirar_fruta(&buffer_lavagem_corte);
        simular_trabalho(ETAPA_CORTAR);
        depositar_fruta(&buffer_corte_extracao, f);
    }
    return NULL;
}

// --- THREAD 4: EXTRAIR ---
void* thread_extrair(void* arg) {
    while(1) {
        Fruta* f = retirar_fruta(&buffer_corte_extracao);
        simular_trabalho(ETAPA_EXTRAIR);
        depositar_fruta(&buffer_extracao_embalagem, f);
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
        fruta_concluida(f); 
        
        // A fruta sai do sistema aqui
        destruir_fruta(f);
    }
    return NULL;
}