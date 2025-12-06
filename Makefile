# --- Makefile compatible con MSYS2 y Windows ---

TARGET := bin/Ben10.exe
SRC_DIR := src
INC_DIR := include
BIN_DIR := bin

# Archivos fuente
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BIN_DIR)/%.o)

# Librerías
LIBS := -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lbox2d

# Compilador y Flags
CXX := g++
CXXFLAGS := -I$(INC_DIR) -I. -g -Wall

# --- Reglas ---

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(OBJS) -o $(TARGET) $(LIBS)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# CORRECCIÓN AQUÍ: Usamos 'mkdir -p' que funciona en MSYS2
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

run: all
	./$(TARGET)

# CORRECCIÓN AQUÍ: Usamos 'rm' en lugar de 'del'
clean:
	rm -rf $(BIN_DIR)

.PHONY: all run clean