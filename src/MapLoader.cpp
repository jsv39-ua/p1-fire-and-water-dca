#include <MapLoader.hpp>
#include <ResourceManager.hpp>
#include <raytmx.h>  // Incluir la implementación en el .cpp
#include <unordered_set>


MapLoader::MapLoader(const char* filePath)
{
    // Cargar mapa desde ResourceManager
    this->mapa = ResourceManager::getInstance()->loadMap(filePath);

    if (this->mapa == nullptr) {
        TraceLog(LOG_ERROR, "Error al cargar mapa del archivo: %s", filePath); 
    }
}

MapLoader::MapLoader()  {
    this->mapa = nullptr;
}

MapLoader::~MapLoader() {
    // No se descarga el mapa
    // se queda en cache para reutilizarlo
    this->mapa = nullptr;
}

TmxMap* MapLoader::getMapa() const {
    return this->mapa;
}

void MapLoader::loadMapa(const char* filePath) {
    // Cargar mapa desde ResourceManager (con cache)
    this->mapa = ResourceManager::getInstance()->loadMap(filePath);

    if (this->mapa == nullptr) {
        TraceLog(LOG_ERROR, "Error al cargar mapa del archivo: %s", filePath); 
        throw std::runtime_error("No se pudo cargar el mapa TMX");
    }
}

void MapLoader::unloadMapa() {
    // limpio la referencia local
    // ResourceManager mantiene el mapa en caché
    this->mapa = nullptr;
}

void MapLoader::draw(const Camera2D& camera) {
    if (this->mapa == nullptr) {
        TraceLog(LOG_WARNING, "No hay mapa cargado para dibujar.");
        return;
    }

    // Pasar NULL como cámara porque BeginMode2D ya aplica la transformación
    DrawTMX(this->mapa, NULL, 0, 0, WHITE);
}



// Extrae las colisiones del mapa TMX cargado
// OPTIMIZADO: Pre-cachea los GIDs sólidos para búsqueda O(1)
std::vector<Rectangle> MapLoader::extractCollisions() {
    std::vector<Rectangle> collisions;
    
    if (this->mapa == nullptr) {
        TraceLog(LOG_WARNING, "No hay mapa cargado para extraer colisiones.");
        return collisions;
    }

    // PASO 1: Pre-cachear qué GIDs son sólidos.
    // Esto elimina la necesidad de buscar en los tilesets dentro del bucle del mapa.
    std::unordered_set<uint32_t> solidGids;

    for (uint32_t t = 0; t < this->mapa->tilesetsLength; t++) {
        TmxTileset* tileset = &this->mapa->tilesets[t];
        
        for (uint32_t tt = 0; tt < tileset->tilesLength; tt++) {
            TmxTilesetTile* tile = &tileset->tiles[tt];
            
            // Si el tile tiene objetos definidos en Tiled, se considera sólido
            if (tile->objectGroup.objectsLength > 0) {
                // Calculamos el GID global real
                uint32_t globalId = tileset->firstGid + tile->id;
                solidGids.insert(globalId);
            }
        }
    }

    // PASO 2: Recorrer el mapa y generar rectángulos solo si el GID está en el set.
    for (uint32_t i = 0; i < this->mapa->layersLength; i++) {
        TmxLayer* layer = &this->mapa->layers[i];
        
        if (layer->type == LAYER_TYPE_TILE_LAYER) {
            // Opcional: Si tienes una capa específica llamada "Colisiones",
            // descomenta esto para ignorar capas decorativas y ganar más rendimiento:
            // if (strcmp(layer->name, "Colisiones") != 0) continue;

            TmxTileLayer* tileLayer = &layer->exact.tileLayer;
            
            for (uint32_t y = 0; y < tileLayer->height; y++) {
                for (uint32_t x = 0; x < tileLayer->width; x++) {
                    uint32_t index = y * tileLayer->width + x;
                    uint32_t gid = tileLayer->tiles[index];
                    
                    // Verificación O(1) gracias al unordered_set
                    if (gid > 0 && solidGids.count(gid)) {
                        Rectangle collision = {
                            (float)(x * this->mapa->tileWidth),
                            (float)(y * this->mapa->tileHeight),
                            (float)this->mapa->tileWidth,
                            (float)this->mapa->tileHeight
                        };
                        collisions.push_back(collision);
                    }
                }
            }
        }
    }
    
    TraceLog(LOG_INFO, "Colisiones extraídas: %d (Optimizado)", collisions.size());
    return collisions;
}

// Obtener los límites del mapa en píxeles
Rectangle MapLoader::getMapBounds() const {
    if (this->mapa == nullptr) {
        TraceLog(LOG_WARNING, "No hay mapa cargado para obtener límites.");
        return {0, 0, 0, 0};
    }
    
    return {
        0.0f,
        0.0f,
        (float)(this->mapa->width * this->mapa->tileWidth),
        (float)(this->mapa->height * this->mapa->tileHeight)
    };
}
