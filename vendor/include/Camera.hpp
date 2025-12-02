#pragma once

#include <vector>

extern "C" {
    #include <raylib.h>
}

class GameCamera
{
public:
    GameCamera();
    ~GameCamera() = default;

    // Inicialización
    void init(Vector2 offset, float initialZoom, Rectangle mapBounds);
    
    // Actualización
    void update(float deltaTime);
    
    // Seguimiento de targets
    void setTarget(Vector2 target);
    void setMultipleTargets(const std::vector<Vector2>& targets);
    
    // Control de zoom
    void setZoom(float zoom);
    void adjustZoom(float delta); // Para zoom incremental
    void setZoomLimits(float minZoom, float maxZoom);
    void setDynamicZoom(bool enabled); // Zoom automático basado en distancia entre jugadores
    
    // Configuración de suavizado
    void setSmoothSpeed(float speed);
    void setZoomSmoothSpeed(float speed);
    
    // Límites del mapa
    void setMapBounds(Rectangle bounds);
    
    // Obtener cámara de Raylib
    Camera2D& getRaylibCamera() { return camera; }
    const Camera2D& getRaylibCamera() const { return camera; }
    
    // Utilidades
    Vector2 getWorldPosition(Vector2 screenPosition) const;
    Vector2 getScreenPosition(Vector2 worldPosition) const;
    float getCurrentZoom() const { return currentZoom; }
    float getTargetZoom() const { return targetZoom; }

private:
    Camera2D camera;
    
    // Target de seguimiento
    Vector2 targetPosition;
    bool hasTarget;
    
    // Zoom
    float currentZoom;
    float targetZoom;
    float minZoom;
    float maxZoom;
    bool dynamicZoomEnabled;
    
    // Suavizado
    float smoothSpeed;
    float zoomSmoothSpeed;
    
    // Límites
    Rectangle mapBounds;
    bool constrainToBounds;
    
    // Configuración de pantalla (para cálculos)
    float screenWidth;
    float screenHeight;
    
    // Métodos privados
    void applyBounds();
    void updateDynamicZoom(const std::vector<Vector2>& targets);
    float calculateOptimalZoom(const std::vector<Vector2>& targets);
};
