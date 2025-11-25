# Documentación de MapLoader 

Creada por Copilot, cogedlo con pinzas. Lo he leído y explica bastante bien el funcionamiento y como usar el metodo MapLoader.

## Descripción
La clase `MapLoader` es responsable de cargar, gestionar y renderizar mapas de Tiled (formato TMX) en el juego. También permite extraer información de colisiones de los tiles del mapa.

## Requisitos Previos

### 1. Crear un mapa en Tiled
1. Abre **Tiled Map Editor**
2. Crea un nuevo mapa con las dimensiones deseadas
3. Configura el tamaño de los tiles (actualmente usando 18x18 píxeles)
4. Guarda el mapa en formato TMX en `assets/levels/`

Aqui teneis un video que explica de forma basica como usar Tiled:

https://www.youtube.com/watch?v=Z_7COcLeYFY

### 2. Configurar colisiones en Tiled
Para que los tiles tengan colisión:

1. **Abrir el Tileset** (archivo `.tsx`) en Tiled
2. **Seleccionar un tile** que quieras que tenga colisión
3. **Abrir el Tile Collision Editor**:
   - `View → Views and Toolbars → Tile Collision Editor`
   - O presiona `Ctrl+Shift+O`
4. **Dibujar un rectángulo de colisión** sobre el tile
5. **Guardar el tileset**

## Uso Básico

### Incluir el Header
```cpp
#include <MapLoader.hpp>
```

### Constructor y Carga de Mapa

#### Opción 1: Constructor con ruta
```cpp
MapLoader mapLoader("assets/levels/nivel1.tmx");
```

#### Opción 2: Constructor vacío + cargar después
```cpp
MapLoader mapLoader;
mapLoader.loadMapa("assets/levels/nivel1.tmx");
```

### Manejo de Errores
El método `loadMapa()` lanza una excepción `std::runtime_error` si falla:

```cpp
try {
    mapLoader.loadMapa("assets/levels/nivel1.tmx");
} catch (const std::runtime_error& e) {
    std::cerr << "Error al cargar mapa: " << e.what() << std::endl;
}
```

## Renderizado del Mapa

### Renderizar con Cámara
```cpp
void render() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    
    // Iniciar modo cámara 2D
    BeginMode2D(camera);
    
    // Dibujar el mapa (se dibuja dentro de BeginMode2D)
    mapLoader.draw(camera);
    
    // ... dibujar otros elementos del juego ...
    
    EndMode2D();
    EndDrawing();
}
```

**Importante:** 
- El mapa **debe dibujarse dentro** de `BeginMode2D()` / `EndMode2D()`
- Pasarle la cámara al método `draw()` aunque no la use directamente
- DrawTMX se llama internamente con `NULL` ya que BeginMode2D aplica la transformación

## Extracción de Colisiones

### Extraer Colisiones de Tiles
El método `extractCollisions()` devuelve un `std::vector<Rectangle>` con las colisiones de todos los tiles que tienen un `objectGroup` configurado en Tiled.

```cpp
// Extraer colisiones del mapa
std::vector<Rectangle> collisions = mapLoader.extractCollisions();

// Convertir a plataformas del juego
for (const auto& collision : collisions) {
    Platform platform = { collision, "default" };
    level_platforms.push_back(platform);
}

TraceLog(LOG_INFO, "Plataformas cargadas: %d", level_platforms.size());
```

### ⚠️ Nota de Rendimiento
El método `extractCollisions()` crea un `Rectangle` individual para cada tile con colisión. Esto puede ser ineficiente si tienes muchos tiles consecutivos. Para mapas grandes, considera optimizar combinando rectángulos adyacentes.

## Ejemplo Completo de Integración

### En MainGameState.hpp
```cpp
class MainGameState : public GameState {
private:
    MapLoader mapLoader;
    std::vector<Platform> level_platforms;
    Camera2D camera;
    // ... otros miembros ...
};
```

