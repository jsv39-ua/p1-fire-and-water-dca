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

void MapLoader::drawTileLayer(TmxLayer* layer, TmxTileLayer* tileLayer) {
    for (uint32_t y = 0; y < tileLayer->height; y++) {
        for (uint32_t x = 0; x < tileLayer->width; x++) {
            uint32_t index = y * tileLayer->width + x;
            uint32_t gid = tileLayer->tiles[index];
            
            if (gid == 0) continue; // Tile vacío
            
            TileData tData = GetTileDataFromGID(gid);
            
            if (tData.valid) {
                Vector2 position = {
                    (float)(x * this->mapa->tileWidth) + layer->offsetX,
                    (float)(y * this->mapa->tileHeight) + layer->offsetY
                };
                
                Rectangle dest = {
                    position.x,
                    position.y,
                    (float)this->mapa->tileWidth,
                    (float)this->mapa->tileHeight
                };
                
                DrawTexturePro(tData.texture, 
                              tData.source, 
                              dest, 
                              (Vector2){0, 0}, 
                              0.0f, 
                              WHITE);
            }
        }
    }
}

void MapLoader::draw(const Camera2D& camera) {
    if (this->mapa == nullptr) {
        TraceLog(LOG_WARNING, "No hay mapa cargado para dibujar.");
        return;
    }

    // Renderizar capas manualmente para aplicar parallax correctamente
    for (uint32_t i = 0; i < this->mapa->layersLength; i++) {
        TmxLayer* layer = &this->mapa->layers[i];
        
        if (!layer->visible) continue;

        // Renderizar capas de imagen con efecto parallax
        if (layer->type == LAYER_TYPE_IMAGE_LAYER) {
            TmxImageLayer* imgLayer = &layer->exact.imageLayer;
            
            if (imgLayer->image.texture.id > 0) {
                // Aplicar parallax usando parallaxX y parallaxY
                float parallaxX = layer->parallaxX;
                float parallaxY = layer->parallaxY;
                
                // Calcular offset con parallax
                // La posición de la cámara afecta el fondo según el factor de parallax
                float offsetX = layer->offsetX - (camera.target.x * (1.0f - parallaxX));
                float offsetY = layer->offsetY - (camera.target.y * (1.0f - parallaxY));
                
                // Dibujar la imagen de fondo con el offset de parallax
                DrawTextureEx(imgLayer->image.texture, 
                             (Vector2){offsetX, offsetY}, 
                             0.0f, 
                             1.0f, 
                             WHITE);
            }
        }
        // Renderizar capas de tiles normalmente
        else if (layer->type == LAYER_TYPE_TILE_LAYER) {
            TmxTileLayer* tileLayer = &layer->exact.tileLayer;
            drawTileLayer(layer, tileLayer);
        }
    }
}

void MapLoader::drawParallaxLayers(const Camera2D& camera) {
    if (this->mapa == nullptr) return;

    // Solo dibujar capas de imagen con parallax
    for (uint32_t i = 0; i < this->mapa->layersLength; i++) {
        TmxLayer* layer = &this->mapa->layers[i];
        
        if (!layer->visible) continue;

        if (layer->type == LAYER_TYPE_IMAGE_LAYER) {
            TmxImageLayer* imgLayer = &layer->exact.imageLayer;
            
            if (imgLayer->image.texture.id > 0) {
                float parallaxX = layer->parallaxX;
                float parallaxY = layer->parallaxY;
                
                // Calcular posición con parallax considerando el zoom de la cámara
                float parallaxOffsetX = camera.target.x * (1.0f - parallaxX);
                float parallaxOffsetY = camera.target.y * (1.0f - parallaxY);
                
                // Ajustar por el offset de la capa y el zoom
                float drawX = (layer->offsetX - parallaxOffsetX) * camera.zoom + camera.offset.x;
                float drawY = (layer->offsetY - parallaxOffsetY) * camera.zoom + camera.offset.y;
                
                // Escalar la textura por el zoom de la cámara
                float scale = camera.zoom;
                
                DrawTextureEx(imgLayer->image.texture, 
                             (Vector2){drawX, drawY}, 
                             0.0f, 
                             scale, 
                             WHITE);
            }
        }
    }
}

