#include "input.h"
#include "interface.h"
#include "economia.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <pthread.h>
#include <string.h> // NOVO: Necessário para usar sprintf/strcpy

// Etapa selecionada no menu de upgrades
int etapa_selecionada = 0;
// NOVO: Variável global para mensagens de feedback
char feedback_mensagem[100] = ""; 

static const char* nomes[] = {"LAVAR", "CORTAR", "EXTRAIR", "EMBALAR"};

void mostrar_menu_upgrades() {
    bloquear_interface();  // Impede a interface de redesenhar durante o menu

    pthread_mutex_lock(&status_interface.tela_mutex);

    clear();
    refresh();

    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(2, 15, "=== MENU DE UPGRADES ===");
    attroff(COLOR_PAIR(1) | A_BOLD);

    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(4, 5, "Dinheiro disponível: R$ %.2f", obter_dinheiro());
    attroff(COLOR_PAIR(2) | A_BOLD);
    
    // NOVO: Exibe a mensagem de feedback
    if (feedback_mensagem[0] != '\0') {
        attron(COLOR_PAIR(4) | A_BOLD); // Vermelho para feedback/erro
        mvprintw(6, 5, ">> %s <<", feedback_mensagem);
        attroff(COLOR_PAIR(4) | A_BOLD);
        // Limpa a mensagem após exibir
        strcpy(feedback_mensagem, ""); 
    }
    // FIM NOVO

    for(int i = 0; i < 4; i++) {
        int linha = 8 + (i * 3); // Linhas ajustadas para dar espaço ao feedback

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
    mvprintw(21, 5, "INSTRUÇÕES:"); // Linhas ajustadas
    mvprintw(22, 5, "[1-4] Selecionar etapa | [V] Velocidade | [Q] Qualidade | [ESC] Voltar");
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
            // NOVO: Processa a compra e define a mensagem de feedback
            if(comprar_upgrade_velocidade(etapa_selecionada)) {
                sprintf(feedback_mensagem, "SUCESSO: Velocidade de %s melhorada!", nomes[etapa_selecionada]);
            } else {
                sprintf(feedback_mensagem, "ERRO: Nível máximo ou Dinheiro Insuficiente!");
            }
            mostrar_menu_upgrades();
        }
        else if(ch == 'q' || ch == 'Q') {
            // NOVO: Processa a compra e define a mensagem de feedback
            if(comprar_upgrade_qualidade(etapa_selecionada)) {
                sprintf(feedback_mensagem, "SUCESSO: Qualidade de %s melhorada!", nomes[etapa_selecionada]);
            } else {
                sprintf(feedback_mensagem, "ERRO: Nível máximo ou Dinheiro Insuficiente!");
            }
            mostrar_menu_upgrades();
        }
    }

    desbloquear_interface();
}

void* thread_input(void*) {
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
            // Garante que a mensagem de feedback não apareça no menu principal
            strcpy(feedback_mensagem, ""); 
            mostrar_menu_upgrades();
            processar_input_upgrades();
        }
        
        // Seleção de etapa (para o menu principal)
        if (ch >= '1' && ch <= '4') {
            extern int etapa_selecionada;
            etapa_selecionada = ch - '1';
        }
    }

    return NULL;
}