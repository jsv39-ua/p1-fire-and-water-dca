#include <ResourceManager.hpp>
#include <raytmx.h>

ResourceManager* ResourceManager::instance = nullptr;

ResourceManager::ResourceManager() {}
#include "raylib_compat.h"

ResourceManager* ResourceManager::getInstance() {
    if (instance == nullptr) {
        instance = new ResourceManager();
    }
    return instance;
}

void ResourceManager::cleanup() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

ResourceManager::~ResourceManager() {
    clearAllCache();
}

// TEXTURAS
Texture2D ResourceManager::loadTexture(const std::string& filePath) {
    auto it = textureCache.find(filePath);
    
    // Si esta en cache lo devuelvo directamente
    if (it != textureCache.end()) {
        TraceLog(LOG_INFO, "[ResourceManager] Textura cargada desde CACHE: %s", filePath.c_str());
        return it->second;
    }
    
    // Si no esta cargo desde disco
    TraceLog(LOG_INFO, "[ResourceManager] Cargando textura desde DISCO: %s", filePath.c_str());
    Texture2D texture = LoadTexture(filePath.c_str());
    
    // Guardar en cache si la carga fue exitosa
    if (texture.id != 0) {
        textureCache[filePath] = texture;
        TraceLog(LOG_INFO, "[ResourceManager] Textura guardada en caché exitosamente");
    } else {
        TraceLog(LOG_ERROR, "[ResourceManager] Error al cargar textura desde disco");
    }
    
    return texture;
}

// Descarga y elimina textura del cache (no se utiliza de momento, habria que usarlo si se quiere liberar memoria)
void ResourceManager::unloadTexture(const std::string& filePath) {
    auto it = textureCache.find(filePath);
    if (it != textureCache.end()) {
        UnloadTexture(it->second);
        textureCache.erase(it);
    }
}

// SONIDOS
Sound ResourceManager::loadSound(const std::string& filePath) {
    auto it = soundCache.find(filePath);
    
    // Si esta en cache lo devuelvo directamente
    if (it != soundCache.end()) {
        TraceLog(LOG_INFO, "[ResourceManager] Sonido cargado desde CACHE: %s", filePath.c_str());
        return it->second;
    }
    
    // Si no esta cargo desde disco
    TraceLog(LOG_INFO, "[ResourceManager] Cargando sonido desde DISCO: %s", filePath.c_str());
    Sound sound = LoadSound(filePath.c_str());
    
    // Guardar en cache si la carga fue exitosa
    if (IsSoundValid(sound)) {
        soundCache[filePath] = sound;
        TraceLog(LOG_INFO, "[ResourceManager] Sonido guardado en caché exitosamente");
    } else {
        TraceLog(LOG_ERROR, "[ResourceManager] Error al cargar sonido desde disco");
    }
    
    return sound;
}

void ResourceManager::unloadSound(const std::string& filePath) {
    auto it = soundCache.find(filePath);
    if (it != soundCache.end()) {
        UnloadSound(it->second);
        soundCache.erase(it);
    }
}

// MÚSICA
Music ResourceManager::loadMusic(const std::string& filePath) {
    auto it = musicCache.find(filePath);
    
    // Si esta en cache lo devuelvo directamente
    if (it != musicCache.end()) {
        TraceLog(LOG_INFO, "[ResourceManager] Música cargada desde CACHE: %s", filePath.c_str());
        return it->second;
    }
    
    // Si no esta cargo desde disco
    TraceLog(LOG_INFO, "[ResourceManager] Cargando música desde DISCO: %s", filePath.c_str());
    Music music = LoadMusicStream(filePath.c_str());
    
    // Guardar en cache si la carga fue exitosa
    if (IsMusicValid(music)) {
        musicCache[filePath] = music;
        TraceLog(LOG_INFO, "[ResourceManager] Música guardada en caché exitosamente");
    } else {
        TraceLog(LOG_ERROR, "[ResourceManager] Error al cargar música desde disco");
    }
    
    return music;
}

// lo mismo que el de arriba (descargar y borrar del cache)
void ResourceManager::unloadMusic(const std::string& filePath) {
    auto it = musicCache.find(filePath);
    if (it != musicCache.end()) {
        UnloadMusicStream(it->second);
        musicCache.erase(it);
    }
}

// MAPAS TMX
TmxMap* ResourceManager::loadMap(const std::string& filePath) {
    auto it = mapCache.find(filePath);
    
    // Si esta en cache lo devuelvo directamente
    if (it != mapCache.end()) {
        TraceLog(LOG_INFO, "[ResourceManager] Mapa TMX cargado desde CACHE: %s", filePath.c_str());
        return it->second;
    }
    
    // Si no esta cargo desde disco
    TraceLog(LOG_INFO, "[ResourceManager] Cargando mapa TMX desde DISCO: %s", filePath.c_str());
    TmxMap* map = LoadTMX(filePath.c_str());
    
    // Guardar en cache si la carga fue exitosa
    if (map != nullptr) {
        mapCache[filePath] = map;
        TraceLog(LOG_INFO, "[ResourceManager] Mapa TMX guardado en caché exitosamente");
    } else {
        TraceLog(LOG_ERROR, "[ResourceManager] Error al cargar mapa TMX desde disco");
    }
    
    return map;
}

// lo mismo que el de arriba (descargar y borrar del cache)
void ResourceManager::unloadMap(const std::string& filePath) {
    auto it = mapCache.find(filePath);
    if (it != mapCache.end()) {
        UnloadTMX(it->second);
        mapCache.erase(it);
    }
}

// LIMPIEZA GENERAL
void ResourceManager::clearAllCache() {
    // Liberar todas las texturas
    for (auto& pair : textureCache) {
        UnloadTexture(pair.second);
    }
    textureCache.clear();
    
    // Liberar todos los sonidos
    for (auto& pair : soundCache) {
        UnloadSound(pair.second);
    }
    soundCache.clear();
    
    // Liberar toda la música
    for (auto& pair : musicCache) {
        UnloadMusicStream(pair.second);
    }
    musicCache.clear();
    
    // Liberar todos los mapas
    for (auto& pair : mapCache) {
        UnloadTMX(pair.second);
    }
    mapCache.clear();
}
