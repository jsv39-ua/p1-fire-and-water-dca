#include <MapLoader.hpp>
#include <Platform.hpp>
#include <ElementType.hpp>
#include <ResourceManager.hpp>

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
std::vector<Platform> MapLoader::extractPlatforms() {
    std::vector<Platform> platforms;
    
    if (this->mapa == nullptr) {
        TraceLog(LOG_WARNING, "No hay mapa cargado para extraer plataformas.");
        return platforms;
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
                        // Mapear GID a tileset y tile local para obtener propiedades
                        ElementType pType = NONE;
                        for (uint32_t t = 0; t < this->mapa->tilesetsLength; t++) {
                            TmxTileset* tileset = &this->mapa->tilesets[t];
                            if (gid >= tileset->firstGid && (t == this->mapa->tilesetsLength - 1 || gid < this->mapa->tilesets[t+1].firstGid)) {
                                uint32_t localId = gid - tileset->firstGid;
                                // buscar el tile declarado en el tileset
                                for (uint32_t tt = 0; tt < tileset->tilesLength; tt++) {
                                    TmxTilesetTile* tile = &tileset->tiles[tt];
                                    if (tile->id == localId) {
                                        // Si tiene propiedades, buscar la property 'type'
                                        if (tile->propertiesLength > 0) {
                                            for (uint32_t pi = 0; pi < tile->propertiesLength; ++pi) {
                                                TmxProperty* prop = &tile->properties[pi];
                                                if (prop->type == PROPERTY_TYPE_STRING && strcmp(prop->name, "type") == 0) {
                                                    if (strcmp(prop->stringValue, "water") == 0) pType = WATER;
                                                    else if (strcmp(prop->stringValue, "fire") == 0) pType = FIRE;
                                                    else if (strcmp(prop->stringValue, "goal") == 0) pType = GOAL;
                                                }
                                            }
                                        }
                                        break; // Encontrado tile correspondiente
                                    }
                                }
                                break; // Encontrado tileset correcto
                            }
                        }

                        // fallback: si la capa tiene nombre, usarlo
                        if (pType == NONE && layer->name != NULL) {
                            if (strcmp(layer->name, "Water") == 0) pType = WATER;
                            else if (strcmp(layer->name, "Fire") == 0) pType = FIRE;
                            else if (strcmp(layer->name, "Goal") == 0) pType = GOAL;
                        }

                        Rectangle collision = {
                            (float)(x * this->mapa->tileWidth),
                            (float)(y * this->mapa->tileHeight),
                            (float)this->mapa->tileWidth,
                            (float)this->mapa->tileHeight
                        };
                        Platform platform = { collision, pType };
                        platforms.push_back(platform);
                    }
                }
            }
        }
    }
    
    TraceLog(LOG_INFO, "Plataformas extraídas: %d (Optimizado)", platforms.size());
    return platforms;
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


std::vector<Enemy> MapLoader::extractEnemies() {
    std::vector<Enemy> enemies;
    if (this->mapa == nullptr) return enemies;

    for (uint32_t i = 0; i < this->mapa->layersLength; i++) {
        TmxLayer* layer = &this->mapa->layers[i];

        if (layer->type == LAYER_TYPE_TILE_LAYER) {
            
            // Verificar si es capa de enemigos
            const char* typeStr = getPropertyStringFromLayer(layer, "elementType", nullptr);
            
            if (typeStr != nullptr) {
                float range = getPropertyFloatFromLayer(layer, "patrolRange", 50.0f);
                
                ElementType eType = FIRE;
                if (strcmp(typeStr, "WATER") == 0) eType = WATER;
                else if (strcmp(typeStr, "GOAL") == 0) eType = GOAL;

                TmxTileLayer* tileLayer = &layer->exact.tileLayer;

                for (uint32_t y = 0; y < tileLayer->height; y++) {
                    for (uint32_t x = 0; x < tileLayer->width; x++) {
                        uint32_t index = y * tileLayer->width + x;
                        uint32_t gid = tileLayer->tiles[index];

                        if (gid > 0) {
                            Rectangle bounds = {
                                (float)(x * this->mapa->tileWidth),
                                (float)(y * this->mapa->tileHeight),
                                (float)this->mapa->tileWidth,
                                (float)this->mapa->tileHeight
                            };

                            // --- AQUÍ ESTÁ EL CAMBIO IMPORTANTE ---
                            // Delegamos la lógica fea a la función auxiliar
                            TileData tData = GetTileDataFromGID(gid);

                            if (tData.valid) {
                                // Constructor con textura (asegúrate de tenerlo en Enemy.hpp)
                                enemies.push_back(Enemy(bounds, eType, range, tData.texture, tData.source));
                            } else {
                                // Constructor sin textura (fallback)
                                enemies.push_back(Enemy(bounds, eType, range));
                            }
                        }
                    }
                }
                
                TraceLog(LOG_INFO, "Capa enemigos procesada: %s. Ocultando capa visual.", layer->name);
                layer->visible = false; // ¡Muy buena idea esto!
            }
        }
    }
    return enemies;
}