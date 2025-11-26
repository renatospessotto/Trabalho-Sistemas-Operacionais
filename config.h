#ifndef CONFIG_H
#define CONFIG_H

// --- CONFIGURAÇÕES GERAIS ---
#define TAMANHO_BUFFER 5      // Mantenha baixo para ver as filas enchendo!
#define MAX_FRUTAS_TOTAL 100  

// --- SISTEMA ECONÔMICO ---
#define RECEITA_BASE 5.0f          
#define MULTIPLICADOR_CUSTO 2.5f   

// --- TEMPOS BASE (segundos) ---
#define TEMPO_BASE_COLHEITA 1.0f   
#define TEMPO_BASE_LAVAGEM  2.5f   
#define TEMPO_BASE_EXTRACAO 4.0f   // Gargalo: Filas vão acumular antes daqui
#define TEMPO_BASE_EMBALAGEM 5.5f  

// --- CUSTOS ---
#define CUSTO_INICIAL_VEL_COLHEITA 50
#define CUSTO_INICIAL_QUAL_COLHEITA 75
#define CUSTO_INICIAL_VEL_LAVAGEM 60
#define CUSTO_INICIAL_QUAL_LAVAGEM 80
#define CUSTO_INICIAL_VEL_EXTRACAO 80
#define CUSTO_INICIAL_QUAL_EXTRACAO 120
#define CUSTO_INICIAL_VEL_EMBALAGEM 70
#define CUSTO_INICIAL_QUAL_EMBALAGEM 150

#endif