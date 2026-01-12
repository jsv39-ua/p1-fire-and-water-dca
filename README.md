# Fuego y Agua: La Aventura Elemental


## Como ejecutar el Cmake

desde Windows entramos en linux con wsl en la terminal

luego ejecutamos:
donde tengamos la carpeta del repo en windows
```bash
cd "/mnt/c/Users/USUARIO/Desktop/curso 25-26/dca/juego/p1-fire-and-water-dca"
```
borramos el build anterior
```bash
rm -rf build
```
```bash
cmake -S . -B build -DRAYLIB_TAG=master -DCMAKE_BUILD_TYPE=Release
```
```bash
cmake --build build -j4
```

### Ejecución CMake
```bash
./build/app/Elements
```

## Otra forma de ejecutar el CMake
```bash
cp build
cpack
```
  - Se ha implementado la lógica de **salto** que solo permite saltar si el personaje está tocando una plataforma.


### **Cómo Ejecutar el Proyecto**

Para compilar y ejecutar esta versión del juego, asegúrate de tener las librerías necesarias de `raylib` en la carpeta `vendor/`. Luego, utiliza el siguiente comando en la raíz del proyecto:

```bash
Para linux:
g++ -o game src/*.cpp -I vendor/include/ -L vendor/lib/linux -lraylib -lGL -lm -lpthread -lrt -lX11


Para Windows:

g++ -o game.exe src/*.cpp -I vendor/include/ -L vendor/lib/windows -lraylib -lopengl32 -lgdi32 -lwinmm

### Si se añaden nuevos textos al juego hay que recompilar el .pot y los .po con:

cmake --build build --target update_translations

# Cómo ejecutar los Tests Unitarios

Este proyecto utiliza **Boost.Test** como framework de pruebas y **CTest** como gestor de ejecución.

## Guía Rápida

```bash

# Configurar con tests habilitados
cmake -S . -B build -DBUILD_GAME_TESTS=ON

# Compilar
cmake --build build

# Ejecutar tests en Linux
cd build && ctest --output-on-failure

# En Windows:
cd build; ctest --output-on-failure -C Debug
```

## Compilar el proyecto en paquetes distribuibles

### Windows (con MinGW)

Para Windows tenemos que compilar con MinGW, ya que MSVC no es compatible con gettext (localización).

**Requisitos:**
- MSYS2 con MinGW64 instalado (`C:\msys64\mingw64`)
- Paquetes: `mingw-w64-x86_64-gcc`, `mingw-w64-x86_64-gettext`

**Pasos:**

```powershell
# Crear carpeta de build y configurar con MinGW
cd c:\ruta\al\proyecto
New-Item -ItemType Directory -Path build_mingw -Force
cd build_mingw

# Añadir MinGW al PATH y configurar CMake
$env:PATH = "C:\msys64\mingw64\bin;$env:PATH"
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..

# Compilar
cmake --build . -j4

# Generar paquete ZIP
cpack -G ZIP
```

El paquete se generará en `build_mingw/p1-fire-and-water-dca-X.X.X-Windows.zip`

### Linux

**Requisitos:**
- GCC/G++
- libintl/gettext (`sudo apt install gettext`)

**Pasos:**

```bash
# Crear carpeta de build y configurar
cd /ruta/al/proyecto
mkdir -p build_linux && cd build_linux
cmake -DCMAKE_BUILD_TYPE=Release ..

# Compilar
cmake --build . -j4

# Generar paquetes (DEB y TGZ)
cpack
```

Los paquetes se generarán en:
- `build_linux/p1-fire-and-water-dca-X.X.X-Linux.deb`
- `build_linux/p1-fire-and-water-dca-X.X.X-Linux.tar.gz`

## CI

Los tests se ejecutan automáticamente en GitHub Actions (`.github/workflows/tests.yml`) en:
- Pull requests hacia `main`
- Push a `main`