# Fábrica de Sucos - Simulador com Threads e Semáforos

Este projeto simula uma fábrica de sucos usando threads (pthread) e semáforos para controlar o acesso às estações de trabalho. A interface usa ncurses para uma visualização organizada e interativa.

## Melhorias Implementadas

### 1. Interface Melhorada com ncurses
- Layout limpo e organizado com títulos fixos para cada etapa
- Visualização em tempo real das frutas sendo processadas
- Filas de espera visíveis quando os semáforos estão bloqueados
- Barra de progresso para acompanhar o andamento
- Cores para melhor identificação dos elementos
- Atualização sem flicker usando refresh() controlado

### 2. Controle Dinâmico de Máquinas
- Possibilidade de aumentar/diminuir o número de máquinas por etapa em tempo real
- Controles intuitivos com teclas + e -
- Seleção de etapa com teclas 1-4
- Indicador visual da etapa selecionada (>)

### 3. Resolução dos Warnings de Semáforos no macOS
- Flag `-Wno-deprecated-declarations` adicionada ao Makefile
- Mantém compatibilidade com sem_t enquanto suprime warnings deprecados

## Estrutura do Projeto

```
Trab1/
├── main.c          # Programa principal
├── fruta.c/h       # Implementação das threads das frutas
├── etapas.c/h      # Controle dos semáforos e processamento das etapas
├── interface.c/h   # Interface ncurses melhorada
├── input.c/h       # Thread de controle de entrada do usuário
├── utils.h         # Definições e constantes
└── makefile        # Compilação com flags corretas
```

## Etapas da Fábrica

1. **LAVAR** (símbolo: *) - 2 máquinas iniciais
2. **CORTAR** (símbolo: X) - 1 máquina inicial  
3. **EXTRAIR** (símbolo: J) - 1 máquina inicial
4. **EMBALAR** (símbolo: P) - 1 máquina inicial

## Controles Interativos

- **[1-4]**: Selecionar etapa para ajuste
- **[+]**: Aumentar número de máquinas na etapa selecionada
- **[-]**: Diminuir número de máquinas na etapa selecionada
- **[r]**: Redesenhar tela completa
- **[q]**: Sair do programa

## Como Compilar e Executar

```bash
# Compilar
make clean && make

# Executar
./fabrica
```

## Dependências

- **pthread**: Para threads das frutas e controle de entrada
- **ncurses**: Para interface de terminal melhorada
- **semáforos POSIX**: Para controle de acesso às estações

## Funcionalidades da Interface

### Visualização em Tempo Real
- Cada etapa mostra quantas frutas estão sendo processadas
- Fila de espera exibida com pontos (.) quando há mais frutas que máquinas
- Contador de frutas concluídas e barra de progresso
- Notificações quando frutas são concluídas

### Controle Dinâmico
- Ajuste em tempo real do número de máquinas por etapa
- Efeito imediato na simulação (semáforos são atualizados dinamicamente)
- Interface responde instantaneamente aos comandos

### Sincronização Thread-Safe
- Mutex protege atualizações da interface
- Semáforos controlam acesso às estações de trabalho
- Threads de frutas processam sequencialmente as 4 etapas

## Observações Técnicas

- **macOS**: Flag `-Wno-deprecated-declarations` resolve warnings de semáforos deprecados
- **Caracteres**: Apenas ASCII básico para máxima compatibilidade
- **Performance**: Atualizações parciais da tela para evitar flicker
- **Escalabilidade**: Fácil ajuste do número de frutas via `MAX_FRUTAS` em utils.h

Este projeto demonstra conceitos importantes de programação concorrente, sincronização de threads e desenvolvimento de interfaces de terminal interativas.