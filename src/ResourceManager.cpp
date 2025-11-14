#include <ResourceManager.hpp>
#include <raytmx.h>

ResourceManager* ResourceManager::instance = nullptr;

ResourceManager::ResourceManager() {}

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
        return it->second;
    }
    
    // Si no esta cargo desde disco
    Texture2D texture = LoadTexture(filePath.c_str());
    
    // Guardar en cache si la carga fue exitosa
    if (texture.id != 0) {
        textureCache[filePath] = texture;
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
        return it->second;
    }
    
    // Si no esta cargo desde disco
    Sound sound = LoadSound(filePath.c_str());
    
    // Guardar en cache si la carga fue exitosa
    if (IsSoundValid(sound)) {
        soundCache[filePath] = sound;
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
        return it->second;
    }
    
    // Si no esta cargo desde disco
    Music music = LoadMusicStream(filePath.c_str());
    
    // Guardar en cache si la carga fue exitosa
    if (IsMusicValid(music)) {
        musicCache[filePath] = music;
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
        return it->second;
    }
    
    // Si no esta cargo desde disco
    TmxMap* map = LoadTMX(filePath.c_str());
    
    // Guardar en cache si la carga fue exitosa
    if (map != nullptr) {
        mapCache[filePath] = map;
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
