# Makefile para o Jogo de Perguntas e Respostas com Raylib

# Compilador
CC = gcc

# Diretórios
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

# Flags de compilação
CFLAGS = -Wall -Wextra -I$(INCLUDE_DIR)
RAYLIB_FLAGS = -lraylib -lm -ldl -lpthread -lGL -lrt -lX11

# Arquivos fonte
MAIN_RAYLIB = $(SRC_DIR)/main_raylib.c
MAIN_CONSOLE = $(SRC_DIR)/main.c

# Arquivos objeto
OBJ_RAYLIB = $(BUILD_DIR)/main_raylib.o
OBJ_CONSOLE = $(BUILD_DIR)/main.o

# Executáveis
TARGET_RAYLIB = $(BUILD_DIR)/jogo_perguntas_raylib
TARGET_CONSOLE = $(BUILD_DIR)/jogo_perguntas_console

# Regra padrão
all: raylib

# Regra para compilar a versão Raylib
raylib: $(TARGET_RAYLIB)

# Regra para compilar a versão console
console: $(TARGET_CONSOLE)

# Compilação do executável Raylib
$(TARGET_RAYLIB): $(OBJ_RAYLIB)
	$(CC) -o $@ $^ $(RAYLIB_FLAGS)

# Compilação do executável console
$(TARGET_CONSOLE): $(OBJ_CONSOLE)
	$(CC) -o $@ $^

# Compilação dos objetos
$(OBJ_RAYLIB): $(MAIN_RAYLIB)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_CONSOLE): $(MAIN_CONSOLE)
	$(CC) -c $(CFLAGS) $< -o $@

# Limpar arquivos gerados
clean:
	rm -f $(BUILD_DIR)/*.o $(TARGET_RAYLIB) $(TARGET_CONSOLE)

# Executar a versão Raylib
run: $(TARGET_RAYLIB)
	$(TARGET_RAYLIB)

# Executar a versão console
run-console: $(TARGET_CONSOLE)
	$(TARGET_CONSOLE)

# Verificar se o Raylib está instalado
check-raylib:
	@echo "Verificando instalação do Raylib..."
	@if [ -f /usr/local/lib/libraylib.a ] || [ -f /usr/lib/libraylib.a ]; then \
		echo "Raylib encontrado!"; \
	else \
		echo "Raylib não encontrado. Por favor, instale-o usando:"; \
		echo "sudo apt-get install libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev"; \
		echo "git clone https://github.com/raysan5/raylib.git"; \
		echo "cd raylib/src && make PLATFORM=PLATFORM_DESKTOP && sudo make install"; \
	fi

.PHONY: all raylib console clean run run-console check-raylib
