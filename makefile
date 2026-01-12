
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
    LIB_DIR = vendor/lib/windows
    LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
    EXT = .exe
    RM = del /Q
    MKDIR = if not exist
	SRC := $(filter-out core/src/main.cpp, \
	       $(wildcard core/src/*.cpp) $(wildcard core/src/*/*.cpp) $(wildcard core/src/*/*/*.cpp) \
	       $(wildcard app/src/*.cpp)  $(wildcard app/src/*/*.cpp)  $(wildcard app/src/*/*/*.cpp))
else
    DETECTED_OS := Linux
    LIB_DIR = vendor/lib/linux
    LIBS = -lraylib -lGL -lm -lpthread -lrt -lX11
    EXT =
    RM = rm -rf
    MKDIR = mkdir -p
	SRC := $(shell find core/src app/src -name '*.cpp' ! -path 'core/src/main.cpp')
endif

CXX = g++
CXXFLAGS = -I vendor/include/ -I include/ -I core/include/ -Wall -MMD -MP
LDFLAGS = -L $(LIB_DIR)

PKG_CONFIG ?= pkg-config
ifeq ($(DETECTED_OS),Windows)
	INTL_CFLAGS :=
	INTL_LIBS := -lintl
	CXXFLAGS += -DHAVE_GETTEXT=1
else
	INTL_CFLAGS := $(shell $(PKG_CONFIG) --cflags libintl 2>/dev/null)
	INTL_LIBS := $(shell $(PKG_CONFIG) --libs libintl 2>/dev/null)
	CXXFLAGS += -DHAVE_GETTEXT=1
endif

ifneq ($(strip $(INTL_LIBS)),)
	CXXFLAGS += $(INTL_CFLAGS)
	LIBS += $(INTL_LIBS)
endif

PREFIX ?= /usr/local
DESTDIR ?=
BINDIR ?= $(PREFIX)/bin
DATADIR ?= $(PREFIX)/share
INSTALL_BIN_DIR := $(DESTDIR)$(BINDIR)
INSTALL_DATA_DIR := $(DESTDIR)$(DATADIR)/p1-fire-and-water-dca

OBJ_DIR = obj
OBJ := $(SRC:core/src/%.cpp=$(OBJ_DIR)/%.o)
OBJ := $(OBJ:app/src/%.cpp=$(OBJ_DIR)/app/%.o)
DEPS := $(OBJ:.o=.d)

TARGET = game$(EXT)
BUILD_CMAKE_DIR = build_dist

all: $(TARGET)

$(TARGET): $(OBJ)
	@echo "Enlazando ejecutable..."
	$(CXX) -o $@ $(OBJ) $(LDFLAGS) $(LIBS)
	@echo "Ejecutable $(TARGET) creado."

$(OBJ_DIR)/%.o: core/src/%.cpp
	@echo "Compilando $<..."
ifeq ($(DETECTED_OS),Windows)
	@if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
else
	@$(MKDIR) $(dir $@)
endif
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/app/%.o: app/src/%.cpp
	@echo "Compilando $<..."
ifeq ($(DETECTED_OS),Windows)
	@if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
else
	@$(MKDIR) $(dir $@)
endif
	@$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEPS)

run: $(TARGET)
	./$(TARGET)

clean:
ifeq ($(DETECTED_OS),Windows)
	@if exist "$(OBJ_DIR)" rmdir /s /q "$(OBJ_DIR)"
	@if exist "$(TARGET)" del /q "$(TARGET)"
else
	$(RM) $(OBJ_DIR) $(TARGET) $(BUILD_CMAKE_DIR) *.deb *.d
endif
	@echo "Limpieza completada."

install: all
ifeq ($(DETECTED_OS),Linux)
	@echo "Instalando..."
	@$(MKDIR) "$(INSTALL_BIN_DIR)"
	@$(MKDIR) "$(INSTALL_DATA_DIR)"
	install -m 0755 "$(TARGET)" "$(INSTALL_BIN_DIR)/$(TARGET)"
	cp -R "assets" "$(INSTALL_DATA_DIR)/assets"
	@echo "Instalacion completada."
endif

uninstall:
ifeq ($(DETECTED_OS),Linux)
	@echo "Desinstalando..."
	@$(RM) "$(INSTALL_BIN_DIR)/$(TARGET)"
	@$(RM) "$(INSTALL_DATA_DIR)"
	@echo "Desinstalacion completada."
endif

dist:
ifeq ($(DETECTED_OS),Windows)
	@echo "Generando paquete para Windows..."
	@if exist "$(BUILD_CMAKE_DIR)" rmdir /s /q "$(BUILD_CMAKE_DIR)"
	cmake -S . -B $(BUILD_CMAKE_DIR) -DCMAKE_BUILD_TYPE=Release
	cmake --build $(BUILD_CMAKE_DIR) --config Release
	cd $(BUILD_CMAKE_DIR) && cpack -G ZIP
	move $(BUILD_CMAKE_DIR)\*.zip .
	rmdir /s /q $(BUILD_CMAKE_DIR)
	@echo "Paquete generado."
else
	@echo "Generando paquete para Linux..."
	@$(RM) $(BUILD_CMAKE_DIR)
	cmake -S . -B $(BUILD_CMAKE_DIR) -DCMAKE_BUILD_TYPE=Release
	cmake --build $(BUILD_CMAKE_DIR) --config Release
	cd $(BUILD_CMAKE_DIR) && cpack
	mv $(BUILD_CMAKE_DIR)/*.deb .
	$(RM) $(BUILD_CMAKE_DIR)
	@echo "Paquete generado."
endif

.PHONY: all clean run install uninstall dist
