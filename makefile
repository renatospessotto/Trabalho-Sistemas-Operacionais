CC = gcc
CFLAGS = -Wall -Wextra -pthread -g
LIBS = -lncurses

# Define a pasta de objetos
OBJ_DIR = obj

# Lista de arquivos objeto (com o prefixo da pasta)
_OBJS = main.o buffers.o etapas.o fruta.o interface.o input.o economia.o
OBJS = $(patsubst %,$(OBJ_DIR)/%,$(_OBJS))

# Nome do executável
TARGET = fabrica

# Regra principal (all)
all: $(OBJ_DIR) $(TARGET)

# Cria o diretório obj se não existir
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Linkagem
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Regra genérica para compilar qualquer .c em .o dentro da pasta obj
$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpeza (apaga a pasta obj inteira)
clean:
	rm -f $(OBJS) $(TARGET) debug.txt
	rm -rf $(OBJ_DIR)

# Executar
run: $(TARGET)
	./$(TARGET)