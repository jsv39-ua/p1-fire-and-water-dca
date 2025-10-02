# Detectar sistema operativo
ifeq ($(OS),Windows_NT)
    # Windows
    DETECTED_OS := Windows
    LIB_DIR = vendor/lib/windows
    LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
    EXT = .exe
    RM = rm
    MKDIR = mkdir
else
    # Linux
    DETECTED_OS := Linux
    LIB_DIR = vendor/lib/linux
    LIBS = -lraylib -lGL -lm -lpthread -lrt -lX11
    EXT =
    RM = rm -f
    MKDIR = mkdir -p
endif

# Configuración común
CXX = g++
CXXFLAGS = -I vendor/include/
LDFLAGS = -L $(LIB_DIR)
SRC = $(wildcard src/*.cpp)
TARGET = game$(EXT)

# Regla principal
all: info $(TARGET)

info:
	@echo "Compilando para $(DETECTED_OS)..."
	@echo "Usando librerías de: $(LIB_DIR)"

$(TARGET): $(SRC)
	$(CXX) -o $(TARGET) $(SRC) $(CXXFLAGS) $(LDFLAGS) $(LIBS)
	@echo "Compilación exitosa!"

clean:
	$(RM) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run info