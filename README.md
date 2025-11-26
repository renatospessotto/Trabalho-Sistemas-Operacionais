# Fábrica de Sucos - Simulação de SO com Threads e Semáforos

Este projeto é um simulador de linha de produção desenvolvido em C para a disciplina de Sistemas Operacionais. Ele demonstra na prática problemas clássicos de concorrência, utilizando o padrão Produtor-Consumidor, Buffers Circulares, Semáforos e Mutexes.

A interface utiliza ncurses para visualização em tempo real do estado dos buffers e threads.

---

## 🚀 Funcionalidades

### 1. Arquitetura de Pipeline (Produtor-Consumidor)

- **Buffers Circulares**: Entre cada etapa de produção existe uma fila limitada (Buffer).
- **Bloqueio Automático**: Se uma fila enche, a etapa anterior para automaticamente de produzir (simulando gargalos reais de I/O).
- **Semáforos POSIX**: Controle rigoroso de acesso às filas (semáforos `empty` e `full`).

### 2. Interface Gráfica (Terminal)

- **Visualização em tempo real** do tamanho das filas.
- **Status das máquinas**: `LIVRE`, `PROCESSANDO` ou `BLOQUEADO` (quando a fila seguinte está cheia).
- **Barra de progresso** de tempo para cada operação.
- **Sistema de menus** navegável via teclado.

### 3. Sistema Econômico e Progressão

- **Dinheiro**: Venda de sucos gera receita.
- **Upgrades**: O jogador pode comprar melhorias para cada etapa:
  - **Velocidade**: Reduz o tempo de processamento (simula upgrade de Hardware).
  - **Qualidade**: Aumenta o valor de venda do suco.
- **Persistência**: O progresso (dinheiro e upgrades) é salvo automaticamente ao sair (`savegame.txt`).

---

## 🛠️ Estrutura do Projeto

```
Trab1/
├── main.c           # Orquestrador (Inicializa buffers, threads e economia)
├── buffers.c/h      # Implementação das Filas Circulares (Semáforos)
├── etapas.c/h       # Lógica das Threads Operárias (Produtores/Consumidores)
├── economia.c/h     # Sistema de Dinheiro, Upgrades e Save/Load
├── interface.c/h    # Renderização visual com ncurses (Thread-Safe)
├── input.c/h        # Captura de teclado e menus interativos
├── config.h         # Constantes de balanceamento e configuração
└── fruta.c/h        # Estrutura de dados (Item passivo)
```

---

## ⚙️ Como Compilar e Executar

Certifique-se de ter `gcc`, `make` e as bibliotecas `ncurses` e `pthread` instaladas.

```bash
# Limpar compilações anteriores e compilar
make clean
make

# Executar a simulação
./fabrica
```

---

## 🎮 Controles

| Tecla   | Ação                                      |
|---------|------------------------------------------|
| [1-4]   | Selecionar etapa (1: Lavar, 2: Cortar, etc.) |
| [U]     | Abrir Menu de Upgrades                   |
| [V]     | Comprar Velocidade (no menu)             |
| [Q]     | Comprar Qualidade (no menu)              |
| [Q] / [X] | Salvar e Sair do Jogo                   |

---

## 🧠 Conceitos de SO Aplicados

- **Threads**: Cada etapa da fábrica (Lavar, Cortar, Extrair, Embalar) roda em sua própria thread independente.
- **Exclusão Mútua (Mutex)**: Proteção de variáveis globais (dinheiro) e recursos compartilhados (tela do terminal) para evitar Race Conditions.
- **Sincronização (Semáforos)**: Coordenação entre produtores e consumidores para garantir que não haja escrita em buffers cheios ou leitura de buffers vazios.
- **Deadlock Prevention**: O fluxo de dados é estritamente unidirecional e hierárquico, prevenindo impasses cíclicos.

---

Desenvolvido para a disciplina de Sistemas Operacionais - 2025