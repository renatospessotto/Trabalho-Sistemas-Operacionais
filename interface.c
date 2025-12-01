#include "interface.h"
#include "etapas.h"
#include "economia.h"
#include "buffers.h"
#include "config.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <time.h> // Para time() e difftime()

StatusInterface status_interface = {
    .frutas_concluidas = 0,
    .tela_mutex = PTHREAD_MUTEX_INITIALIZER,
    .interface_bloqueada = 0,
    .tempo_restante = {0.0f, 0.0f, 0.0f, 0.0f},
    .etapa_travada = {0, 0, 0, 0},
    .tempo_mutex = PTHREAD_MUTEX_INITIALIZER,
    
    // Inicialização da TPS
    .tps_concluidas_anterior = 0,
    .tps_ultima_medicao = 0,
    .tps_atual = 0.0f
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
    // Limpeza de novos mutexes, se houver
    pthread_mutex_destroy(&status_interface.tempo_mutex); 
    endwin();
}

void bloquear_interface() { status_interface.interface_bloqueada = 1; }
void desbloquear_interface() { status_interface.interface_bloqueada = 0; }
int interface_esta_bloqueada() { return status_interface.interface_bloqueada; }

// Função para calcular a Taxa de Produção por Segundo (TPS)
static void calcular_tps() {
    pthread_mutex_lock(&status_interface.tempo_mutex);
    
    time_t agora = time(NULL);
    
    if (status_interface.tps_ultima_medicao == 0) {
        status_interface.tps_ultima_medicao = agora;
        status_interface.tps_concluidas_anterior = status_interface.frutas_concluidas;
    }
    
    double delta_tempo = difftime(agora, status_interface.tps_ultima_medicao);
    
    // Atualiza a TPS a cada 2 segundos para ter uma leitura mais estável
    if (delta_tempo >= 2.0) {
        int delta_frutas = status_interface.frutas_concluidas - status_interface.tps_concluidas_anterior;
        
        if (delta_tempo > 0.0) {
            status_interface.tps_atual = (float)delta_frutas / (float)delta_tempo;
        } else {
            status_interface.tps_atual = 0.0f;
        }
        
        // Reset para a próxima medição
        status_interface.tps_ultima_medicao = agora;
        status_interface.tps_concluidas_anterior = status_interface.frutas_concluidas;
    }

    pthread_mutex_unlock(&status_interface.tempo_mutex);
}

// Desenha a animação do buffer
static void desenhar_animacao_buffer(int linha_base, BufferCircular* b) {
    int linha_desenho = linha_base; 

    // Limpa a área da esteira (da Coluna 40 em diante) antes de desenhar
    move(linha_desenho, 40);
    clrtoeol(); 

    pthread_mutex_lock(&b->mutex); 
    
    int fila_real = b->count;
    int pos = b->out; 
    
    // Desenha o background da "esteira" vazia (por exemplo, 20 colunas de '·')
    attron(COLOR_PAIR(5));
    mvprintw(linha_desenho, 40, "...................."); // Coluna 40 é o início da animação
    attroff(COLOR_PAIR(5));
    
    // Desenha as frutas
    if (fila_real > 0) {
        attron(COLOR_PAIR(3) | A_BOLD); // Cor Cyan
        for(int i = 0; i < fila_real; i++) {
            // Coluna de desenho: 40 (início) + (índice do slot * espaço por slot)
            // Usa 19 colunas para desenhar, garantindo espaço entre
            int coluna_desenho = 40 + (pos * (19 / TAMANHO_BUFFER)); 
            mvprintw(linha_desenho, coluna_desenho, "█"); // Fruta como um bloco
            
            // Move para o próximo índice circular
            pos = (pos + 1) % TAMANHO_BUFFER;
        }
        attroff(COLOR_PAIR(3) | A_BOLD);
    }

    pthread_mutex_unlock(&b->mutex);
}


