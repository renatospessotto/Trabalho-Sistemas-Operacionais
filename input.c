#include "input.h"
#include "interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

Etapa etapa_selecionada = LAVAR; // Etapa inicial selecionada

void mostrar_etapa_selecionada() {
    pthread_mutex_lock(&status_interface.tela_mutex);
    
    // Limpar indicador anterior
    for (int i = 0; i < 4; i++) {
        int linha = 4 + (i * 6);
        mvprintw(linha, 0, " ");
    }
    
    // Mostrar indicador na etapa selecionada
    int linha_selecionada = 4 + (etapa_selecionada * 6);
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(linha_selecionada, 0, ">");
    attroff(COLOR_PAIR(1) | A_BOLD);
    
    // Atualizar instruções
    attron(COLOR_PAIR(3));
    mvprintw(2, 5, "Etapa selecionada: %d (%s) | [+/-] Ajustar | [1-4] Trocar etapa | [q] Sair   ", 
             etapa_selecionada + 1, 
             (etapa_selecionada == LAVAR) ? "LAVAR" :
             (etapa_selecionada == CORTAR) ? "CORTAR" :
             (etapa_selecionada == EXTRAIR) ? "EXTRAIR" : "EMBALAR");
    attroff(COLOR_PAIR(3));
    
    refresh();
    pthread_mutex_unlock(&status_interface.tela_mutex);
}

void* thread_input(void* arg) {
    int ch;
    
    // Mostrar etapa selecionada inicial
    sleep(1); // Dar tempo para a interface se inicializar
    mostrar_etapa_selecionada();
    
    while ((ch = getch()) != 'q') {
        switch (ch) {
            case '+':
                // Incrementar máquinas na etapa selecionada
                incrementar_maquinas_etapa(etapa_selecionada);
                break;
                
            case '-':
                // Decrementar máquinas na etapa selecionada
                decrementar_maquinas_etapa(etapa_selecionada);
                break;
                
            case '1':
                etapa_selecionada = LAVAR;
                mostrar_etapa_selecionada();
                break;
                
            case '2':
                etapa_selecionada = CORTAR;
                mostrar_etapa_selecionada();
                break;
                
            case '3':
                etapa_selecionada = EXTRAIR;
                mostrar_etapa_selecionada();
                break;
                
            case '4':
                etapa_selecionada = EMBALAR;
                mostrar_etapa_selecionada();
                break;
                
            case 'r':
            case 'R':
                // Redesenhar tela completa
                desenhar_interface_completa();
                mostrar_etapa_selecionada();
                break;
                
            default:
                // Ignorar outras teclas
                break;
        }
        
        // Pequena pausa para evitar input muito rápido
        usleep(50000); // 50ms
    }
    
    endwin();
    exit(0);
}
