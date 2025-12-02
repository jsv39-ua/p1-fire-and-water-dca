# Guía de Uso: Clase GameCamera

## Descripción

La clase `GameCamera` es una abstracción de la cámara 2D de Raylib diseñada para facilitar el seguimiento de jugadores, control de zoom y gestión de límites del mapa.

## Características

### ✨ Funcionalidades Principales

1. **Seguimiento Suavizado**
   - Seguimiento smooth de un solo target
   - Seguimiento automático de múltiples jugadores (calcula punto medio)
   - Velocidad de suavizado configurable

2. **Control de Zoom**
   - Zoom manual (con límites configurables)
   - Zoom dinámico automático basado en distancia entre jugadores
   - Transiciones suaves de zoom

3. **Límites del Mapa**
   - Restringe la cámara dentro de los bounds del mapa
   - Maneja mapas más pequeños que la pantalla

4. **Utilidades**
   - Conversión screen ↔ world coordinates
   - Renderizado sub-pixel evitado (redondeo automático)

## Uso Básico

### Inicialización

```cpp
// En MainGameState::init()
Rectangle mapBounds = mapLoader.getMapBounds();
camera.init(
    {640, 360},  // Offset (centro de pantalla para 1280x720)
    2.0f,        // Zoom inicial
    mapBounds    // Límites del mapa
);
```

### Configuración

```cpp
// Velocidad de seguimiento (mayor = más rápido, menos suave)
camera.setSmoothSpeed(5.0f);

// Velocidad de transición de zoom
camera.setZoomSmoothSpeed(3.0f);

// Límites de zoom
camera.setZoomLimits(1.0f, 3.0f);  // min, max

// Habilitar zoom dinámico (opcional)
camera.setDynamicZoom(true);
```

### Actualización (cada frame)

```cpp
// Seguir múltiples jugadores
std::vector<Vector2> playerPositions = {
    players[0].position, 
    players[1].position
};
camera.setMultipleTargets(playerPositions);
camera.update(deltaTime);

// O seguir un solo target
camera.setTarget(player.position);
camera.update(deltaTime);
```

### Renderizado

```cpp

BeginMode2D(camera.getRaylibCamera());
    
    mapLoader.draw(camera.getRaylibCamera());
    // ...
EndMode2D();
```

## API Detallada

### Métodos de Configuración

| Método | Descripción |
|--------|-------------|
| `init(offset, zoom, bounds)` | Inicializa la cámara con configuración básica |
| `setSmoothSpeed(speed)` | Configura velocidad de seguimiento (default: 5.0) |
| `setZoomSmoothSpeed(speed)` | Configura velocidad de zoom (default: 3.0) |
| `setZoomLimits(min, max)` | Define rangos de zoom permitidos |
| `setMapBounds(bounds)` | Actualiza límites del mapa |

### Métodos de Control

| Método | Descripción |
|--------|-------------|
| `setTarget(target)` | Establece un solo punto a seguir |
| `setMultipleTargets(targets)` | Sigue múltiples puntos (calcula centro) |
| `setZoom(zoom)` | Establece zoom específico |
| `adjustZoom(delta)` | Incrementa/decrementa zoom actual |
| `setDynamicZoom(enabled)` | Activa/desactiva zoom automático |
| `update(deltaTime)` | Actualiza posición y zoom (llamar cada frame) |

### Métodos de Consulta

| Método | Descripción |
|--------|-------------|
| `getRaylibCamera()` | Obtiene la Camera2D de Raylib |
| `getCurrentZoom()` | Retorna zoom actual |
| `getTargetZoom()` | Retorna zoom objetivo |
| `getWorldPosition(screenPos)` | Convierte coordenadas screen → world |
| `getScreenPosition(worldPos)` | Convierte coordenadas world → screen |

## Ejemplos de Uso Avanzado


### Zoom Dinámico Personalizado

```cpp
// Desactivar zoom automático
camera.setDynamicZoom(false);

// Calcular zoom basado en tu lógica
float distanceBetweenPlayers = Vector2Distance(
    players[0].position, 
    players[1].position
);

if (distanceBetweenPlayers > 500.0f) {
    camera.setZoom(1.5f);
} else {
    camera.setZoom(2.5f);
}
```

### Shake de Cámara (Futuro)

```cpp
// Agregar estos métodos a GameCamera:
void shake(float intensity, float duration);
void updateShake(float deltaTime);
```

## Configuración Actual en MainGameState

```cpp
// Valores actuales configurados:
- Offset: {640, 360}  // Centro para 1280x720
- Zoom Inicial: 2.0f
- Smooth Speed: 5.0f
- Zoom Smooth Speed: 3.0f
- Zoom Límites: 1.0f - 3.0f
- Dynamic Zoom: false (desactivado por defecto)
```

## Mejoras Futuras Sugeridas

1. **Camera Shake**: Para efectos de impacto
2. **Zonas de Cámara**: Diferentes configuraciones por área del mapa
3. **Interpolación Personalizada**: Easing functions (ease-in, ease-out)
4. **Dead Zones**: Áreas donde el jugador puede moverse sin mover la cámara
5. **Look-Ahead**: Anticipar movimiento del jugador
6. **Parallax Support**: Para fondos con efecto parallax

## Notas Técnicas

- La cámara redondea las posiciones para evitar sub-pixel rendering
- El zoom dinámico calcula distancia máxima entre todos los targets
- Los límites del mapa se aplican automáticamente en cada update()
- La interpolación usa `1.0f - exp(-speed * deltaTime)` para suavizado exponencial

## Migración desde Camera2D

Si tenías código usando `Camera2D` directamente:

```cpp
// Antes:
BeginMode2D(camera);
mapLoader.draw(camera);

// Ahora:
BeginMode2D(camera.getRaylibCamera());
mapLoader.draw(camera.getRaylibCamera());
```

## Troubleshooting

**Problema**: La cámara no sigue a los jugadores
- **Solución**: Asegúrate de llamar `setTarget()` o `setMultipleTargets()` antes de `update()`

**Problema**: El zoom no cambia
- **Solución**: Verifica que el zoom esté dentro de los límites configurados con `setZoomLimits()`

**Problema**: La cámara se mueve muy rápido/lento
- **Solución**: Ajusta `smoothSpeed` (valores típicos: 3.0 - 10.0)

**Problema**: La cámara se sale del mapa
- **Solución**: Verifica que `setMapBounds()` se haya llamado correctamente con los bounds del mapa
