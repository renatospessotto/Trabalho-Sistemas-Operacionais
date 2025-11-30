
#include "input.h"
#include "interface.h"
#include "economia.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <pthread.h>

// Etapa selecionada no menu de upgrades
int etapa_selecionada = 0;

void mostrar_menu_upgrades() {
    bloquear_interface();  // Impede a interface de redesenhar durante o menu

    pthread_mutex_lock(&status_interface.tela_mutex);

    clear();

    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(2, 15, "=== MENU DE UPGRADES ===");
    attroff(COLOR_PAIR(1) | A_BOLD);

    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(4, 5, "Dinheiro disponível: R$ %.2f", obter_dinheiro());
    attroff(COLOR_PAIR(2) | A_BOLD);

    const char* nomes[] = {"LAVAR", "CORTAR", "EXTRAIR", "EMBALAR"};

    for(int i = 0; i < 4; i++) {
        int linha = 7 + (i * 3);

        if(i == etapa_selecionada) {
            attron(COLOR_PAIR(3) | A_BOLD);
            mvprintw(linha, 3, ">");
            attroff(COLOR_PAIR(3) | A_BOLD);
        }

        attron(COLOR_PAIR(1));
        mvprintw(linha, 5, "[%d] %s", i+1, nomes[i]);
        attroff(COLOR_PAIR(1));

        int nivel_vel = obter_nivel_velocidade(i);
        int nivel_qual = obter_nivel_qualidade(i);

        mvprintw(linha + 1, 8, "Velocidade: Nv%d (Custo: R$ %d)", 
                 nivel_vel, obter_custo_velocidade(i));

        mvprintw(linha + 2, 8, "Qualidade:  Nv%d (Custo: R$ %d)", 
                 nivel_qual, obter_custo_qualidade(i));
    }

    attron(COLOR_PAIR(5));
    mvprintw(19, 5, "INSTRUÇÕES:");
    mvprintw(20, 5, "[1-4] Selecionar etapa | [V] Velocidade | [Q] Qualidade | [ESC] Voltar");
    attroff(COLOR_PAIR(5));

    refresh();

    pthread_mutex_unlock(&status_interface.tela_mutex);
}

void processar_input_upgrades() {
    int ch;

    while (1) {
        pthread_mutex_lock(&status_interface.tela_mutex);
        ch = getch();
        pthread_mutex_unlock(&status_interface.tela_mutex);

        if(ch == 27 || ch == '\n') {
            break;
        }

        if(ch == ERR) {
            usleep(10000);
            continue;
        }

        if(ch >= '1' && ch <= '4') {
            etapa_selecionada = ch - '1';
            mostrar_menu_upgrades();
        }
        else if(ch == 'v' || ch == 'V') {
            comprar_upgrade_velocidade(etapa_selecionada);
            mostrar_menu_upgrades();
        }
        else if(ch == 'q' || ch == 'Q') {
            comprar_upgrade_qualidade(etapa_selecionada);
            mostrar_menu_upgrades();
        }
    }

    desbloquear_interface();
}

void* thread_input(void* arg) {
    int ch;

    while (1) {
        if (interface_esta_bloqueada()) {
            usleep(50000);
            continue;
        }

        pthread_mutex_lock(&status_interface.tela_mutex);
        ch = getch();
        pthread_mutex_unlock(&status_interface.tela_mutex);

        // SAIR DO JOGO
        if (ch == 'x' || ch == 'X' || ch == 'q' || ch == 'Q') {
            bloquear_interface();

            pthread_mutex_lock(&status_interface.tela_mutex);

            clear();
            attron(COLOR_PAIR(2) | A_BOLD);
            mvprintw(10, 20, "+-------------------------+");
            mvprintw(11, 20, "|    SALVANDO JOGO...     |");
            mvprintw(12, 20, "+-------------------------+");
            attroff(COLOR_PAIR(2) | A_BOLD);
            refresh();

            pthread_mutex_unlock(&status_interface.tela_mutex);

            salvar_progresso("savegame.txt");

            sleep(1);
            break;
        }

        // MENU DE UPGRADES
        if (ch == 'u' || ch == 'U') {
            mostrar_menu_upgrades();
            processar_input_upgrades();
        }
    }

    return NULL;
}
