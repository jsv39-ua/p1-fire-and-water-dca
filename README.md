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
./build/game
```

## Otra forma de ejecutar el CMake
```bash
cp build
cpack
```

## **Hito 1 - Versión Alpha (Práctica 1)**

Este repositorio contiene el proyecto para la asignatura de **Desarrollo Colaborativo de Aplicaciones (DCA)**, enfocado en el desarrollo de un videojuego 2D cooperativo de plataformas y puzzles llamado "Fuego y Agua".

### **Progreso del Proyecto**

Hemos completado la configuración inicial de la versión "alpha" del juego. Los siguientes hitos han sido alcanzados:

- **Ejercicio 0: Configuración de Entidades y Ventana**
  - Se ha inicializado una ventana de juego con las dimensiones correctas.
  - Se han definido las estructuras de datos básicas para los jugadores (Fuego y Agua).
  - Se ha creado un nivel de prueba con plataformas estáticas.
  - Los personajes y el nivel se dibujan correctamente en la pantalla utilizando figuras geométricas básicas (rectángulos).

- **Ejercicio 1: Movimiento y Física de Plataformas**
  - Se ha implementado el control de movimiento horizontal para ambos personajes con sus respectivas teclas (**A, D, Izquierda, Derecha**).
  - La **gravedad y la física de colisiones** con las plataformas estáticas se han añadido correctamente.
  - Se ha implementado la lógica de **salto** que solo permite saltar si el personaje está tocando una plataforma.

- **Ejercicio 2: Colisiones Elementales y Transición a Game Over**
  - Se han añadido al nivel **peligros de lava y agua** como obstáculos.
  - Se ha implementado la **lógica de colisiones elementales** que hace que cada personaje muera al tocar el elemento opuesto.
  - Se ha implementado una **pantalla de Game Over** que se activa al morir un personaje y permite reiniciar el juego.


### **Cómo Ejecutar el Proyecto**

Para compilar y ejecutar esta versión del juego, asegúrate de tener las librerías necesarias de `raylib` en la carpeta `vendor/`. Luego, utiliza el siguiente comando en la raíz del proyecto:

```bash
Para linux:
g++ -o game src/*.cpp -I vendor/include/ -L vendor/lib/linux -lraylib -lGL -lm -lpthread -lrt -lX11


Para Windows:

g++ -o game.exe src/*.cpp -I vendor/include/ -L vendor/lib/windows -lraylib -lopengl32 -lgdi32 -lwinmm