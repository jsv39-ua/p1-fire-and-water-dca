#pragma once


extern "C" {
    #include <raylib.h>
}



#include <iostream>
#include <stdexcept>
#include <vector>
#include "Enemy.hpp"
#include <raytmx.h> 

struct TileData {
    Texture2D texture;
    Rectangle source;
    bool valid;
};

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

    // Extraer enemigos desde las capas de objetos del mapa
    std::vector<Enemy> extractEnemies();

    // Obtener las dimensiones del mapa en pixeles
    Rectangle getMapBounds() const;

private:

   TmxMap* mapa; // Mapa cargado actualmente
  

   const int TILE_SIZE = 18; // Tamaño de los tiles en pixeles

   // En la sección private de MapLoader.hpp

    const char* getPropertyStringFromLayer(TmxLayer* layer, const char* propName, const char* defaultValue) {
        if (!layer->properties) return defaultValue;
        for (uint32_t i = 0; i < layer->propertiesLength; i++) {
            if (strcmp(layer->properties[i].name, propName) == 0 && 
                layer->properties[i].type == PROPERTY_TYPE_STRING) {
                return layer->properties[i].stringValue;
            }
        }
        return defaultValue;
    }

    float getPropertyFloatFromLayer(TmxLayer* layer, const char* propName, float defaultValue) {
        if (!layer->properties) return defaultValue;
        for (uint32_t i = 0; i < layer->propertiesLength; i++) {
            if (strcmp(layer->properties[i].name, propName) == 0 && 
                layer->properties[i].type == PROPERTY_TYPE_FLOAT) {
                return layer->properties[i].floatValue;
            }
        }
        return defaultValue;
    }

    TileData GetTileDataFromGID(uint32_t gid) {
        TileData data = { {0}, {0}, false };
        
        for (uint32_t t = 0; t < this->mapa->tilesetsLength; t++) {
            TmxTileset* tileset = &this->mapa->tilesets[t];
            
            // Verificar si el GID pertenece a este tileset
            if (gid >= tileset->firstGid) {
                // Verificar si es el último tileset o es menor que el siguiente
                bool isCorrectTileset = (t == this->mapa->tilesetsLength - 1) || 
                                      (gid < this->mapa->tilesets[t+1].firstGid);
                
                if (isCorrectTileset) {
                    if (tileset->hasImage) {
                        data.texture = tileset->image.texture;
                        uint32_t localId = gid - tileset->firstGid;
                        
                        uint32_t col = localId % tileset->columns;
                        uint32_t row = localId / tileset->columns;
                        
                        data.source.x = (float)(tileset->margin + col * (tileset->tileWidth + tileset->spacing));
                        data.source.y = (float)(tileset->margin + row * (tileset->tileHeight + tileset->spacing));
                        data.source.width = (float)tileset->tileWidth;
                        data.source.height = (float)tileset->tileHeight;
                        data.valid = true;
                    }
                    return data; // Encontramos el tileset, retornamos inmediatamente
                }
            }
        }
        return data;
    }
};
