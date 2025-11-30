#include "upgrades.h"
#include "economia.h"

Upgrades upgrades;

void init_upgrades() {
    upgrades.nivel_colheita  = 1;
    upgrades.nivel_lavagem   = 1;
    upgrades.nivel_corte     = 1;
    upgrades.nivel_extracao  = 1;
    upgrades.nivel_embalagem = 1;

    upgrades.custo_colheita  = 20;
    upgrades.custo_lavagem   = 30;
    upgrades.custo_corte     = 35;
    upgrades.custo_extracao  = 45;
    upgrades.custo_embalagem = 60;
}

int tentar_upgrade_colheita() {
    if (get_dinheiro() < upgrades.custo_colheita) return 0;
    add_dinheiro(-upgrades.custo_colheita);
    upgrades.nivel_colheita++;
    upgrades.custo_colheita += 15;
    return 1;
}

int tentar_upgrade_lavagem() {
    if (get_dinheiro() < upgrades.custo_lavagem) return 0;
    add_dinheiro(-upgrades.custo_lavagem);
    upgrades.nivel_lavagem++;
    upgrades.custo_lavagem += 20;
    return 1;
}

int tentar_upgrade_corte() {
    if (get_dinheiro() < upgrades.custo_corte) return 0;
    add_dinheiro(-upgrades.custo_corte);
    upgrades.nivel_corte++;
    upgrades.custo_corte += 20;
    return 1;
}

int tentar_upgrade_extracao() {
    if (get_dinheiro() < upgrades.custo_extracao) return 0;
    add_dinheiro(-upgrades.custo_extracao);
    upgrades.nivel_extracao++;
    upgrades.custo_extracao += 25;
    return 1;
}

int tentar_upgrade_embalagem() {
    if (get_dinheiro() < upgrades.custo_embalagem) return 0;
    add_dinheiro(-upgrades.custo_embalagem);
    upgrades.nivel_embalagem++;
    upgrades.custo_embalagem += 30;
