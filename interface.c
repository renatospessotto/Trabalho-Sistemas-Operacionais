#include "interface.h"
#include "utils.h"
#include <ncurses.h>
#include <string.h>
#include <stdio.h>

// Estado global da interface
StatusInterface status_interface = {
    .frutas_em_etapa = {0, 0, 0, 0},
    .num_maquinas = {2, 1, 1, 1}, // Valores iniciais dos semáforos
    .frutas_concluidas = 0,
    .total_frutas = MAX_FRUTAS,
    .tela_mutex = PTHREAD_MUTEX_INITIALIZER
};

// Nomes das etapas
static const char* nomes_etapas[] = {
    "LAVAR", "CORTAR", "EXTRAIR", "EMBALAR"
};

// Símbolos simples para as frutas em cada etapa
static const char* simbolos_etapas[] = {
    "*", "X", "J", "P"
};

void iniciar_tela() {
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    nodelay(stdscr, TRUE); // Para input não bloqueante
    
    // Verificar se o terminal suporta cores
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);   // Títulos
        init_pair(2, COLOR_YELLOW, COLOR_BLACK);  // Frutas processando
        init_pair(3, COLOR_CYAN, COLOR_BLACK);    // Informações
        init_pair(4, COLOR_RED, COLOR_BLACK);     // Filas
        init_pair(5, COLOR_WHITE, COLOR_BLACK);   // Normal
    }
    
    pthread_mutex_init(&status_interface.tela_mutex, NULL);
    desenhar_interface_completa();
}

void encerrar_tela() {
    pthread_mutex_destroy(&status_interface.tela_mutex);
    endwin();
}

void desenhar_interface_completa() {
    pthread_mutex_lock(&status_interface.tela_mutex);
    
    clear();
    
    // Título principal
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(0, 25, "=== FABRICA DE SUCOS ===");
    attroff(COLOR_PAIR(1) | A_BOLD);
    
    // Instruções de controle
    attron(COLOR_PAIR(3));
    mvprintw(1, 5, "Controles: [+/-] Ajustar maquinas | [1-4] Selecionar etapa | [q] Sair");
    attroff(COLOR_PAIR(3));
    
    // Desenhar cada etapa
    for (int i = 0; i < 4; i++) {
        int linha_etapa = 4 + (i * 6);
        
        // Título da etapa
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(linha_etapa, 2, "ETAPA %d - %s", i + 1, nomes_etapas[i]);
        attroff(COLOR_PAIR(1) | A_BOLD);
        
        // Número de máquinas disponíveis
        attron(COLOR_PAIR(3));
        mvprintw(linha_etapa + 1, 4, "Maquinas: %d", status_interface.num_maquinas[i]);
        attroff(COLOR_PAIR(3));
        
        // Área de processamento
        mvprintw(linha_etapa + 2, 4, "Processando: ");
        
        // Desenhar frutas em processamento
        attron(COLOR_PAIR(2));
        int frutas_processando = (status_interface.frutas_em_etapa[i] > status_interface.num_maquinas[i]) 
                                ? status_interface.num_maquinas[i] 
                                : status_interface.frutas_em_etapa[i];
        
        for (int j = 0; j < frutas_processando; j++) {
            mvprintw(linha_etapa + 2, 17 + (j * 3), "%s", simbolos_etapas[i]);
        }
        attroff(COLOR_PAIR(2));
        
        // Fila de espera (se houver)
        int fila_size = status_interface.frutas_em_etapa[i] - status_interface.num_maquinas[i];
        if (fila_size > 0) {
            attron(COLOR_PAIR(4));
            mvprintw(linha_etapa + 3, 4, "Fila de espera: ");
            for (int j = 0; j < fila_size; j++) {
                mvprintw(linha_etapa + 3, 20 + (j * 2), ".");
            }
            attroff(COLOR_PAIR(4));
        } else {
            mvprintw(linha_etapa + 3, 4, "Fila de espera: vazia");
        }
        
        // Linha separadora
        mvprintw(linha_etapa + 4, 0, "--------------------------------------------------------");
    }
    
    // Estatísticas finais
    int linha_stats = 4 + (4 * 6) + 1;
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(linha_stats, 2, "ESTATISTICAS");
    attroff(COLOR_PAIR(1) | A_BOLD);
    
    attron(COLOR_PAIR(3));
    mvprintw(linha_stats + 1, 4, "Frutas concluidas: %d/%d", 
             status_interface.frutas_concluidas, status_interface.total_frutas);
    
    // Barra de progresso simples
    int progresso = (status_interface.frutas_concluidas * 20) / status_interface.total_frutas;
    mvprintw(linha_stats + 2, 4, "Progresso: [");
    for (int i = 0; i < 20; i++) {
        if (i < progresso) {
            addch('#');
        } else {
            addch('-');
        }
    }
    addstr("]");
    attroff(COLOR_PAIR(3));
    
    refresh();
    pthread_mutex_unlock(&status_interface.tela_mutex);
}

