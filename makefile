
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
    
    LIB_DIR = vendor/lib/windows
    LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
    EXT = .exe
    RM = del /Q
 
    MKDIR = if not exist
	
    # Buscamos dentro de todas las carpetas y subcarpetas
	SRC := $(filter-out core/src/main.cpp, \
	       $(wildcard core/src/*.cpp) $(wildcard core/src/*/*.cpp) $(wildcard core/src/*/*/*.cpp) \
	       $(wildcard app/src/*.cpp)  $(wildcard app/src/*/*.cpp)  $(wildcard app/src/*/*/*.cpp))
    
    INSTALL_DIR = "C:/Program Files/ElementsDCA" 
else
    DETECTED_OS := Linux
    LIB_DIR = vendor/lib/linux
    LIBS = -lraylib -lGL -lm -lpthread -lrt -lX11
    EXT =
    RM = rm -rf
    MKDIR = mkdir -p
    

    # 'find' busca en todas las subcarpetas automáticamente
    # Excluimos core/src/main.cpp
	SRC := $(shell find core/src app/src -name '*.cpp' ! -path 'core/src/main.cpp')
    
    INSTALL_DIR = /usr/local/bin
endif

# Opciones del compilador
CXX = g++
CXXFLAGS = -I vendor/include/ -I include/ -I core/include/ -Wall -MMD -MP
LDFLAGS = -L $(LIB_DIR)


# 
PKG_CONFIG ?= pkg-config
ifeq ($(DETECTED_OS),Windows)
	# En Windows no tenemos /dev/null y generalmente no usamos pkg-config para libintl
	INTL_CFLAGS :=
	INTL_LIBS :=
else
	INTL_CFLAGS := $(shell $(PKG_CONFIG) --cflags libintl 2>/dev/null)
	INTL_LIBS := $(shell $(PKG_CONFIG) --libs libintl 2>/dev/null)
endif

# En muchas distros (glibc), Intl viene "built-in" y no hay libintl.pc.
# Para no romper Linux, activamos gettext por defecto en no-Windows.
ifeq ($(DETECTED_OS),Windows)
	HAVE_GETTEXT := 0
else
	HAVE_GETTEXT := 1
	CXXFLAGS += -DHAVE_GETTEXT=1
endif

# Si existe libintl por pkg-config, añadimos flags/libs extra.
ifneq ($(strip $(INTL_LIBS)),)
	CXXFLAGS += $(INTL_CFLAGS)
	LIBS += $(INTL_LIBS)
endif

# --- INSTALL (estilo GNU) ---
PREFIX ?= /usr/local
DESTDIR ?=
BINDIR ?= $(PREFIX)/bin
DATADIR ?= $(PREFIX)/share
INSTALL_BIN_DIR := $(DESTDIR)$(BINDIR)
INSTALL_DATA_DIR := $(DESTDIR)$(DATADIR)/p1-fire-and-water-dca

# --- ARCHIVOS OBJETO ---
OBJ_DIR = obj
# Esto convierte ej: "core/src/systems/MapLoader.cpp" -> "obj/systems/MapLoader.o"
OBJ := $(SRC:core/src/%.cpp=$(OBJ_DIR)/%.o)
OBJ := $(OBJ:app/src/%.cpp=$(OBJ_DIR)/app/%.o)
DEPS := $(OBJ:.o=.d)

TARGET = game$(EXT)
BUILD_CMAKE_DIR = build_dist


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

# Aqui creamos el directorio correspondiente antes de compilar cada archivo, ya que si
# por ejemplo intentamos compilar "obj/systems/MapLoader.o" pero "obj/systems/" no existe, fallara

$(OBJ_DIR)/%.o: core/src/%.cpp
	@echo "Compilando $<..."
ifeq ($(DETECTED_OS),Windows)
	@if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
else
	@$(MKDIR) $(dir $@)
endif
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/app/%.o: app/src/%.cpp
	@echo "Compilando $<..."
ifeq ($(DETECTED_OS),Windows)
	@if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
else
	@$(MKDIR) $(dir $@)
endif
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEPS)

run: $(TARGET)
	./$(TARGET)

clean:
ifeq ($(DETECTED_OS),Windows)
	@if exist "$(OBJ_DIR)" (rmdir /s /q "$(OBJ_DIR)" & echo Directorio obj eliminado) else (echo No hay obj para limpiar)
	@if exist "$(TARGET)" (del /q "$(TARGET)" & echo Ejecutable eliminado) else (echo No hay ejecutable para limpiar)
else
	$(RM) $(OBJ_DIR) $(TARGET) $(BUILD_CMAKE_DIR) *.deb *.d
endif
	@echo "Limpieza completada."

install: all
ifeq ($(DETECTED_OS),Linux)
	@echo "Instalando en $(INSTALL_BIN_DIR) y datos en $(INSTALL_DATA_DIR)..."
	@$(MKDIR) "$(INSTALL_BIN_DIR)"
	@$(MKDIR) "$(INSTALL_DATA_DIR)"
	install -m 0755 "$(TARGET)" "$(INSTALL_BIN_DIR)/$(TARGET)"
	cp -R "assets" "$(INSTALL_DATA_DIR)/assets"
else
	@echo "Install en Windows requiere admin manual."
endif

uninstall:
ifeq ($(DETECTED_OS),Linux)
	@echo "Desinstalando de $(INSTALL_BIN_DIR) y $(INSTALL_DATA_DIR)..."
	@$(RM) "$(INSTALL_BIN_DIR)/$(TARGET)"
	@$(RM) "$(INSTALL_DATA_DIR)"
else
	@echo "Uninstall en Windows requiere admin manual."
endif

dist:
ifeq ($(DETECTED_OS),Windows)
	@echo "Generando paquete .zip para Windows..."
	@if exist "$(BUILD_CMAKE_DIR)" rmdir /s /q "$(BUILD_CMAKE_DIR)"
	cmake -S . -B $(BUILD_CMAKE_DIR) -DCMAKE_BUILD_TYPE=Release
	cmake --build $(BUILD_CMAKE_DIR) --config Release
	cd $(BUILD_CMAKE_DIR) && cpack -G ZIP
	move $(BUILD_CMAKE_DIR)\*.zip .
	rmdir /s /q $(BUILD_CMAKE_DIR)
else
	@echo "Generando paquete .deb para Linux..."
	@$(RM) $(BUILD_CMAKE_DIR)
	cmake -S . -B $(BUILD_CMAKE_DIR) -DCMAKE_BUILD_TYPE=Release
	cmake --build $(BUILD_CMAKE_DIR) --config Release
	cd $(BUILD_CMAKE_DIR) && cpack
	mv $(BUILD_CMAKE_DIR)/*.deb .
	$(RM) $(BUILD_CMAKE_DIR)
endif

.PHONY: all clean run info install uninstall dist