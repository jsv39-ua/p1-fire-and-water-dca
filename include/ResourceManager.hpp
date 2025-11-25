#pragma once

#include <string>
#include <unordered_map>

extern "C" {
    #include <raylib.h>
}

typedef struct tmx_map TmxMap;

// Gestor de recursos con sistema de cache
class ResourceManager {
private:
    static ResourceManager* instance;
    
    // Caches para evitar cargas repetidas desde disco
    std::unordered_map<std::string, Texture2D> textureCache;
    std::unordered_map<std::string, Sound> soundCache;
    std::unordered_map<std::string, Music> musicCache;
    std::unordered_map<std::string, TmxMap*> mapCache;
    
    ResourceManager();
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

public:
    // Obtiene la instancia unica del gestor
    static ResourceManager* getInstance();
    // Destruye la instancia del gestor
    static void cleanup();

    // Carga textura
    Texture2D loadTexture(const std::string& filePath);
    // Descarga y elimina textura del caché
    void unloadTexture(const std::string& filePath);
    
    // Carga sonido
    Sound loadSound(const std::string& filePath);
    // Descarga y elimina sonido del caché
    void unloadSound(const std::string& filePath);

    // Carga música
    Music loadMusic(const std::string& filePath);
    // Descarga y elimina música del cache
    void unloadMusic(const std::string& filePath);

    // Carga mapa TMX
    TmxMap* loadMap(const std::string& filePath);
    // Descarga y elimina mapa del cache
    void unloadMap(const std::string& filePath);

    // Libera todos los recursos cacheados
    void clearAllCache();
    
    ~ResourceManager();
};
