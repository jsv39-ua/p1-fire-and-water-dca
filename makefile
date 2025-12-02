# Detectar sistema operativo
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
    LIB_DIR = vendor/lib/windows
    LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
    EXT = .exe
    RM = del /Q
    # Fix para limpiar carpetas en Windows si es necesario, o usar rm de git bash
else
    DETECTED_OS := Linux
    LIB_DIR = vendor/lib/linux
    LIBS = -lraylib -lGL -lm -lpthread -lrt -lX11
    EXT =
    RM = rm -rf
endif

# Configuración del compilador
CXX = g++
# -MMD -MP genera archivos de dependencias (.d) automáticamente
CXXFLAGS = -I vendor/include/ -Wall -MMD -MP
LDFLAGS = -L $(LIB_DIR)

# --- CORRECCIÓN 1: Búsqueda Recursiva ---
# Busca todos los .cpp dentro de src y cualquier subcarpeta
SRC := $(shell find src -name '*.cpp')

# --- CORRECCIÓN 2: Archivos Objeto ---
# Convierte la lista de .cpp (src/main.cpp) en .o (obj/main.o)
OBJ_DIR = obj
OBJ := $(SRC:src/%.cpp=$(OBJ_DIR)/%.o)
# Archivos de dependencias generados
DEPS := $(OBJ:.o=.d)

TARGET = game$(EXT)

# Regla principal
all: info $(TARGET)

info:
	@echo "Sistema: $(DETECTED_OS)"
	@echo "Archivos fuente encontrados: $(SRC)"

# Linkado Final (Depende de todos los objetos)
$(TARGET): $(OBJ)
	@echo "Linkando ejecutable..."
	$(CXX) -o $@ $(OBJ) $(LDFLAGS) $(LIBS)
	@echo "Compilación exitosa: $(TARGET)"

# Regla de Compilación Individual (Para que funcione el multihilo -jN)
# Crea la carpeta necesaria y compila de .cpp a .o
$(OBJ_DIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Incluir dependencias (detecta si cambias un .hpp)
-include $(DEPS)

clean:
	$(RM) $(OBJ_DIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run info