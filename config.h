#ifndef CONFIG_H
#define CONFIG_H

// --- CONFIGURAÇÕES GERAIS ---
#define TAMANHO_BUFFER 10     // Tamanho das filas entre etapas
  

// --- SISTEMA ECONÔMICO ---
#define RECEITA_BASE 8.0f          // Receita base por suco          
#define MULTIPLICADOR_CUSTO 1.4f   // Crescimento mais suave   

// --- TEMPOS BASE (segundos) ---
#define TEMPO_BASE_COLHEITA 1.5f   // Começa rápido para alimentar sistema
#define TEMPO_BASE_LAVAGEM 2.0f    // Mais lento no início
#define TEMPO_BASE_CORTAR  3.0f    
#define TEMPO_BASE_EXTRACAO 3.5f   // Gargalo reduzido
#define TEMPO_BASE_EMBALAGEM 4.0f  // Mais rápido para evitar travamento  

// --- CUSTOS ---
#define CUSTO_INICIAL_VEL_LAVAGEM 25
#define CUSTO_INICIAL_QUAL_LAVAGEM 35
#define CUSTO_INICIAL_VEL_CORTAR 30
#define CUSTO_INICIAL_QUAL_CORTAR 40
#define CUSTO_INICIAL_VEL_EXTRACAO 40
#define CUSTO_INICIAL_QUAL_EXTRACAO 55
#define CUSTO_INICIAL_VEL_EMBALAGEM 35
#define CUSTO_INICIAL_QUAL_EMBALAGEM 60

#endif