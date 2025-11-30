#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "config.h"
#include "buffers.h"
#include "etapas.h"
#include "interface.h"
#include "input.h"
#include "economia.h"



int main() {
    // 1. Inicializações
    inicializar_economia();
    init_interface();
    
    // Inicializar Buffers (CRÍTICO)
    init_buffer(&buffer_colheita_lavagem);
    init_buffer(&buffer_lavagem_corte);
    init_buffer(&buffer_corte_extracao);
    init_buffer(&buffer_extracao_embalagem);

    // 2. Criar Threads
    pthread_t t_input, t_interface;
    pthread_t t_colheita, t_lavar, t_cortar, t_extrair, t_embalar;

    // Interface e Input
    pthread_create(&t_input, NULL, thread_input, NULL);
    // Thread dedicada para atualizar a tela (FPS)
    pthread_create(&t_interface, NULL, thread_atualizador_interface, NULL);

    // Operários da Fábrica (Pipeline)
    pthread_create(&t_colheita, NULL, thread_colheita, NULL);
    pthread_create(&t_lavar, NULL, thread_lavar, NULL);
    pthread_create(&t_cortar, NULL, thread_cortar, NULL);
    pthread_create(&t_extrair, NULL, thread_extrair, NULL);
    pthread_create(&t_embalar, NULL, thread_embalar, NULL);

    // 3. Loop principal (espera o input sair)
    pthread_join(t_input, NULL);

    // 4. Limpeza (Ao sair do jogo)
    // Cancelar threads
    pthread_cancel(t_colheita);
    pthread_cancel(t_lavar);
    pthread_cancel(t_cortar);
    pthread_cancel(t_extrair);
    pthread_cancel(t_embalar);
    pthread_cancel(t_interface);

    salvar_progresso("savegame.txt");
    
    destroy_buffer(&buffer_colheita_lavagem);
    destroy_buffer(&buffer_lavagem_corte);
    destroy_buffer(&buffer_corte_extracao);
    destroy_buffer(&buffer_extracao_embalagem);
    
    cleanup_interface();
    finalizar_economia();

    return 0;
}
