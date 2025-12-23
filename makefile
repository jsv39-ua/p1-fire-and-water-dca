# --- DETECCIÓN DEL SISTEMA OPERATIVO ---
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
    # Ajusta estas rutas según donde tengas tus librerías en Windows
    LIB_DIR = vendor/lib/windows
    LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
    EXT = .exe
    RM = del /Q
    
    # Comandos específicos de Windows para crear carpetas
    # Usamos if not exist para evitar errores si ya existe
    MKDIR = if not exist
    
	# BUSQUEDA RECURSIVA (WINDOWS):
	# Busca en core/src y app/src
	SRC := $(filter-out core/src/main.cpp, \
	       $(wildcard core/src/*.cpp) $(wildcard core/src/*/*.cpp) $(wildcard core/src/*/*/*.cpp) \
	       $(wildcard app/src/*.cpp)  $(wildcard app/src/*/*.cpp)  $(wildcard app/src/*/*/*.cpp))
    
    INSTALL_DIR = "C:/Program Files/TuJuego" 
else
    DETECTED_OS := Linux
    LIB_DIR = vendor/lib/linux
    LIBS = -lraylib -lGL -lm -lpthread -lrt -lX11
    EXT =
    RM = rm -rf
    MKDIR = mkdir -p
    
	# BUSQUEDA RECURSIVA (LINUX):
	# 'find' busca en todas las subcarpetas automáticamente
	# Excluimos core/src/main.cpp: el main vive en app/src/main.cpp
	SRC := $(shell find core/src app/src -name '*.cpp' ! -path 'core/src/main.cpp')
    
    INSTALL_DIR = /usr/local/bin
endif

# --- CONFIGURACIÓN DEL COMPILADOR ---
CXX = g++
# -I vendor/include/ es CRUCIAL porque ahí tienes tus headers (.hpp)
# Agregamos -I include/ para headers propios del proyecto
CXXFLAGS = -I vendor/include/ -I include/ -I core/include/ -Wall -MMD -MP
LDFLAGS = -L $(LIB_DIR)

# --- ARCHIVOS OBJETO ---
OBJ_DIR = obj
# Esto convierte ej: "core/src/systems/MapLoader.cpp" -> "obj/systems/MapLoader.o"
OBJ := $(SRC:core/src/%.cpp=$(OBJ_DIR)/%.o)
OBJ := $(OBJ:app/src/%.cpp=$(OBJ_DIR)/app/%.o)
DEPS := $(OBJ:.o=.d)

TARGET = game$(EXT)
BUILD_CMAKE_DIR = build_dist

# --- REGLAS PRINCIPALES ---

all: info $(TARGET)

info:
	@echo "--------------------------------------"
	@echo "Sistema: $(DETECTED_OS)"
	@echo "Estructura detectada en src/:"
	@echo $(SRC)
	@echo "--------------------------------------"

# Linkado Final
$(TARGET): $(OBJ)
	@echo "Linkando ejecutable..."
	$(CXX) -o $@ $(OBJ) $(LDFLAGS) $(LIBS)
	@echo "--> EXITO: Ejecutable $(TARGET) creado."

# --- REGLA DE COMPILACIÓN (MAGIA MULTIDIRECTORIO) ---
$(OBJ_DIR)/%.o: core/src/%.cpp
	@echo "Compilando $<..."
ifeq ($(DETECTED_OS),Windows)
	@if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
else
	@$(MKDIR) $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
endif

$(OBJ_DIR)/app/%.o: app/src/%.cpp
	@echo "Compilando $<..."
ifeq ($(DETECTED_OS),Windows)
	@if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
else
	@$(MKDIR) $(dir $@)
endif
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Incluir dependencias generadas (.d)
-include $(DEPS)

# --- UTILIDADES ---

run: $(TARGET)
	./$(TARGET)

clean:
ifeq ($(DETECTED_OS),Windows)
	@if exist $(OBJ_DIR) rmdir /s /q $(OBJ_DIR)
	@if exist $(TARGET) del /q $(TARGET)
else
	$(RM) $(OBJ_DIR) $(TARGET) $(BUILD_CMAKE_DIR) *.deb *.d
endif
	@echo "Limpieza completada."

# --- REGLAS DE ENTREGA (NO TOCAR) ---
install: all
ifeq ($(DETECTED_OS),Linux)
	@echo "Instalando..."
	sudo cp $(TARGET) $(INSTALL_DIR)/$(TARGET)
else
	@echo "Install en Windows requiere admin manual."
endif

dist:
	@echo "Generando paquete .deb..."
	@$(RM) $(BUILD_CMAKE_DIR)
	cmake -S . -B $(BUILD_CMAKE_DIR) -DCPACK_GENERATOR="DEB" -DCMAKE_BUILD_TYPE=Release
	cmake --build $(BUILD_CMAKE_DIR) --config Release
	cd $(BUILD_CMAKE_DIR) && cpack
	mv $(BUILD_CMAKE_DIR)/*.deb .
	$(RM) $(BUILD_CMAKE_DIR)

.PHONY: all clean run info install dist