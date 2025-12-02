# Detectar sistema operativo
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
    LIB_DIR = vendor/lib/windows
    LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
    EXT = .exe
    RM = del /Q
    MKDIR = if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
    # En Windows, usar una lista explícita de archivos .cpp
    SRC := $(wildcard src/*.cpp)
    INSTALL_DIR = "C:/Program Files/TuJuego" # Ejemplo para Windows
else
    DETECTED_OS := Linux
    LIB_DIR = vendor/lib/linux
    LIBS = -lraylib -lGL -lm -lpthread -lrt -lX11
    EXT =
    RM = rm -rf
    MKDIR = mkdir -p
    # En Linux, búsqueda recursiva
    SRC := $(shell find src -name '*.cpp')
    INSTALL_DIR = /usr/local/bin
endif

# Configuración del compilador
CXX = g++
# -MMD -MP genera archivos de dependencias (.d) automáticamente
CXXFLAGS = -I vendor/include/ -Wall -MMD -MP
LDFLAGS = -L $(LIB_DIR)

# --- Archivos Objeto ---
OBJ_DIR = obj
OBJ := $(SRC:src/%.cpp=$(OBJ_DIR)/%.o)
# Archivos de dependencias generados
DEPS := $(OBJ:.o=.d)

TARGET = game$(EXT)
BUILD_CMAKE_DIR = build_dist

# Regla principal
all: info $(TARGET)

info:
	@echo "Sistema detectado: $(DETECTED_OS)"
	@echo "Compilando fuentes..."

# Linkado Final
$(TARGET): $(OBJ)
	@echo "Linkando ejecutable..."
	$(CXX) -o $@ $(OBJ) $(LDFLAGS) $(LIBS)
	@echo "Compilación exitosa: $(TARGET)"

# Compilación de objetos
$(OBJ_DIR)/%.o: src/%.cpp
	@$(MKDIR) $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Incluir dependencias
-include $(DEPS)

# --- REGLAS EXIGIDAS POR EL ENTREGABLE ---

# 1. Regla INSTALL (Copia el ejecutable al sistema)
install: all
ifeq ($(DETECTED_OS),Linux)
	@echo "Instalando en $(INSTALL_DIR)... (Requiere sudo)"
	sudo cp $(TARGET) $(INSTALL_DIR)/$(TARGET)
	@echo "Instalación completada. Ejecuta '$(TARGET)' desde cualquier lugar."
else
	@echo "La regla install en Windows requiere permisos de administrador manuales o configuración extra."
endif

# 2. Regla DIST (Genera el paquete .deb usando CPack)
# Esta regla llama a CMake solo para empaquetar, cumpliendo el requisito académico
dist:
	@echo "Generando paquete distribuible (.deb)..."
	@$(RM) $(BUILD_CMAKE_DIR)
	cmake -S . -B $(BUILD_CMAKE_DIR) -DCPACK_GENERATOR="DEB" -DCMAKE_BUILD_TYPE=Release
	cd $(BUILD_CMAKE_DIR) && cpack
	@echo "Moviendo paquete a la raíz..."
	mv $(BUILD_CMAKE_DIR)/*.deb .
	@echo "Limpiando archivos temporales de empaquetado..."
	$(RM) $(BUILD_CMAKE_DIR)
	@echo "¡Paquete generado con éxito!"

# Limpieza
clean:
	$(RM) $(OBJ_DIR) $(TARGET) $(BUILD_CMAKE_DIR) *.deb *.d
	@echo "Limpieza completada."

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run info install dist