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
./build/app/game.bin
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

## CI

Los tests se ejecutan automáticamente en GitHub Actions (`.github/workflows/tests.yml`) en:
- Pull requests hacia `main`
- Push a `main`