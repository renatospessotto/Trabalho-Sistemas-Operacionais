#include "fruta.h"
#include "etapas.h"
#include "interface.h"
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

void* thread_fruta(void* arg) {
    Fruta* f = (Fruta*) arg;
    
    // Inicializar valores da fruta
    f->pontuacao = 0;
    f->entrada_etapa = time(NULL);
    
    // Pequeno delay aleatório antes de começar (simular chegada das frutas)
    usleep((rand() % 1000000)); // 0-1 segundo
    
    // Processar cada etapa sequencialmente
    processar_etapa(f, LAVAR);
    usleep(200000); // 200ms entre etapas
    
    processar_etapa(f, CORTAR);
    usleep(200000);
    
    processar_etapa(f, EXTRAIR);
    usleep(200000);
    
    processar_etapa(f, EMBALAR);
    
    // Fruta concluída
    f->pontuacao = 100; // Pontuação final
    fruta_concluida(f);
    
    pthread_exit(NULL);
}