void marcar_fruta_entrando_etapa(int fruta_id, Etapa etapa) {
    pthread_mutex_lock(&status_interface.tela_mutex);
    status_interface.frutas_em_etapa[etapa]++;
    
    // Atualizar apenas a seção relevante da tela
    int linha_etapa = 4 + (etapa * 6);
    
    // Limpar linha de processamento
    mvprintw(linha_etapa + 2, 17, "                    ");
    
    // Redesenhar frutas em processamento
    attron(COLOR_PAIR(2));
    int frutas_processando = (status_interface.frutas_em_etapa[etapa] > status_interface.num_maquinas[etapa]) 
                            ? status_interface.num_maquinas[etapa] 
                            : status_interface.frutas_em_etapa[etapa];
    
    for (int j = 0; j < frutas_processando; j++) {
        mvprintw(linha_etapa + 2, 17 + (j * 3), "%s", simbolos_etapas[etapa]);
    }
    attroff(COLOR_PAIR(2));
    
    // Atualizar fila de espera
    int fila_size = status_interface.frutas_em_etapa[etapa] - status_interface.num_maquinas[etapa];
    mvprintw(linha_etapa + 3, 4, "                              "); // Limpar linha
    if (fila_size > 0) {
        attron(COLOR_PAIR(4));
        mvprintw(linha_etapa + 3, 4, "Fila de espera: ");
        for (int j = 0; j < fila_size; j++) {
            mvprintw(linha_etapa + 3, 20 + (j * 2), ".");
        }
        attroff(COLOR_PAIR(4));
    } else {
        mvprintw(linha_etapa + 3, 4, "Fila de espera: vazia");
    }
    
    refresh();
    pthread_mutex_unlock(&status_interface.tela_mutex);
}

void marcar_fruta_saindo_etapa(int fruta_id, Etapa etapa) {
    pthread_mutex_lock(&status_interface.tela_mutex);
    if (status_interface.frutas_em_etapa[etapa] > 0) {
        status_interface.frutas_em_etapa[etapa]--;
    }
    
    // Atualizar apenas a seção relevante da tela
    int linha_etapa = 4 + (etapa * 6);
    
    // Limpar linha de processamento
    mvprintw(linha_etapa + 2, 17, "                    ");
    
    // Redesenhar frutas em processamento
    attron(COLOR_PAIR(2));
    int frutas_processando = (status_interface.frutas_em_etapa[etapa] > status_interface.num_maquinas[etapa]) 
                            ? status_interface.num_maquinas[etapa] 
                            : status_interface.frutas_em_etapa[etapa];
    
    for (int j = 0; j < frutas_processando; j++) {
        mvprintw(linha_etapa + 2, 17 + (j * 3), "%s", simbolos_etapas[etapa]);
    }
    attroff(COLOR_PAIR(2));
    
    // Atualizar fila de espera
    int fila_size = status_interface.frutas_em_etapa[etapa] - status_interface.num_maquinas[etapa];
    mvprintw(linha_etapa + 3, 4, "                              "); // Limpar linha
    if (fila_size > 0) {
        attron(COLOR_PAIR(4));
        mvprintw(linha_etapa + 3, 4, "Fila de espera: ");
        for (int j = 0; j < fila_size; j++) {
            mvprintw(linha_etapa + 3, 20 + (j * 2), ".");
        }
        attroff(COLOR_PAIR(4));
    } else {
        mvprintw(linha_etapa + 3, 4, "Fila de espera: vazia");
    }
    
    refresh();
    pthread_mutex_unlock(&status_interface.tela_mutex);
}

