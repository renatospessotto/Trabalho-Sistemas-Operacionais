#ifndef UPGRADES_H
#define UPGRADES_H

typedef struct {
    int nivel_colheita;
    int nivel_lavagem;
    int nivel_extracao;
    int nivel_envase;
    int custo[4];
} Upgrades;

extern Upgrades upgrades;

void init_upgrades();
int tentar_upgrade(int etapa);

#endif
