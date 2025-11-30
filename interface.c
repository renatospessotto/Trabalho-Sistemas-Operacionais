#include "interface.h"
#include "etapas.h"
#include "economia.h"
#include "buffers.h"
#include "config.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>

StatusInterface status_interface = {
    .frutas_concluidas = 0,
    .tela_mutex = PTHREAD_MUTEX_INITIALIZER,
    .interface_bloqueada = 0,
    .tempo_restante = {0.0f, 0.0f, 0.0f, 0.0f},
    .etapa_travada = {0, 0, 0, 0},
    .tempo_mutex = PTHREAD_MUTEX_INITIALIZER
};

static const char* nomes_etapas[] = {"LAVAR", "CORTAR", "EXTRAIR", "EMBALAR"};

void init_interface() {
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    nodelay(stdscr, TRUE); // input não-bloqueante

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);
        init_pair(2, COLOR_YELLOW, COLOR_BLACK);
        init_pair(3, COLOR_CYAN, COLOR_BLACK);
        init_pair(4, COLOR_RED, COLOR_BLACK);
        init_pair(5, COLOR_WHITE, COLOR_BLACK);
    }
}

void cleanup_interface() {
    pthread_mutex_destroy(&status_interface.tela_mutex);
    endwin();
}

void bloquear_interface() { status_interface.interface_bloqueada = 1; }
void desbloquear_interface() { status_interface.interface_bloqueada = 0; }
int interface_esta_bloqueada() { return status_interface.interface_bloqueada; }

static void desenhar_conteudo_interface() {
    // Cabeçalho
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(0, 15, "=== FABRICA DE SUCOS ===");
    clrtoeol();
    attroff(COLOR_PAIR(1) | A_BOLD);

    // HUD Dinheiro / Frutas
    attron(COLOR_PAIR(2));
    mvprintw(1, 2, "Dinheiro: R$ %.2f | Sucos Prontos: %d | Valor/Suco: R$ %.2f",
             obter_dinheiro(),
             status_interface.frutas_concluidas,
             calcular_valor_produto());
    clrtoeol();
    attroff(COLOR_PAIR(2));

    extern int etapa_selecionada;
    for (int i = 0; i < 4; i++) {
        int linha = 4 + (i * 4);

        // Cursor de seleção
        if (i == etapa_selecionada) {
            attron(COLOR_PAIR(3) | A_BOLD);
            mvprintw(linha, 0, ">");
            attroff(COLOR_PAIR(3) | A_BOLD);
        } else mvprintw(linha, 0, " ");

        // Nome da etapa
        attron(COLOR_PAIR(1));
        mvprintw(linha, 2, "ETAPA %d - %s", i + 1, nomes_etapas[i]);
        clrtoeol();
        attroff(COLOR_PAIR(1));

        // Upgrades
        mvprintw(linha + 1, 4, "Upgrades: Velocidade Nv%d | Qualidade Nv%d",
                 obter_nivel_velocidade(i), obter_nivel_qualidade(i));
        clrtoeol();

        // Tempo restante e estado de travamento
        pthread_mutex_lock(&status_interface.tempo_mutex);
        float tempo_rest = status_interface.tempo_restante[i];
        int esta_travada = status_interface.etapa_travada[i];
        pthread_mutex_unlock(&status_interface.tempo_mutex);

        // Fila real
        int fila_real = 0;
        switch(i) {
            case 0: fila_real = obter_tamanho_fila(&buffer_colheita_lavagem); break;
            case 1: fila_real = obter_tamanho_fila(&buffer_lavagem_corte); break;
            case 2: fila_real = obter_tamanho_fila(&buffer_corte_extracao); break;
            case 3: fila_real = obter_tamanho_fila(&buffer_extracao_embalagem); break;
        }

        // Status visual
        if (esta_travada) {
            attron(COLOR_PAIR(4) | A_BOLD);
            mvprintw(linha + 2, 4, "Status: TRAVADO (FILA SEGUINTE CHEIA)");
            attroff(COLOR_PAIR(4) | A_BOLD);
        } else if (tempo_rest > 0.0f) {
            attron(COLOR_PAIR(2));
            mvprintw(linha + 2, 4, "Status: PROCESSANDO [%.1fs]", tempo_rest);
            attroff(COLOR_PAIR(2));
        } else {
            mvprintw(linha + 2, 4, "Status: LIVRE");
        }
        clrtoeol();

        // Fila visual
        if (fila_real >= TAMANHO_BUFFER) attron(COLOR_PAIR(4));
        mvprintw(linha + 3, 4, "Fila de Entrada: [%d / %d] itens", fila_real, TAMANHO_BUFFER);
        if (fila_real >= TAMANHO_BUFFER) attroff(COLOR_PAIR(4));
        clrtoeol();
    }

    attron(COLOR_PAIR(5));
    mvprintw(20, 2, "Controles: [1-4]Selecionar | [U]pgrades | [Q]Sair");
    clrtoeol();
    attroff(COLOR_PAIR(5));
}

void desenhar_interface_completa() {
    if (interface_esta_bloqueada()) return;

    pthread_mutex_lock(&status_interface.tela_mutex);
    desenhar_conteudo_interface();
    refresh();
    pthread_mutex_unlock(&status_interface.tela_mutex);
}

void* thread_atualizador_interface(void* arg) {
    while(1) {
        desenhar_interface_completa();
        usleep(100000); // 10 FPS
    }
    return NULL;
}

// Setters seguros para tempo e frutas
void iniciar_processamento_etapa(int etapa, float tempo_total) {
    pthread_mutex_lock(&status_interface.tempo_mutex);
    status_interface.tempo_restante[etapa] = tempo_total;
    pthread_mutex_unlock(&status_interface.tempo_mutex);
}

void atualizar_tempo_etapa(int etapa, float tempo_decorrido) {
    pthread_mutex_lock(&status_interface.tempo_mutex);
    status_interface.tempo_restante[etapa] = tempo_decorrido;
    pthread_mutex_unlock(&status_interface.tempo_mutex);
}

void finalizar_processamento_etapa(int etapa) {
    pthread_mutex_lock(&status_interface.tempo_mutex);
    status_interface.tempo_restante[etapa] = 0.0f;
    pthread_mutex_unlock(&status_interface.tempo_mutex);
}



void marcar_etapa_travada(int etapa, int travada) {
    pthread_mutex_lock(&status_interface.tempo_mutex);
    status_interface.etapa_travada[etapa] = travada;
    pthread_mutex_unlock(&status_interface.tempo_mutex);
}

void fruta_concluida() {
    pthread_mutex_lock(&status_interface.tempo_mutex);
    status_interface.frutas_concluidas++;
    pthread_mutex_unlock(&status_interface.tempo_mutex);
}
