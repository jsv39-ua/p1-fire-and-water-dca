#include <MapLoader.hpp>
#include <raytmx.h>  // Incluir la implementación en el .cpp
#include <iostream>
#include <stdexcept>

MapLoader::MapLoader(const char* filePath)
{
    this->mapa = LoadTMX(filePath);


    if (this->mapa == nullptr) {
        TraceLog(LOG_ERROR, "Error al cargar mapa del archivo: %s", filePath); 
    }
}

MapLoader::MapLoader()  {
    this->mapa = nullptr;
}

MapLoader::~MapLoader() {
        if (mapa != nullptr) {
            UnloadTMX(mapa);
        }
    }

TmxMap* MapLoader::getMapa() const {
    return this->mapa;
}

void MapLoader::loadMapa(const char* filePath) {
    if (this->mapa != nullptr) {
        UnloadTMX(this->mapa);
    }

    this->mapa = LoadTMX(filePath);

    if (this->mapa == nullptr) {
        TraceLog(LOG_ERROR, "Error al cargar mapa del archivo: %s", filePath); 
        throw std::runtime_error("No se pudo cargar el mapa TMX");
    }
}

void MapLoader::unloadMapa() {
    if (this->mapa != nullptr) {
        UnloadTMX(this->mapa);
        this->mapa = nullptr;
    }
}

void MapLoader::draw(const Camera2D& camera) {
    if (this->mapa == nullptr) {
        TraceLog(LOG_WARNING, "No hay mapa cargado para dibujar.");
        return;
    }

    DrawTMX(this->mapa, &camera, 0, 0, WHITE);
}
