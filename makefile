# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -pthread -g
LIBS = -lncurses

# Diretório para objetos
OBJ_DIR = obj

# Lista de arquivos fonte
SRCS = main.c buffers.c etapas.c fruta.c interface.c input.c economia.c upgrades.c

# Lista de objetos dentro da pasta obj
OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))

# Nome do executável
TARGET = fabrica

# Regra principal
all: $(OBJ_DIR) $(TARGET)

# Cria o diretório obj se não existir
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Linkagem final
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Compilação de cada .c em .o dentro de obj
$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpeza
clean:
	rm -f $(OBJS) $(TARGET) debug.txt
	rm -rf $(OBJ_DIR)

# Executar
run: $(TARGET)
	./$(TARGET)
