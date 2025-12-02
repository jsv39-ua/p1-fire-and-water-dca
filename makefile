# Makefile Todo Terreno (Busca en todas partes)
CXX      := g++
CXXFLAGS := -Iinclude -Ivendor/include -Wall -std=c++17 -MMD -MP
LDFLAGS  := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Configuración de CCACHE
ifdef CCACHE
    CXX := ccache g++
endif

OBJ_DIR  := obj
BIN      := game

# BUSCAR TODOS LOS .cpp recursivamente en src, core y graphics
SOURCES  := $(shell find src core graphics -name "*.cpp" 2>/dev/null)
OBJECTS  := $(SOURCES:%.cpp=$(OBJ_DIR)/%.o)

# Regla Principal
all: $(BIN)

$(BIN): $(OBJECTS)
	@echo "Enlazando ejecutable..."
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# Compilación de objetos (Mantiene estructura de carpetas en obj/)
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpieza
clean:
	rm -rf $(OBJ_DIR) $(BIN)

run: $(BIN)
	./$(BIN)

-include $(OBJECTS:.o=.d)
