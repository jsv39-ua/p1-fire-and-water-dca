#include <Camera.hpp>
#include <algorithm>
#include <cmath>

extern "C" {
    #include <raylib.h>
}

GameCamera::GameCamera() 
    : hasTarget(false),
      currentZoom(1.0f),
      targetZoom(1.0f),
      minZoom(0.5f),
      maxZoom(4.0f),
      dynamicZoomEnabled(false),
      smoothSpeed(5.0f),
      zoomSmoothSpeed(3.0f),
      constrainToBounds(true),
      screenWidth(1280.0f),
      screenHeight(720.0f)
{
    camera = { 0 };
    camera.target = { 0, 0 };
    camera.offset = { 640, 360 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    
    targetPosition = { 0, 0 };
    mapBounds = { 0, 0, 0, 0 };
}

void GameCamera::init(Vector2 offset, float initialZoom, Rectangle bounds)
{
    camera.offset = offset;
    currentZoom = initialZoom;
    targetZoom = initialZoom;
    camera.zoom = initialZoom;
    mapBounds = bounds;
    constrainToBounds = true;
    
    screenWidth = offset.x * 2.0f;
    screenHeight = offset.y * 2.0f;
}

void GameCamera::update(float deltaTime)
{
    if (!hasTarget) return;
    
    // Suavizado de la posición
    float posLerpFactor = 1.0f - expf(-smoothSpeed * deltaTime);
    camera.target.x += (targetPosition.x - camera.target.x) * posLerpFactor;
    camera.target.y += (targetPosition.y - camera.target.y) * posLerpFactor;
    
    // Suavizado del zoom
    float zoomLerpFactor = 1.0f - expf(-zoomSmoothSpeed * deltaTime);
    currentZoom += (targetZoom - currentZoom) * zoomLerpFactor;
    camera.zoom = currentZoom;
    
    // Aplicar límites del mapa
    if (constrainToBounds) {
        applyBounds();
    }
    
    // Redondear para evitar sub-pixel rendering y evitar lineas blancas parpadeantes
    camera.target.x = roundf(camera.target.x);
    camera.target.y = roundf(camera.target.y);
}

// Por si alguna vez queremos tener solo un jugador
void GameCamera::setTarget(Vector2 target)
{
    targetPosition = target;
    hasTarget = true;
}

void GameCamera::setMultipleTargets(const std::vector<Vector2>& targets)
{
    if (targets.empty()) {
        hasTarget = false;
        return;
    }
    
    Vector2 midPoint = { 0, 0 };
    for (const auto& target : targets) {
        midPoint.x += target.x;
        midPoint.y += target.y;
    }
    midPoint.x /= targets.size();
    midPoint.y /= targets.size();
    
    targetPosition = midPoint;
    hasTarget = true;
    
    if (dynamicZoomEnabled && targets.size() > 1) {
        updateDynamicZoom(targets);
    }
}

void GameCamera::setZoom(float zoom)
{
    targetZoom = fmaxf(minZoom, fminf(zoom, maxZoom));
}

void GameCamera::adjustZoom(float delta)
{
    setZoom(targetZoom + delta);
}

void GameCamera::setZoomLimits(float minZ, float maxZ)
{
    minZoom = minZ;
    maxZoom = maxZ;
    targetZoom = fmaxf(minZoom, fminf(targetZoom, maxZoom));
}

void GameCamera::setDynamicZoom(bool enabled)
{
    dynamicZoomEnabled = enabled;
}

void GameCamera::setSmoothSpeed(float speed)
{
    smoothSpeed = fmaxf(0.1f, speed);
}

void GameCamera::setZoomSmoothSpeed(float speed)
{
    zoomSmoothSpeed = fmaxf(0.1f, speed);
}

void GameCamera::setMapBounds(Rectangle bounds)
{
    mapBounds = bounds;
    constrainToBounds = true;
}

void GameCamera::applyBounds()
{
    float visibleWidth = screenWidth / currentZoom;
    float visibleHeight = screenHeight / currentZoom;
    
    float minX = mapBounds.x + visibleWidth / 2.0f;
    float maxX = mapBounds.x + mapBounds.width - visibleWidth / 2.0f;
    float minY = mapBounds.y + visibleHeight / 2.0f;
    float maxY = mapBounds.y + mapBounds.height - visibleHeight / 2.0f;
    
    // Confinamos la cámara dentro de los límites del mapa para que no se pueda ver fuera de las paredes del mapa

    if (mapBounds.width > visibleWidth) {
        camera.target.x = fmaxf(minX, fminf(camera.target.x, maxX));
    } else {
        camera.target.x = mapBounds.x + mapBounds.width / 2.0f;
    }
    
    if (mapBounds.height > visibleHeight) {
        camera.target.y = fmaxf(minY, fminf(camera.target.y, maxY));
    } else {
        camera.target.y = mapBounds.y + mapBounds.height / 2.0f;
    }
}

void GameCamera::updateDynamicZoom(const std::vector<Vector2>& targets)
{
    float optimalZoom = calculateOptimalZoom(targets);
    setZoom(optimalZoom);
}

float GameCamera::calculateOptimalZoom(const std::vector<Vector2>& targets)
{
    if (targets.size() < 2) {
        return targetZoom;
    }
    
    // Encontrar la distancia máxima entre jugadores
    float maxDistance = 0.0f;
    for (size_t i = 0; i < targets.size(); i++) {
        for (size_t j = i + 1; j < targets.size(); j++) {
            float dx = targets[i].x - targets[j].x;
            float dy = targets[i].y - targets[j].y;
            float distance = sqrtf(dx * dx + dy * dy);
            maxDistance = fmaxf(maxDistance, distance);
        }
    }
    
    // Calcular zoom basado en la distancia
    
    const float minDistanceForZoom = 200.0f;  // Distancia para máximo zoom
    const float maxDistanceForZoom = 800.0f;  // Distancia para mínimo zoom
    
    float normalizedDistance = (maxDistance - minDistanceForZoom) / (maxDistanceForZoom - minDistanceForZoom);
    normalizedDistance = fmaxf(0.0f, fminf(1.0f, normalizedDistance));
    
    // Interpolar entre maxZoom y minZoom
    float optimalZoom = maxZoom - (normalizedDistance * (maxZoom - minZoom));
    
    return optimalZoom;
}

Vector2 GameCamera::getWorldPosition(Vector2 screenPosition) const
{
    return GetScreenToWorld2D(screenPosition, camera);
}

Vector2 GameCamera::getScreenPosition(Vector2 worldPosition) const
{
    return GetWorldToScreen2D(worldPosition, camera);
}