void MapLoader::drawTileLayers() {
    if (this->mapa == nullptr) return;

    // Solo dibujar capas de tiles
    for (uint32_t i = 0; i < this->mapa->layersLength; i++) {
        TmxLayer* layer = &this->mapa->layers[i];
        
        if (!layer->visible) continue;

        if (layer->type == LAYER_TYPE_TILE_LAYER) {
            TmxTileLayer* tileLayer = &layer->exact.tileLayer;
            drawTileLayer(layer, tileLayer);
        }
    }
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
                        if (pType == GOAL) {
                            TraceLog(LOG_INFO, "GOAL platform found at x=%f y=%f w=%f h=%f", collision.x, collision.y, collision.width, collision.height);
                        }
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
                const char* movementStr = getPropertyStringFromLayer(layer, "movement", "horizontal");
                bool vertical = (movementStr != NULL && strcmp(movementStr, "vertical") == 0);
                
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
                                enemies.push_back(Enemy(bounds, eType, range, tData.texture, tData.source, vertical));
                                TraceLog(LOG_INFO, "Enemy spawned at x=%f y=%f type=%d vertical=%d", bounds.x, bounds.y, eType, vertical);
                            } else {
                                // Constructor sin textura (fallback)
                                enemies.push_back(Enemy(bounds, eType, range, vertical));
                                TraceLog(LOG_INFO, "Enemy spawned at x=%f y=%f type=%d vertical=%d", bounds.x, bounds.y, eType, vertical);
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

std::vector<MapTransition> MapLoader::extractTransitions() {
    std::vector<MapTransition> transitions;
    if (this->mapa == nullptr) return transitions;

    for (uint32_t i = 0; i < this->mapa->layersLength; i++) {
        TmxLayer* layer = &this->mapa->layers[i];

        if (layer->type == LAYER_TYPE_OBJECT_GROUP) {
            if (strcmp(layer->name, "Transitions") == 0) {
                TmxObjectGroup* objGroup = &layer->exact.objectGroup;
                
                for (uint32_t j = 0; j < objGroup->objectsLength; j++) {
                    TmxObject* obj = &objGroup->objects[j];
                    
                    MapTransition transition;
                    transition.bounds = { (float)obj->x, (float)obj->y, (float)obj->width, (float)obj->height };
                    
                    // Default values
                    transition.targetMap = "";
                    transition.targetSpawn = { 0, 0 };
                    transition.direction = "none";
                    transition.transitionType = "instant";

                    for (uint32_t k = 0; k < obj->propertiesLength; k++) {
                        TmxProperty* prop = &obj->properties[k];
                        
                        if (strcmp(prop->name, "targetMap") == 0 && prop->type == PROPERTY_TYPE_STRING) {
                            transition.targetMap = prop->stringValue;
                        } else if (strcmp(prop->name, "targetSpawnX") == 0 && prop->type == PROPERTY_TYPE_FLOAT) {
                            transition.targetSpawn.x = prop->floatValue;
                        } else if (strcmp(prop->name, "targetSpawnY") == 0 && prop->type == PROPERTY_TYPE_FLOAT) {
                            transition.targetSpawn.y = prop->floatValue;
                        } else if (strcmp(prop->name, "direction") == 0 && prop->type == PROPERTY_TYPE_STRING) {
                            transition.direction = prop->stringValue;
                        } else if (strcmp(prop->name, "transitionType") == 0 && prop->type == PROPERTY_TYPE_STRING) {
                            transition.transitionType = prop->stringValue;
                        }
                    }
                    
                    transitions.push_back(transition);
                    TraceLog(LOG_INFO, "Transition loaded: target=%s spawn=(%f,%f)", transition.targetMap.c_str(), transition.targetSpawn.x, transition.targetSpawn.y);
                }
            }
        }
    }
    return transitions;
}