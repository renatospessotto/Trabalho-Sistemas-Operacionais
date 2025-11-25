#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "fruta.h"
#include "etapas.h"
#include "interface.h"
#include "input.h"
#include "utils.h"

int main() {
    pthread_t frutas[MAX_FRUTAS], input;
    Fruta dados[MAX_FRUTAS];

    // Inicializar gerador de números aleatórios
    srand(time(NULL));

    // Inicializar interface e etapas
    iniciar_tela();
    init_etapas();

    // Criar thread de input
    pthread_create(&input, NULL, thread_input, NULL);

    // Criar threads das frutas com intervalo
    for (int i = 0; i < MAX_FRUTAS; i++) {
        dados[i].id = i + 1;
        pthread_create(&frutas[i], NULL, thread_fruta, &dados[i]);
        sleep(2); // Intervalo de 2 segundos entre criação das frutas
    }

    // Aguardar conclusão de todas as frutas
    for (int i = 0; i < MAX_FRUTAS; i++) {
        pthread_join(frutas[i], NULL);
    }

    // Aguardar um pouco antes de encerrar para ver resultado final
    sleep(3);

    // Limpar recursos
    encerrar_tela();
    destroy_etapas();

    return 0;
}
