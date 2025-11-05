#pragma once


extern "C" {
    #include <raylib.h>
}

#include <iostream>
#include <stdexcept>
#include <vector>

// Forward declaration - evita incluir raytmx.h en el header
typedef struct tmx_map TmxMap;

class MapLoader
{
public:

    MapLoader();
    MapLoader(const char* filePath);
    ~MapLoader();

    // Const define que el metodo no modificará el objeto, es como un readOnly
    TmxMap* getMapa() const;

    // Cargar el mapa pasado como parametro
    void loadMapa(const char* filePath);

    // Descargar el mapa actual
    void unloadMapa();

    void draw(const Camera2D& camera);

    
    std::vector<Rectangle> extractCollisions();

private:

   TmxMap* mapa; // Mapa cargado actualmente
  

   const int TILE_SIZE = 18; // Tamaño de los tiles en pixeles
};