void fruta_concluida(Fruta* f) {
    pthread_mutex_lock(&status_interface.tela_mutex);
    status_interface.frutas_concluidas++;
    
    // Atualizar estatísticas
    int linha_stats = 4 + (4 * 6) + 1;
    attron(COLOR_PAIR(3));
    mvprintw(linha_stats + 1, 4, "Frutas concluidas: %d/%d", 
             status_interface.frutas_concluidas, status_interface.total_frutas);
    
    // Atualizar barra de progresso
    int progresso = (status_interface.frutas_concluidas * 20) / status_interface.total_frutas;
    mvprintw(linha_stats + 2, 4, "Progresso: [");
    for (int i = 0; i < 20; i++) {
        if (i < progresso) {
            addch('#');
        } else {
            addch('-');
        }
    }
    addstr("]");
    attroff(COLOR_PAIR(3));
    
    // Mostrar notificação temporária de fruta concluída
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(linha_stats + 4, 4, "*** Fruta %d concluida! ***", f->id);
    attroff(COLOR_PAIR(1) | A_BOLD);
    
    refresh();
    pthread_mutex_unlock(&status_interface.tela_mutex);
}

void atualizar_maquinas_etapa(Etapa etapa, int delta) {
    pthread_mutex_lock(&status_interface.tela_mutex);
    status_interface.num_maquinas[etapa] += delta;
    
    // Garantir que não fique negativo
    if (status_interface.num_maquinas[etapa] < 0) {
        status_interface.num_maquinas[etapa] = 0;
    }
    
    // Atualizar display da etapa
    int linha_etapa = 4 + (etapa * 6);
    attron(COLOR_PAIR(3));
    mvprintw(linha_etapa + 1, 4, "Maquinas: %d  ", status_interface.num_maquinas[etapa]);
    attroff(COLOR_PAIR(3));
    
    // Atualizar fila de espera também
    int fila_size = status_interface.frutas_em_etapa[etapa] - status_interface.num_maquinas[etapa];
    mvprintw(linha_etapa + 3, 4, "                              ");
    if (fila_size > 0) {
        attron(COLOR_PAIR(4));
        mvprintw(linha_etapa + 3, 4, "Fila de espera: ");
        for (int j = 0; j < fila_size; j++) {
            mvprintw(linha_etapa + 3, 20 + (j * 2), ".");
        }
        attroff(COLOR_PAIR(4));
    } else {
        mvprintw(linha_etapa + 3, 4, "Fila de espera: vazia");
    }
    
    refresh();
    pthread_mutex_unlock(&status_interface.tela_mutex);
}

void desenhar_fila_espera(Etapa etapa, int tamanho_fila) {
    pthread_mutex_lock(&status_interface.tela_mutex);
    int linha_etapa = 4 + (etapa * 6);
    
    mvprintw(linha_etapa + 3, 4, "                              ");
    if (tamanho_fila > 0) {
        attron(COLOR_PAIR(4));
        mvprintw(linha_etapa + 3, 4, "Fila de espera: ");
        for (int j = 0; j < tamanho_fila; j++) {
            mvprintw(linha_etapa + 3, 20 + (j * 2), ".");
        }
        attroff(COLOR_PAIR(4));
    } else {
        mvprintw(linha_etapa + 3, 4, "Fila de espera: vazia");
    }
    
    refresh();
    pthread_mutex_unlock(&status_interface.tela_mutex);
}

void atualizar_status() {
    desenhar_interface_completa();
}

// Função auxiliar para compatibilidade com código antigo
void desenhar_fruta_em_etapa(int fruta_id, Etapa etapa) {
    marcar_fruta_entrando_etapa(fruta_id, etapa);
}

void desenhar_saida_fruta_etapa(int fruta_id, Etapa etapa) {
    marcar_fruta_saindo_etapa(fruta_id, etapa);
}