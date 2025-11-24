#include <MapLoader.hpp>
#include <ResourceManager.hpp>
#include <raytmx.h>  // Incluir la implementación en el .cpp


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



// Este metodo de extraer las colisiones es muy ineficiente
// porque asigna un rectangulo individual a cada tile.
// Si hay muchos tiles con colision, esto puede ser un problema de rendimiento

// Extrae las colisiones del mapa TMX cargado
std::vector<Rectangle> MapLoader::extractCollisions() {
    std::vector<Rectangle> collisions;
    
    if (this->mapa == nullptr) {
        TraceLog(LOG_WARNING, "No hay mapa cargado para extraer colisiones.");
        return collisions;
    }

    // Iterar por todas las capas del mapa
    for (uint32_t i = 0; i < this->mapa->layersLength; i++) {
        TmxLayer* layer = &this->mapa->layers[i];
        
        // Solo procesar capas de tipo tile
        if (layer->type == LAYER_TYPE_TILE_LAYER) {
            TmxTileLayer* tileLayer = &layer->exact.tileLayer;
            
            // Iterar por todos los tiles de la capa
            for (uint32_t y = 0; y < tileLayer->height; y++) {
                for (uint32_t x = 0; x < tileLayer->width; x++) {
                    uint32_t index = y * tileLayer->width + x;
                    uint32_t gid = tileLayer->tiles[index];
                    
                    // Si el tile no está vacío (gid > 0)
                    if (gid > 0) {
                        // Buscar el tileset que contiene este GID
                        for (uint32_t t = 0; t < this->mapa->tilesetsLength; t++) {
                            TmxTileset* tileset = &this->mapa->tilesets[t];
                            
                            // Si el GID está en el rango de este tileset
                            if (gid >= tileset->firstGid && gid <= tileset->lastGid) {
                                uint32_t localId = gid - tileset->firstGid;
                                
                                // Buscar el tile específico en el tileset
                                for (uint32_t tt = 0; tt < tileset->tilesLength; tt++) {
                                    TmxTilesetTile* tile = &tileset->tiles[tt];
                                    
                                    // Si encontramos el tile y tiene objetos de colisión
                                    if (tile->id == localId && tile->objectGroup.objectsLength > 0) {
                                        // Agregar un rectángulo de colisión por cada tile con colisión
                                        Rectangle collision = {
                                            (float)(x * this->mapa->tileWidth),
                                            (float)(y * this->mapa->tileHeight),
                                            (float)this->mapa->tileWidth,
                                            (float)this->mapa->tileHeight
                                        };
                                        collisions.push_back(collision);
                                    }
                                }
                                break; // Ya encontramos el tileset, salir del loop
                            }
                        }
                    }
                }
            }
        }
    }
    
    TraceLog(LOG_INFO, "Colisiones extraídas del mapa: %d", collisions.size());
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
