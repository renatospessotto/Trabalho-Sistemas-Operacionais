#ifndef UPGRADES_H
#define UPGRADES_H

typedef struct {
    int nivel_colheita;
    int nivel_lavagem;
    int nivel_corte;
    int nivel_extracao;
    int nivel_embalagem;

    int custo_colheita;
    int custo_lavagem;
    int custo_corte;
    int custo_extracao;
    int custo_embalagem;

} Upgrades;

extern Upgrades upgrades;

void init_upgrades();
int tentar_upgrade_colheita();
int tentar_upgrade_lavagem();
int tentar_upgrade_corte();
int tentar_upgrade_extracao();
int tentar_upgrade_embalagem();

#endif
