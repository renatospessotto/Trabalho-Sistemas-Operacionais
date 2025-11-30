#include "upgrades.h"
#include "economia.h"

Upgrades upgrades;

void init_upgrades() {
    upgrades.nivel_colheita = 1;
    upgrades.nivel_lavagem  = 1;
    upgrades.nivel_extracao = 1;
    upgrades.nivel_envase   = 1;

    upgrades.custo[0] = 20;
    upgrades.custo[1] = 30;
    upgrades.custo[2] = 50;
    upgrades.custo[3] = 70;
}

int tentar_upgrade(int etapa) {
    int custo = upgrades.custo[etapa];

    if (obter_dinheiro() < custo)
        return 0;

    registrar_receita(-custo);

    switch (etapa) {
        case 0: upgrades.nivel_colheita++; break;
        case 1: upgrades.nivel_lavagem++; break;
        case 2: upgrades.nivel_extracao++; break;
        case 3: upgrades.nivel_envase++; break;
    }
    return 1;
}