### En MainGameState.cpp - init()
```cpp
void MainGameState::init() {
    // Inicializar cámara
    camera = { 0 };
    camera.target = (Vector2){ 0, 0 };
    camera.offset = (Vector2){ 640, 360 };
    camera.rotation = 0.0f;
    camera.zoom = 2.0f;
    
    // Cargar mapa y extraer colisiones
    try {
        mapLoader.loadMapa("assets/levels/nivel1.tmx");
        
        // Extraer colisiones del mapa
        std::vector<Rectangle> collisions = mapLoader.extractCollisions();
        
        // Convertir colisiones en plataformas
        for (const auto& collision : collisions) {
            Platform platform = { collision, "default" };
            level_platforms.push_back(platform);
        }
        
        TraceLog(LOG_INFO, "Plataformas cargadas: %d", level_platforms.size());
        
    } catch (const std::runtime_error& e) {
        std::cerr << "Error al cargar mapa: " << e.what() << std::endl;
        // Manejar error (volver al menú, etc.)
    }
}
```

### En MainGameState.cpp - render()
```cpp
void MainGameState::render() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    
    BeginMode2D(camera);
    
    // Dibujar el mapa
    mapLoader.draw(camera);
    
    // Dibujar personajes y otros elementos...
    
    EndMode2D();
    EndDrawing();
}
```

## API Completa

### Métodos Públicos

#### `MapLoader()`
Constructor por defecto. Crea un MapLoader sin mapa cargado.

#### `MapLoader(const char* filePath)`
Constructor que carga un mapa directamente.
- **Parámetros:** `filePath` - Ruta al archivo TMX
- **Nota:** No lanza excepciones, registra errores en el log

#### `~MapLoader()`
Destructor. Libera automáticamente el mapa cargado.

#### `void loadMapa(const char* filePath)`
Carga un mapa TMX desde archivo.
- **Parámetros:** `filePath` - Ruta al archivo TMX
- **Excepciones:** Lanza `std::runtime_error` si falla la carga

#### `void unloadMapa()`
Descarga el mapa actual y libera la memoria.

#### `void draw(const Camera2D& camera)`
Renderiza el mapa.
- **Parámetros:** `camera` - Referencia a la cámara 2D
- **Nota:** Debe llamarse dentro de `BeginMode2D()` / `EndMode2D()`

#### `std::vector<Rectangle> extractCollisions()`
Extrae las colisiones de todos los tiles que tienen un objectGroup definido.
- **Retorna:** Vector de `Rectangle` con las colisiones
- **Nota:** Cada tile con colisión genera un rectángulo individual

#### `TmxMap* getMapa() const`
Obtiene un puntero al mapa TMX cargado.
- **Retorna:** Puntero al `TmxMap` o `nullptr` si no hay mapa cargado

## Flujo de Trabajo Recomendado

1. **Diseñar nivel en Tiled**
   - Crear capas de tiles
   - Configurar colisiones en el tileset
   - Guardar como `.tmx`

2. **Cargar en el juego**
   ```cpp
   mapLoader.loadMapa("assets/levels/mi_nivel.tmx");
   ```

3. **Extraer colisiones**
   ```cpp
   auto collisions = mapLoader.extractCollisions();
   ```

4. **Renderizar cada frame**
   ```cpp
   BeginMode2D(camera);
   mapLoader.draw(camera);
   EndMode2D();
   ```

## Solución de Problemas

### El mapa no se ve
- ✅ Verificar que estás llamando `draw()` dentro de `BeginMode2D()`
- ✅ Verificar que las rutas de las imágenes en los archivos `.tsx` son correctas
- ✅ Verificar que el zoom de la cámara no es 0

### Las colisiones no funcionan
- ✅ Asegurarse de haber configurado el Tile Collision Editor en Tiled
- ✅ Verificar que `extractCollisions()` devuelve elementos (ver log)
- ✅ Comprobar que los rectángulos se están usando en el sistema de física

### El mapa se ve pequeño
- ✅ Ajustar el `camera.zoom` (valores mayores = más grande)
- ✅ Verificar que la cámara está correctamente configurada

## Notas Técnicas

- El MapLoader usa **raytmx** internamente para cargar archivos TMX
- Soporta múltiples capas de tiles
- Soporta tilesets con spacing y margin
- Las colisiones se extraen del `objectGroup` de cada tile
- La memoria se gestiona automáticamente en el destructor

## Próximas Mejoras

- [ ] Optimizar `extractCollisions()` para combinar rectángulos adyacentes
- [ ] Soportar extracción de objetos (no solo tiles)
- [ ] Agregar soporte para propiedades personalizadas de tiles
- [ ] Cache de colisiones para evitar recalcular cada vez
