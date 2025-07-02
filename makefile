# Diretórios
SRC_DIR = src
INC_DIR = include
BIN_DIR = bin
UTIL_INC = utils/include
UTIL_LIB = utils/lib

# Compilador e flags
CC = gcc
CFLAGS = -I$(INC_DIR) -I$(UTIL_INC)
LDFLAGS = -L$(UTIL_LIB) -lmingw32 -lSDL2main -lSDL2

# Fontes e objetos
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:.c=.o)

# Executável
TARGET = main

# Regra principal
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

# Regra genérica para compilar .c em .o
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Limpa arquivos compilados
clean:
	del /Q $(SRC_DIR)\*.o $(TARGET).exe 2>nul || true