static void desenhar_conteudo_interface() {
    // 1. Cálculo de estatísticas
    calcular_tps(); 

    // Cabeçalho
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(0, 15, "=== FABRICA DE SUCOS ===");
    clrtoeol();
    attroff(COLOR_PAIR(1) | A_BOLD);

    // HUD Dinheiro / Frutas / NOVO: TPS
    attron(COLOR_PAIR(2));
    mvprintw(1, 2, "Dinheiro: R$ %.2f | Sucos Prontos: %d (%.2f TPS) | Valor/Suco: R$ %.2f",
             obter_dinheiro(),
             status_interface.frutas_concluidas,
             status_interface.tps_atual, // Exibe o TPS
             calcular_valor_produto());
    clrtoeol();
    attroff(COLOR_PAIR(2));

    extern int etapa_selecionada;
    for (int i = 0; i < 4; i++) {
        // Espaçamento corrigido (i * 5)
        int linha = 4 + (i * 5);

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
        BufferCircular* buffer_atual;
        switch(i) {
            case 0: 
                buffer_atual = &buffer_colheita_lavagem;
                fila_real = obter_tamanho_fila(buffer_atual); 
                break;
            case 1: 
                buffer_atual = &buffer_lavagem_corte;
                fila_real = obter_tamanho_fila(buffer_atual); 
                break;
            case 2: 
                buffer_atual = &buffer_corte_extracao;
                fila_real = obter_tamanho_fila(buffer_atual); 
                break;
            case 3: 
                buffer_atual = &buffer_extracao_embalagem;
                fila_real = obter_tamanho_fila(buffer_atual); 
                break;
            default:
                buffer_atual = NULL;
        }
        
        // CÁLCULO DE PROGRESSO (Tempo Total - Tempo Restante)
        float tempo_decorrido = obter_tempo_etapa_calculado(i) - tempo_rest;
        if (tempo_decorrido < 0.0f) tempo_decorrido = 0.0f; // Evita valor negativo

        // Status visual
        if (esta_travada) {
            attron(COLOR_PAIR(4) | A_BOLD);
            mvprintw(linha + 2, 4, "Status: TRAVADO (FILA SEGUINTE CHEIA)");
            attroff(COLOR_PAIR(4) | A_BOLD);
        } else if (tempo_rest > 0.0f) {
            attron(COLOR_PAIR(2));
            // Mostra o tempo DECORRIDO
            mvprintw(linha + 2, 4, "Status: PROCESSANDO [%.1fs]", tempo_decorrido); 
            attroff(COLOR_PAIR(2));
        } else {
            mvprintw(linha + 2, 4, "Status: LIVRE");
        }
        clrtoeol();
        
        // Fila visual 
        move(linha + 3, 0); 
        clrtoeol();
        
        if (fila_real >= TAMANHO_BUFFER) attron(COLOR_PAIR(4));
        mvprintw(linha + 3, 4, "Fila de Entrada: [%d / %d] itens", fila_real, TAMANHO_BUFFER);
        if (fila_real >= TAMANHO_BUFFER) attroff(COLOR_PAIR(4));
        
        // Desenha a animação!
        if (buffer_atual != NULL) {
            desenhar_animacao_buffer(linha + 3, buffer_atual);
        }
        
        clrtoeol(); // Limpa o restante da linha
    }

   
    attron(COLOR_PAIR(5));
    mvprintw(25, 2, "Controles: [1-4]Selecionar | [U]pgrades | [Q]Sair");
    clrtoeol();
    attroff(COLOR_PAIR(5));
}

void desenhar_interface_completa() {
    if (interface_esta_bloqueada()) return;

    pthread_mutex_lock(&status_interface.tela_mutex);
    erase();
    desenhar_conteudo_interface();
    refresh();
    pthread_mutex_unlock(&status_interface.tela_mutex);
}

void* thread_atualizador_interface(void*) {
    while(1) {
        desenhar_interface_completa();
        usleep(100000); // 10 FPS
    }
    return NULL;
}

// Setters seguros
void iniciar_processamento_etapa(int etapa, float tempo_total) {
    pthread_mutex_lock(&status_interface.tempo_mutex);
    status_interface.tempo_restante[etapa] = tempo_total; // Tempo restante é o tempo total no início
    pthread_mutex_unlock(&status_interface.tempo_mutex);
}

void atualizar_tempo_etapa(int etapa, float tempo_decorrido) {
    pthread_mutex_lock(&status_interface.tempo_mutex);
    // Tempo restante é o tempo total menos o tempo decorrido
    float tempo_total = obter_tempo_etapa_calculado(etapa);
    status_interface.tempo_restante[etapa] = tempo_total - tempo_decorrido; 
    pthread_mutex_unlock(&status_interface.tempo_mutex);
}

void finalizar_processamento_etapa(int etapa) {
    pthread_mutex_lock(&status_interface.tempo_mutex);
    status_interface.tempo_restante[etapa] = 0.0f; // Marca como livre/ocioso
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

// Função para obter o tempo total real da etapa (sem mutex)
float obter_tempo_etapa_calculado(int etapa) {
    // Chama a função da economia que já faz o cálculo com upgrades
    return calcular_tempo_etapa(etapa); 
}