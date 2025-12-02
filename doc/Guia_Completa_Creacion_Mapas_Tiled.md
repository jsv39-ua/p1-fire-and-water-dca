# 📋 Guía Completa de Creación e Importación de Mapas con Tiled

## 📚 Índice

1. [Introducción al Sistema](#1-introducción-al-sistema)
2. [Prerequisitos y Herramientas](#2-prerequisitos-y-herramientas)
3. [Estructura del Sistema de Mapas](#3-estructura-del-sistema-de-mapas)
4. [Preparación de Recursos](#4-preparación-de-recursos)
5. [Creación del Tileset en Tiled](#5-creación-del-tileset-en-tiled)
6. [Configuración de Colisiones](#6-configuración-de-colisiones)
7. [Creación del Mapa](#7-creación-del-mapa)
8. [Integración en el Proyecto](#8-integración-en-el-proyecto)
9. [Sistema de Caché y Optimización](#9-sistema-de-caché-y-optimización)
10. [Resolución de Problemas](#10-resolución-de-problemas)
11. [Mejores Prácticas](#11-mejores-prácticas)
12. [Ejemplos Prácticos](#12-ejemplos-prácticos)

---

## 1. Introducción al Sistema

### ¿Cómo Funciona el Sistema de Mapas?

Tu proyecto utiliza una arquitectura en capas para cargar y renderizar mapas:

```
┌─────────────────────────────────────────────────────────┐
│                    ARQUITECTURA                         │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  MainGameState.cpp                                      │
│       ↓                                                 │
│  MapLoader (Gestión del mapa)                          │
│       ↓                                                 │
│  ResourceManager (Sistema de caché)                     │
│       ↓                                                 │
│  raytmx (Biblioteca de carga TMX)                       │
│       ↓                                                 │
│  Raylib (Renderizado)                                   │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

### Componentes Principales

| Componente | Archivo | Función |
|------------|---------|---------|
| **MapLoader** | `src/MapLoader.cpp` | Carga y dibuja mapas TMX |
| **ResourceManager** | `src/ResourceManager.cpp` | Gestiona caché de recursos |
| **raytmx** | `vendor/include/raytmx.h` | Parsea archivos TMX |
| **MainGameState** | `src/MainGameState.cpp` | Integra el mapa en el juego |

### Flujo de Trabajo Completo

```
[1. Diseño] → [2. Tilesets] → [3. Colisiones] → [4. Mapa] → [5. Integración]
     ↓              ↓               ↓              ↓              ↓
   Tiled        Tiled TSX      Collision      Tiled TMX      C++ Code
                                 Editor
```

---

## 2. Prerequisitos y Herramientas

### Instalación de Tiled Map Editor

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install tiled
```

#### Linux (Flatpak - Recomendado para versiones más recientes)
```bash
flatpak install flathub org.mapeditor.Tiled
flatpak run org.mapeditor.Tiled
```

#### Desde el sitio oficial
```bash
# Descarga desde https://www.mapeditor.org/
# Versión mínima recomendada: 1.10+
```

### Verificar Instalación
```bash
tiled --version
# Debe mostrar: Tiled 1.10.x o superior
```

### Estructura de Directorios del Proyecto

Tu proyecto tiene esta estructura para assets:

```
assets/
├── images/                    # Imágenes generales del juego
├── levels/                    # Carpeta principal de niveles
│   ├── level_prueba/         # Carpeta de niveles de prueba
│   │   ├── nivel1-pantalla2.tmx      ← Archivos de mapa (.tmx)
│   │   └── nivel1-prueba.tmx         ← Archivos de mapa (.tmx)
│   └── tilesets_niveles_prueba/      # Tilesets para niveles
│       ├── TileMap PixelPlatformer.tsx     ← Tileset con colisiones
│       ├── tileset-tiles.tsx               ← Tileset simple
│       ├── tilemap-backgrounds.tsx         ← Backgrounds
│       ├── tilemap-backgrounds2.tsx
│       ├── tileset-characters.tsx          ← Personajes
│       └── kenney_pixel-platformer/        # Recursos gráficos
│           ├── Tilemap/
│           │   ├── tilemap.png             ← Imagen del tileset
│           │   └── tilemap_packed.png
│           ├── Tiles/
│           │   ├── Backgrounds/
│           │   └── Characters/
│           └── Tiled/                      # Ejemplos de Tiled
│               ├── tilemap-example-a.tmx
│               └── tileset-tiles.tsx
├── music/                     # Música del juego
└── ui_sounds/                 # Sonidos de interfaz
```

**IMPORTANTE**: Mantén esta estructura de carpetas para evitar problemas de rutas.

---

## 3. Estructura del Sistema de Mapas

### Formato TMX (Tiled Map XML)

Un archivo `.tmx` es un archivo XML que define:
- Dimensiones del mapa (ancho × alto en tiles)
- Tamaño de cada tile (18×18 píxeles en tu proyecto)
- Referencias a tilesets (archivos `.tsx`)
- Capas de tiles
- Propiedades personalizadas

#### Ejemplo de Archivo TMX

```xml
<?xml version="1.0" encoding="UTF-8"?>
<map version="1.10" 
     tiledversion="1.11.2" 
     orientation="orthogonal" 
     renderorder="right-down" 
     width="30"              <!-- 30 tiles de ancho -->
     height="20"             <!-- 20 tiles de alto -->
     tilewidth="18"          <!-- 18 píxeles por tile -->
     tileheight="18"         <!-- 18 píxeles por tile -->
     infinite="0">
  
  <!-- Referencia al tileset externo -->
  <tileset firstgid="1" source="../tilesets_niveles_prueba/TileMap PixelPlatformer.tsx"/>
  
  <!-- Capa de plataformas -->
  <layer id="1" name="Plataformas" width="30" height="20">
    <data encoding="csv">
      0,0,0,0,0,0,0,0,0,0,
      0,182,183,183,184,0,0,0,0,0,
      <!-- ... más datos -->
    </data>
  </layer>
  
  <!-- Capa decorativa (foreground) -->
  <layer id="2" name="Foreground" width="30" height="20">
    <data encoding="csv">
      0,0,0,0,0,0,0,0,0,0,
      <!-- ... más datos -->
    </data>
  </layer>
</map>
```

### Formato TSX (Tileset XML)

Un archivo `.tsx` define un tileset:
- Imagen fuente del tileset
- Tamaño de tiles
- Spacing y margin
- **Colisiones por tile** (objectgroup)
- Propiedades personalizadas

#### Ejemplo de Archivo TSX

```xml
<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" 
         tiledversion="1.11.2" 
         name="TileMap PixelPlatformer" 
         tilewidth="18" 
         tileheight="18" 
         spacing="1"          <!-- Separación entre tiles -->
         tilecount="180"      <!-- Total de tiles -->
         columns="20">        <!-- Tiles por fila -->
  
  <!-- Imagen del tileset -->
  <image source="kenney_pixel-platformer/Tilemap/tilemap.png" 
         width="379" 
         height="170"/>
  
  <!-- Tile ID 0 con colisión -->
  <tile id="0">
    <objectgroup>
      <object id="1" type="Plataforma" x="0" y="0" width="18" height="18"/>
    </objectgroup>
  </tile>
  
  <!-- Tile ID 1 con colisión -->
  <tile id="1">
    <objectgroup>
      <object id="1" type="Plataforma" x="0" y="0" width="18" height="18"/>
    </objectgroup>
  </tile>
  
  <!-- Más tiles con colisiones... -->
</tileset>
```

### Sistema de IDs (GID - Global ID)

**CONCEPTO CRUCIAL**: Cada tile en un mapa se referencia por su **GID (Global ID)**.

```
┌─────────────────────────────────────────────────────┐
│  CÁLCULO DE GID                                     │
├─────────────────────────────────────────────────────┤
│                                                     │
│  GID = firstgid + local_id                         │
│                                                     │
│  Ejemplo:                                           │
│  ├─ Tileset firstgid = 1                           │
│  ├─ Tile local_id = 9 (bloque de tierra)          │
│  └─ GID en el mapa = 1 + 9 = 10                   │
│                                                     │
│  Si tienes múltiples tilesets:                     │
│  ├─ Tileset A: firstgid = 1   (IDs 1-180)        │
│  ├─ Tileset B: firstgid = 181 (IDs 181-360)      │
│  └─ Tileset C: firstgid = 361 (IDs 361-540)      │
│                                                     │
└─────────────────────────────────────────────────────┘
```

**Ejemplo Visual**:

```
Tileset con 20 columnas:

ID:  0   1   2   3   4   5  ...  19
    ┌───┬───┬───┬───┬───┬───┬───┬───┐
    │   │   │   │   │   │   │...│   │  Fila 0
    ├───┼───┼───┼───┼───┼───┼───┼───┤
    │ 20│ 21│ 22│ 23│ 24│ 25│...│ 39│  Fila 1
    ├───┼───┼───┼───┼───┼───┼───┼───┤
    │ 40│ 41│ 42│ 43│ 44│ 45│...│ 59│  Fila 2
    └───┴───┴───┴───┴───┴───┴───┴───┘

Si firstgid = 1, entonces en el mapa:
- Tile (0,0) del tileset → GID 1
- Tile (1,0) del tileset → GID 2
- Tile (0,1) del tileset → GID 21
```

---

## 4. Preparación de Recursos

### Paso 1: Obtener o Crear Tileset

Tienes dos opciones:

#### Opción A: Usar Tileset Existente (Kenney Pixel Platformer)

Ya tienes un tileset en:
```
assets/levels/tilesets_niveles_prueba/kenney_pixel-platformer/Tilemap/tilemap.png
```

**Especificaciones**:
- Tamaño de imagen: 379×170 píxeles
- Tamaño de tile: 18×18 píxeles
- Spacing: 1 píxel entre tiles
- Total de tiles: 180 (20 columnas × 9 filas)

#### Opción B: Crear Tu Propio Tileset

**Requisitos**:
1. Formato: PNG con transparencia
2. Tiles del mismo tamaño (recomendado: 16×16, 18×18, o 32×32)
3. Organización en cuadrícula uniforme
4. Opcionalmente con spacing/margin consistente

**Herramientas recomendadas**:
- GIMP (gratuito)
- Aseprite (de pago, especializado en pixel art)
- Photoshop
- Piskel (online, gratuito)

**Guía para crear tileset**:

```
1. Crea una imagen nueva:
   - Ancho: (tamaño_tile + spacing) × columnas
   - Alto: (tamaño_tile + spacing) × filas
   - Ejemplo: (18 + 1) × 20 = 380px de ancho

2. Dibuja tiles en cuadrícula:
   ┌──────┬──────┬──────┬──────┐
   │ tile │ tile │ tile │ tile │
   ├──────┼──────┼──────┼──────┤
   │ tile │ tile │ tile │ tile │
   └──────┴──────┴──────┴──────┘

3. Tipos de tiles recomendados:
   - Plataformas sólidas (suelo, techo)
   - Paredes laterales
   - Esquinas
   - Elementos decorativos
   - Peligros (lava, agua, púas)

4. Guarda como PNG con transparencia
```

### Paso 2: Organizar Archivos

Coloca tus recursos así:

```
assets/levels/
├── mi_nivel/                         ← Crea esta carpeta para tu nivel
│   ├── mi_mapa.tmx                   ← Tu archivo de mapa (lo crearás)
│   └── screenshots/                   ← Opcional: capturas del nivel
└── tilesets_mi_nivel/                ← Crea esta carpeta para tus tilesets
    ├── mi_tileset.tsx                ← Tu tileset (lo crearás)
    ├── mi_tileset.png                ← Tu imagen de tileset
    └── decoraciones.tsx              ← Opcional: tileset adicional
```

**IMPORTANTE**: Las rutas relativas en TMX/TSX dependen de esta estructura.

---

## 5. Creación del Tileset en Tiled

### Paso 1: Abrir Tiled

```bash
tiled
# O si instalaste con Flatpak:
flatpak run org.mapeditor.Tiled
```

### Paso 2: Crear Nuevo Tileset

1. **Menú**: `File → New → New Tileset...`

2. **Configuración del Tileset**:

```
┌─────────────────────────────────────────┐
│  New Tileset                            │
├─────────────────────────────────────────┤
│  Name: mi_tileset_plataformas          │
│                                         │
│  Type: ● Based on Tileset Image        │
│        ○ Collection of Images           │
│                                         │
│  [Browse...] → Selecciona tu PNG       │
│                                         │
│  Tile width:  18 px                    │
│  Tile height: 18 px                    │
│                                         │
│  Spacing:     1 px  (entre tiles)      │
│  Margin:      0 px  (borde externo)    │
│                                         │
│  [Save As Tileset]                     │
│    → assets/levels/tilesets_mi_nivel/  │
│      mi_tileset.tsx                     │
│                                         │
│  [OK]                                   │
└─────────────────────────────────────────┘
```

3. **Campos Explicados**:

| Campo | Descripción | Ejemplo |
|-------|-------------|---------|
| **Name** | Nombre identificativo del tileset | `mi_tileset_plataformas` |
| **Type** | `Based on Tileset Image`: Todos los tiles en una imagen<br>`Collection of Images`: Cada tile es un archivo | Usa `Based on...` |
| **Tile width/height** | Tamaño de cada tile en píxeles | 18×18 (tu proyecto) |
| **Spacing** | Separación entre tiles en la imagen | 1 (Kenney) o 0 |
| **Margin** | Borde externo de la imagen | Usualmente 0 |

4. **Guardar el Tileset**:
   - Guarda el archivo `.tsx` en `assets/levels/tilesets_mi_nivel/`
   - **CRUCIAL**: La imagen PNG debe estar en la **misma carpeta** o usar ruta relativa correcta

### Paso 3: Verificar el Tileset

En el panel **Tilesets** de Tiled, deberías ver:

```
┌─────────────────────────────┐
│ Tilesets                    │
├─────────────────────────────┤
│ ▼ mi_tileset_plataformas    │
│   ┌───┬───┬───┬───┬───┐     │
│   │ 0 │ 1 │ 2 │ 3 │ 4 │...  │
│   ├───┼───┼───┼───┼───┤     │
│   │20 │21 │22 │23 │24 │...  │
│   └───┴───┴───┴───┴───┘     │
└─────────────────────────────┘
```

Si ves "❌ Missing" o tiles incorrectos:
- Verifica la ruta de la imagen en el `.tsx`
- Verifica spacing/margin
- Refresca el tileset: `Right-click → Refresh`

---

## 6. Configuración de Colisiones

**CONCEPTO CLAVE**: Las colisiones se definen **a nivel de tileset**, no del mapa. Cuando colocas un tile con colisión en el mapa, la colisión viene automáticamente con él.

### ¿Por Qué Configurar Colisiones?

```cpp
// MapLoader::extractCollisions() busca tiles con objectgroup
for (cada tile en el mapa) {
    if (tile tiene objectgroup) {
        // Este tile tiene colisión
        agregar Rectangle a lista de colisiones
    }
}
```

### Paso 1: Abrir Tile Collision Editor

1. **Menú**: `View → Views and Toolbars → Tile Collision Editor`
   - O presiona: `Ctrl+Shift+O`

2. Verás un panel nuevo:

```
┌─────────────────────────────────────────┐
│ Tile Collision Editor                   │
├─────────────────────────────────────────┤
│  [📐 Rectangle] [⬭ Polygon] [◯ Circle] │
│                                         │
│  ┌─────────────────────────────────┐   │
│  │                                 │   │
│  │     [Vista del tile actual]     │   │
│  │                                 │   │
│  └─────────────────────────────────┘   │
│                                         │
│  Objects:                               │
│  └─ (vacío inicialmente)               │
└─────────────────────────────────────────┘
```

### Paso 2: Seleccionar Tiles con Colisión

En el panel **Tilesets**:
1. Click en un tile que debe tener colisión (ej: un bloque de plataforma)
2. El **Tile Collision Editor** muestra ese tile

### Paso 3: Dibujar Área de Colisión

**Método 1: Colisión Total (Tile Completo)**

1. En **Tile Collision Editor**, selecciona la herramienta **Rectangle** [📐]
2. Click y arrastra para cubrir todo el tile:

```
┌──────────────────┐
│ ████████████████ │  ← Cuadrado de colisión 18×18
│ ████████████████ │     cubre todo el tile
│ ████████████████ │
│ ████████████████ │
└──────────────────┘
```

3. En el panel **Objects** aparecerá:
   ```
   Objects:
   └─ object (rectangle)
      x: 0, y: 0
      width: 18, height: 18
   ```

**Método 2: Colisión Parcial (Plataforma de Salto)**

Para plataformas donde solo la parte superior colisiona:

```
┌──────────────────┐
│                  │  ← Sin colisión (se puede pasar por abajo)
│                  │
│ ████████████████ │  ← Solo la parte superior tiene colisión
│ ████████████████ │     (por ejemplo, altura 6 píxeles)
└──────────────────┘
```

Configuración:
```
Objects:
└─ object (rectangle)
   x: 0, y: 12
   width: 18, height: 6
```

**Método 3: Colisión Compleja (Polígono)**

Para formas irregulares:

```
┌──────────────────┐
│      ████        │  ← Pico triangular
│    ████████      │
│  ████████████    │
│ ██████████████   │
└──────────────────┘
```

1. Selecciona herramienta **Polygon** [⬭]
2. Click en cada vértice del polígono
3. Double-click para cerrar

### Paso 4: Propiedades del Objeto de Colisión

**IMPORTANTE**: Puedes agregar un **type** para distinguir tipos de colisión:

1. Click derecho en el objeto de colisión → **Object Properties**
2. En el campo **Type**, escribe:
   - `Plataforma` → Colisión sólida normal
   - `Bloque` → Colisión sólida en todas direcciones
   - `PeligroFuego` → Mata al jugador de agua
   - `PeligroAgua` → Mata al jugador de fuego
   - `Meta` → Zona de victoria

Ejemplo en TSX:
```xml
<tile id="0">
  <objectgroup>
    <object id="1" type="Plataforma" x="0" y="0" width="18" height="18"/>
  </objectgroup>
</tile>
```

### Paso 5: Repetir para Todos los Tiles Sólidos

Debes configurar colisiones para **cada tile** que necesite ser sólido:

**Tiles típicos con colisión**:
- ✅ Bloques de tierra/piedra
- ✅ Plataformas
- ✅ Paredes
- ✅ Esquinas
- ❌ Decoraciones (plantas, nubes, fondos)
- ❌ Tiles transparentes

**Atajo**: Copia colisión de un tile a otros:
1. Selecciona tile con colisión configurada
2. `Ctrl+C` (copiar colisión)
3. Selecciona otros tiles (mantén `Shift` para múltiples)
4. `Ctrl+V` (pegar colisión)

### Paso 6: Guardar el Tileset

`Ctrl+S` o `File → Save`

**Verificación**: Abre el archivo `.tsx` con un editor de texto:

```xml
<!-- Deberías ver objectgroup en los tiles con colisión -->
<tile id="0">
  <objectgroup>
    <object id="1" type="Plataforma" x="0" y="0" width="18" height="18"/>
  </objectgroup>
</tile>
<tile id="1">
  <objectgroup>
    <object id="1" type="Plataforma" x="0" y="0" width="18" height="18"/>
  </objectgroup>
</tile>
```

---

## 7. Creación del Mapa

### Paso 1: Nuevo Mapa

1. **Menú**: `File → New → New Map...`

2. **Configuración del Mapa**:

```
┌─────────────────────────────────────────┐
│  New Map                                │
├─────────────────────────────────────────┤
│  Orientation: ● Orthogonal              │
│               ○ Isometric               │
│               ○ Hexagonal               │
│                                         │
│  Tile layer format: ● CSV              │
│                     ○ Base64            │
│                                         │
│  Tile render order: Right Down         │
│                                         │
│  Map size:                              │
│    Width:  30 tiles                    │
│    Height: 20 tiles                    │
│                                         │
│  Tile size:                             │
│    Width:  18 px                       │
│    Height: 18 px                       │
│                                         │
│  [OK]                                   │
└─────────────────────────────────────────┘
```

3. **Explicación de Campos**:

| Campo | Valor Recomendado | Descripción |
|-------|-------------------|-------------|
| **Orientation** | `Orthogonal` | Vista top-down estándar (no isométrico) |
| **Tile layer format** | `CSV` | Formato legible, fácil debug |
| **Map size** | `30×20` (ejemplo) | Tamaño total del nivel en tiles |
| **Tile size** | `18×18` | Debe coincidir con tu tileset |

**Cálculo del tamaño físico**:
```
Tamaño del mapa = width_tiles × tile_width = 30 × 18 = 540 píxeles de ancho
                  height_tiles × tile_height = 20 × 18 = 360 píxeles de alto
```

### Paso 2: Importar Tileset al Mapa

1. En el panel **Tilesets**, haz click en el icono **[+]**
2. Selecciona **Add Existing Tileset...**
3. Navega a tu archivo `.tsx` creado anteriormente
4. Click **Open**

```
┌─────────────────────────────┐
│ Tilesets                    │
├─────────────────────────────┤
│ ▼ mi_tileset_plataformas    │  ← Ahora aparece aquí
│   ┌───┬───┬───┬───┬───┐     │
│   │ 0 │ 1 │ 2 │ 3 │ 4 │     │
│   └───┴───┴───┴───┴───┘     │
└─────────────────────────────┘
```

**IMPORTANTE**: El mapa referenciará el tileset con una ruta **relativa**:

```xml
<!-- En mi_mapa.tmx -->
<tileset firstgid="1" source="../tilesets_mi_nivel/mi_tileset.tsx"/>
                               ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
                               Ruta relativa desde el .tmx
```

### Paso 3: Crear Capas (Layers)

Tu proyecto utiliza múltiples capas para organizar el mapa:

1. **Panel Layers** → Click derecho → **New Layer → Tile Layer**

**Capas recomendadas** (de abajo hacia arriba):

```
Layers:
├─ 📄 Background (fondo estático)
├─ 📄 Plataformas (colisiones)
├─ 📄 Decoraciones (elementos visuales sin colisión)
└─ 📄 Foreground (elementos que aparecen delante)
```

2. Nombra cada capa descriptivamente
3. Ordénalas arrastrando (la de arriba se dibuja encima)

### Paso 4: Dibujar el Nivel

#### Herramientas de Dibujo

| Herramienta | Atajo | Uso |
|-------------|-------|-----|
| **Stamp Brush** [🖌️] | `B` | Pintar tiles individuales |
| **Fill** [🪣] | `F` | Rellenar área con un tile |
| **Eraser** [🧽] | `E` | Borrar tiles (establecer a 0) |
| **Rectangle Select** [▭] | `R` | Seleccionar área rectangular |
| **Terrain Brush** [🏔️] | `T` | Auto-tile (esquinas automáticas) |

#### Proceso de Dibujo

**1. Selecciona la capa "Plataformas"** (debe estar activa/azul en el panel Layers)

**2. Selecciona herramienta Stamp Brush** [🖌️]

**3. En el panel Tilesets, click en un tile** (ej: bloque de tierra ID 2)

**4. Click en el mapa para colocar ese tile**

```
Antes:                 Después:
┌─────────┐            ┌─────────┐
│░░░░░░░░░│            │░░░░░░░░░│
│░░░░░░░░░│            │░░░[2]░░░│  ← Tile colocado
│░░░░░░░░░│            │░░░░░░░░░│
└─────────┘            └─────────┘
```

**5. Dibuja una plataforma completa**:

```
Click y arrastra para pintar múltiples tiles:

┌─────────────────────────┐
│                         │
│     [2][3][3][3][4]     │  ← Plataforma
│                         │
└─────────────────────────┘

Donde:
- 2 = Borde izquierdo
- 3 = Tile repetible del medio
- 4 = Borde derecho
```

#### Patrones de Construcción

**Plataforma Simple**:
```
[inicio][medio][medio][medio][fin]
```

**Estructura con Altura**:
```
[techo]       [techo]
[pared]       [pared]
[pared]       [pared]
[esquina_inf] [esquina_inf]
```

**Escaleras/Plataformas Flotantes**:
```
          [plataforma]
    
    [plataforma]

[plataforma]
```

### Paso 5: Agregar Decoraciones

1. **Selecciona capa "Decoraciones"**
2. En Tilesets, selecciona tiles decorativos (plantas, rocas, etc.)
3. Colócalos sobre las plataformas

```
Capa Decoraciones:     Resultado Visual:
┌─────────────┐        ┌─────────────┐
│  [🌿]  [🌸] │        │  🌿    🌸   │  ← Decoraciones
│             │        │             │
└─────────────┘        └─────────────┘

Capa Plataformas:      (se superpone)
┌─────────────┐        ┌─────────────┐
│[2][3][3][4] │        │████████████ │  ← Plataforma sólida
│             │        │             │
└─────────────┘        └─────────────┘
```

**IMPORTANTE**: Los tiles decorativos **NO deben tener colisiones** en el tileset.

### Paso 6: Prueba de Visualización

1. **Menú**: `View → Zoom In/Out` (o `Ctrl+rueda del ratón`)
2. Navega con el scroll o arrastrando con botón central
3. Activa/desactiva capas para verificar cada una:
   - Panel **Layers** → Click en el icono del ojo [👁️] para ocultar/mostrar

### Paso 7: Guardar el Mapa

1. `Ctrl+S` o `File → Save As...`
2. Guarda en `assets/levels/mi_nivel/mi_mapa.tmx`

**Estructura final de archivos**:
```
assets/levels/
├── mi_nivel/
│   └── mi_mapa.tmx                    ← Tu mapa
└── tilesets_mi_nivel/
    ├── mi_tileset.tsx                 ← Tileset referenciado
    └── mi_tileset.png                 ← Imagen del tileset
```

---

## 8. Integración en el Proyecto

### Paso 1: Comprender MapLoader

El archivo `src/MapLoader.cpp` tiene dos métodos principales:

```cpp
// Cargar mapa desde disco (con caché de ResourceManager)
void loadMapa(const char* filePath);

// Extraer colisiones del mapa
std::vector<Rectangle> extractCollisions();
```

### Paso 2: Editar MainGameState.cpp

Localiza el método `MainGameState::init()` en `src/MainGameState.cpp`:

```cpp
void MainGameState::init()
{
    // ... código existente (jugadores, cámara) ...

    // ========================================
    // SECCIÓN DE CARGA DEL MAPA
    // ========================================
    try {
        // 1. Cargar tu mapa
        mapLoader.loadMapa("assets/levels/mi_nivel/mi_mapa.tmx");
        //                  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        //                  ¡CAMBIA ESTA RUTA A TU MAPA!
        
        // 2. Extraer colisiones
        std::vector<Rectangle> collisions = mapLoader.extractCollisions();
        
        // 3. Convertir colisiones en plataformas del juego
        for (const auto& collision : collisions) {
            Platform platform = { collision, NONE };  // Tipo de plataforma
            level_platforms.push_back(platform);
        }
        
        TraceLog(LOG_INFO, "Plataformas cargadas: %d", level_platforms.size());
        
    } catch (const std::runtime_error& e) {
        // Manejo de error
        std::cerr << "Error al cargar mapa: " << e.what() << std::endl;
        this->state_machine->add_state(std::make_unique<MainMenuState>(), true);
    }

    // ... resto del código (enemigos, peligros) ...
}
```

### Paso 3: Tipos de Plataformas

Si quieres que algunos tiles tengan comportamientos especiales, modifica el código:

```cpp
// En lugar de esto:
Platform platform = { collision, NONE };

// Haz esto (basado en el tipo del tile):
ElementType type = NONE;  // Por defecto

// Aquí podrías verificar propiedades del tile para asignar tipo
// Por ejemplo, consultando el type del objectgroup en el TSX

if (/* tile es de fuego */) {
    type = FIRE;   // Mata al jugador de agua
} else if (/* tile es de agua */) {
    type = WATER;  // Mata al jugador de fuego
} else if (/* tile es meta */) {
    type = GOAL;   // Zona de victoria
}

Platform platform = { collision, type };
level_platforms.push_back(platform);
```

**NOTA**: Actualmente, `extractCollisions()` no devuelve el tipo del tile. Para implementar esto, necesitarías modificar `MapLoader::extractCollisions()` para leer el campo `type` del `<object>` en el tileset.

### Paso 4: Renderizar el Mapa

El renderizado ya está implementado en `MainGameState::render()`:

```cpp
void MainGameState::render()
{
    BeginDrawing();
    BeginMode2D(camera);       // Inicia modo cámara 2D
    ClearBackground(RAYWHITE);
    
    // ¡El mapa se dibuja automáticamente aquí!
    mapLoader.draw(camera);    // ← Renderiza el mapa completo
    
    // Dibujar jugadores
    fire_player.render();
    water_player.render();
    
    // ... resto del código de renderizado ...
    
    EndMode2D();
    EndDrawing();
}
```

### Paso 5: Compilar el Proyecto

```bash
# Desde la raíz del proyecto
make clean
make

# Si hay errores, revisa:
# - Rutas de archivos TMX/TSX
# - Nombre del archivo de mapa en loadMapa()
```

### Paso 6: Ejecutar y Probar

```bash
./game
```

**Verificaciones**:
1. ¿El mapa se visualiza correctamente?
2. ¿Los jugadores colisionan con las plataformas?
3. ¿Las decoraciones aparecen sin colisión?

**Debugging**:
- Revisa la consola para logs de `MapLoader`:
  ```
  [ResourceManager] Mapa TMX cargado desde DISCO: assets/levels/mi_nivel/mi_mapa.tmx
  [MapLoader] Plataformas cargadas desde Tiled: 45
  ```

---

## 9. Sistema de Caché y Optimización

### ¿Cómo Funciona el Caché?

El `ResourceManager` implementa un sistema de caché para evitar recargar recursos:

```cpp
// Primera vez que cargas el mapa
mapLoader.loadMapa("assets/levels/nivel1.tmx");
// ↓ ResourceManager carga desde disco → ~100-200ms
// ↓ Guarda en caché

// Segunda vez (después de volver al menú y reiniciar nivel)
mapLoader.loadMapa("assets/levels/nivel1.tmx");
// ↓ ResourceManager devuelve desde caché → <1ms ⚡
```

### Ventajas del Caché

| Sin Caché | Con Caché |
|-----------|-----------|
| Cada carga = lectura de disco | Primera carga = disco, resto = RAM |
| Tiempo: 100-200ms | Tiempo: <1ms |
| I/O intensivo | Sin I/O después de primera carga |

### Uso de Memoria

El caché mantiene el mapa en RAM:

```
Nivel pequeño (30×20 tiles):    ~100 KB en RAM
Nivel mediano (70×40 tiles):    ~200 KB en RAM
Nivel grande (100×100 tiles):   ~1 MB en RAM

Para un juego con 10 niveles:   ~2-5 MB en RAM total

→ Despreciable en sistemas modernos (8+ GB RAM)
```

### Limpiar Caché (Si Es Necesario)

```cpp
// En algún punto del código (ej: al salir del juego)
ResourceManager::getInstance()->clearAllCache();
```

**NOTA**: No necesitas hacer esto manualmente. El destructor de `ResourceManager` limpia automáticamente al cerrar la aplicación.

---

## 10. Resolución de Problemas

### Problema 1: Mapa No Se Carga

**Síntomas**:
```
[ResourceManager] Error al cargar mapa TMX desde disco
ERROR: No se pudo cargar el mapa TMX
```

**Soluciones**:

1. **Verificar ruta del archivo**:
   ```cpp
   // Ruta INCORRECTA (desde src/)
   mapLoader.loadMapa("levels/mi_nivel/mi_mapa.tmx");  // ❌
   
   // Ruta CORRECTA (desde raíz del proyecto)
   mapLoader.loadMapa("assets/levels/mi_nivel/mi_mapa.tmx");  // ✅
   ```

2. **Verificar que el archivo existe**:
   ```bash
   ls -la assets/levels/mi_nivel/mi_mapa.tmx
   # Debe mostrar el archivo
   ```

3. **Permisos del archivo**:
   ```bash
   chmod 644 assets/levels/mi_nivel/mi_mapa.tmx
   ```

### Problema 2: Tileset No Aparece en el Mapa

**Síntomas**:
- El mapa carga pero está en blanco
- En Tiled aparece pero no en el juego

**Soluciones**:

1. **Verificar ruta relativa del tileset en el TMX**:
   
   Abre `mi_mapa.tmx` con un editor de texto:
   ```xml
   <tileset firstgid="1" source="../tilesets_mi_nivel/mi_tileset.tsx"/>
                                  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   ```
   
   Debe ser una ruta **relativa** desde donde está el `.tmx`:
   ```
   assets/levels/mi_nivel/mi_mapa.tmx
                  ↓ sube un nivel (..)
   assets/levels/tilesets_mi_nivel/mi_tileset.tsx
   ```

2. **Verificar ruta de imagen en el TSX**:
   
   Abre `mi_tileset.tsx`:
   ```xml
   <image source="mi_tileset.png" width="380" height="180"/>
                  ^^^^^^^^^^^^^^
   ```
   
   La imagen debe estar en la **misma carpeta** que el `.tsx` o usar ruta relativa.

3. **Regenerar caché de Tiled**:
   ```bash
   # En Tiled, menú:
   # Edit → Preferences → Clear Cache
   ```

### Problema 3: Colisiones No Funcionan

**Síntomas**:
- Los jugadores caen a través de las plataformas
- Log muestra: `Plataformas cargadas: 0`

**Soluciones**:

1. **Verificar que los tiles tienen objectgroup**:
   
   Abre el `.tsx` con editor de texto:
   ```xml
   <tile id="2">
     <objectgroup>  <!-- ← Esto debe existir -->
       <object id="1" type="Plataforma" x="0" y="0" width="18" height="18"/>
     </objectgroup>
   </tile>
   ```

2. **Verificar Tile Collision Editor en Tiled**:
   - Abre el tileset en Tiled
   - `View → Tile Collision Editor`
   - Selecciona un tile de plataforma
   - Debería aparecer un rectángulo rojo sobre el tile

3. **Verificar que usas los tiles correctos en el mapa**:
   - Los tiles con colisión deben ser los que tienen objectgroup
   - Si colocas un tile decorativo, no tendrá colisión

4. **Debug en código**:
   ```cpp
   // En MainGameState::init(), después de extractCollisions()
   auto collisions = mapLoader.extractCollisions();
   TraceLog(LOG_INFO, "Colisiones extraídas: %d", collisions.size());
   for (const auto& col : collisions) {
       TraceLog(LOG_DEBUG, "Colisión: x=%.2f y=%.2f w=%.2f h=%.2f", 
                col.x, col.y, col.width, col.height);
   }
   ```

### Problema 4: Mapa Se Ve Distorsionado

**Síntomas**:
- Tiles desalineados
- Spacing incorrecto

**Soluciones**:

1. **Verificar consistencia de tamaños**:
   
   | Elemento | Debe Coincidir |
   |----------|----------------|
   | **Tileset**: Tile width/height | 18×18 |
   | **Mapa**: Tile width/height | 18×18 |
   | **Spacing** en tileset | 1 (o 0 según tu imagen) |

2. **Recalcular spacing**:
   
   ```
   Fórmula para spacing:
   spacing = (ancho_imagen - margin*2 - tilewidth*columnas) / (columnas - 1)
   
   Ejemplo:
   Imagen: 379px ancho
   Columnas: 20
   Tilewidth: 18px
   Margin: 0
   
   spacing = (379 - 0 - 18*20) / (20-1)
           = (379 - 360) / 19
           = 1
   ```

3. **Rehacer tileset**:
   - Borra el tileset del mapa
   - Recrea el `.tsx` con valores correctos
   - Vuelve a importar al mapa

### Problema 5: Rendimiento Lento

**Síntomas**:
- FPS bajos al dibujar el mapa
- Lag al cargar niveles

**Soluciones**:

1. **Optimizar número de colisiones**:
   
   `extractCollisions()` crea un `Rectangle` por cada tile con colisión. Para un mapa grande, esto puede ser ineficiente.
   
   **Alternativa**: Combinar rectángulos adyacentes (requiere modificar `MapLoader`).

2. **Limitar tamaño del mapa**:
   
   | Tamaño del Mapa | Rendimiento |
   |-----------------|-------------|
   | 30×20 (600 tiles) | ⚡ Excelente |
   | 70×40 (2800 tiles) | ✅ Bueno |
   | 100×100 (10000 tiles) | ⚠️ Aceptable |
   | 200×200 (40000 tiles) | ❌ Lento |

3. **Usar culling**:
   
   Solo renderizar la porción del mapa visible en cámara (requiere modificación de `MapLoader::draw()`).

### Problema 6: Errores de Compilación

**Error**: `undefined reference to LoadTMX`

**Solución**: Verifica que `raytmx_impl.cpp` esté en `src/` y se compile:

```bash
# Verifica makefile
grep raytmx makefile
# Debe incluir: src/raytmx_impl.o
```

**Error**: `No such file or directory: raytmx.h`

**Solución**: Verifica que `raytmx.h` esté en `vendor/include/`:

```bash
ls -la vendor/include/raytmx.h
```

---

## 11. Mejores Prácticas

### Organización de Archivos

```
✅ BUENA PRÁCTICA:
assets/levels/
├── nivel1/
│   ├── nivel1.tmx
│   └── nivel1_variante.tmx
├── nivel2/
│   └── nivel2.tmx
└── tilesets_comunes/
    ├── plataformas.tsx
    ├── plataformas.png
    ├── decoraciones.tsx
    └── decoraciones.png

❌ MALA PRÁCTICA:
assets/
├── nivel1.tmx          ← Desorganizado
├── nivel2.tmx
├── tileset1.tsx
├── tileset2.tsx
└── imagen.png
```

### Nomenclatura

```
✅ BUENOS NOMBRES:
- nivel1_tutorial.tmx
- tileset_plataformas_rocosas.tsx
- mapa_boss_final.tmx

❌ MALOS NOMBRES:
- map1.tmx
- tileset.tsx
- test.tmx
```

### Propiedades Personalizadas

Puedes agregar propiedades custom a tiles para comportamientos especiales:

```xml
<!-- En el tileset -->
<tile id="15">
  <properties>
    <property name="tipo" value="peligro_fuego"/>
    <property name="daño" type="int" value="10"/>
  </properties>
  <objectgroup>
    <object id="1" x="0" y="0" width="18" height="18"/>
  </objectgroup>
</tile>
```

Luego en código:
```cpp
// Lee la propiedad "tipo" del tile
// (Requiere extender MapLoader para leer propiedades)
```

### Versionado

Usa control de versiones para tus mapas:

```bash
git add assets/levels/mi_nivel/mi_mapa.tmx
git add assets/levels/tilesets_mi_nivel/mi_tileset.tsx
git commit -m "Añadir nivel 3 con plataformas móviles"
```

### Documentación

Crea un archivo README en cada carpeta de nivel:

```markdown
# Nivel 1 - Tutorial

## Descripción
Nivel introductorio que enseña mecánicas básicas.

## Características
- Tamaño: 30×20 tiles (540×360 px)
- Enemigos: 2 fuego, 1 agua
- Dificultad: Fácil

## Tilesets
- `tilesets_comunes/plataformas.tsx`: Plataformas principales
- `tilesets_comunes/decoraciones.tsx`: Elementos visuales

## Notas
- La meta está en la esquina superior derecha
- Hay un salto difícil en x=15, considera agregar plataforma intermedia
```

### Testing

Crea niveles de prueba para verificar mecánicas:

```
assets/levels/test/
├── test_colisiones.tmx       ← Prueba física de colisiones
├── test_enemigos.tmx          ← Prueba spawn de enemigos
└── test_renderizado.tmx       ← Prueba capas y visuales
```

---

## 12. Ejemplos Prácticos

### Ejemplo 1: Nivel Simple (Tutorial)

**Objetivo**: Nivel de 30×20 tiles con plataformas básicas

**Paso a paso**:

1. **Crear estructura de carpetas**:
```bash
mkdir -p assets/levels/tutorial
mkdir -p assets/levels/tilesets_tutorial
```

2. **Crear tileset en Tiled**:
   - Nuevo Tileset: "plataformas_tutorial"
   - Imagen: Kenney tilemap (ya tienes)
   - Tamaño tile: 18×18, Spacing: 1
   - Guardar en: `assets/levels/tilesets_tutorial/plataformas_tutorial.tsx`

3. **Configurar colisiones**:
   - Abrir Tile Collision Editor
   - Tiles 0-3: Plataformas superiores → Colisión completa (18×18)
   - Tiles 20-23: Plataformas medias → Colisión completa
   - Tiles 40-43: Plataformas inferiores → Colisión completa

4. **Crear mapa**:
   - Nuevo mapa: 30×20 tiles, 18×18 px
   - Importar tileset `plataformas_tutorial.tsx`
   - Crear capa "Plataformas"
   - Dibujar suelo en la fila 18 (casi al fondo)
   - Agregar plataformas flotantes en filas 10 y 14
   - Guardar en: `assets/levels/tutorial/tutorial_nivel1.tmx`

5. **Integrar en código**:
```cpp
// En MainGameState::init()
mapLoader.loadMapa("assets/levels/tutorial/tutorial_nivel1.tmx");
```

6. **Compilar y probar**:
```bash
make clean && make
./game
```

### Ejemplo 2: Nivel con Múltiples Tilesets

**Objetivo**: Nivel que combina plataformas + decoraciones

1. **Crear dos tilesets**:
   - `tilesets_nivel2/plataformas.tsx` (con colisiones)
   - `tilesets_nivel2/decoraciones.tsx` (sin colisiones)

2. **En el mapa**:
   - Importar ambos tilesets
   - Capa 1: "Plataformas" → Usa tileset de plataformas
   - Capa 2: "Decoraciones" → Usa tileset de decoraciones

3. **En el TMX resultante**:
```xml
<tileset firstgid="1" source="../tilesets_nivel2/plataformas.tsx"/>
<tileset firstgid="181" source="../tilesets_nivel2/decoraciones.tsx"/>
<!--     ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
         Nota: firstgid = 181 porque plataformas tiene 180 tiles -->
```

### Ejemplo 3: Nivel Grande con Scroll

**Objetivo**: Nivel de 70×40 tiles que requiere movimiento de cámara

1. **Crear mapa grande**:
   - Nuevo mapa: 70×40 tiles
   - Tamaño físico: 1260×720 píxeles

2. **Diseñar con secciones**:
   ```
   [Inicio] → [Sección 1] → [Sección 2] → [Sección 3] → [Meta]
   0-15      15-30         30-45         45-60         60-70
   ```

3. **La cámara en MainGameState ya sigue a los jugadores**:
```cpp
// En MainGameState::update()
Vector2 midPoint = {
    (fire_player.position.x + water_player.position.x) / 2.0f,
    (fire_player.position.y + water_player.position.y) / 2.0f
};
camera.target.x += (midPoint.x - camera.target.x) * cameraSpeed;
```

4. **Probar scroll**:
   - Los jugadores se mueven → La cámara los sigue
   - El mapa grande se revela gradualmente

### Ejemplo 4: Nivel con Peligros (Fuego/Agua)

**Objetivo**: Crear tiles que matan jugadores según su tipo

**Método Actual (Sin Propiedades TMX)**:

```cpp
// En MainGameState::init(), después de cargar colisiones
auto collisions = mapLoader.extractCollisions();

for (const auto& collision : collisions) {
    // Por ahora, todas son plataformas normales
    Platform platform = { collision, NONE };
    level_platforms.push_back(platform);
}

// Agregar peligros MANUALMENTE
Platform lava = Platform({ 200, 350, 100, 50 }, FIRE);
level_platforms.push_back(lava);

Platform water = Platform({ 550, 350, 100, 50 }, WATER);
level_platforms.push_back(water);
```

**Método Futuro (Con Propiedades TMX - Requiere Extender Código)**:

1. **En Tiled, agregar propiedad al tile**:
   - Selecciona tile de lava
   - Panel Properties → Add Property
   - Name: `elemento`, Type: `string`, Value: `fuego`

2. **Modificar `MapLoader::extractCollisions()`** para leer propiedades:
```cpp
// Pseudocódigo - implementación real es más compleja
if (tile->properties["elemento"] == "fuego") {
    type = FIRE;
} else if (tile->properties["elemento"] == "agua") {
    type = WATER;
}
```

3. **Usar el tipo extraído**:
```cpp
for (const auto& collision : collisions) {
    // Ahora collision trae el tipo
    Platform platform = { collision.rect, collision.type };
    level_platforms.push_back(platform);
}
```

---

## 📖 Recursos Adicionales

### Documentación Oficial
- **Tiled Manual**: https://doc.mapeditor.org/en/stable/
- **Raylib TMX**: Incluido en `vendor/include/raytmx.h`
- **Kenney Assets**: https://kenney.nl/assets/pixel-platformer

### Tutoriales en Video
- **Tiled Basics**: https://www.youtube.com/watch?v=Z_7COcLeYFY
- **Raylib + Tiled Integration**: Busca "raylib tiled map tutorial"

### Herramientas Recomendadas
- **Tiled Map Editor**: https://www.mapeditor.org/
- **Aseprite** (pixel art): https://www.aseprite.org/
- **GIMP** (editor de imágenes gratuito): https://www.gimp.org/

### Comunidades
- **Raylib Discord**: Para preguntas sobre integración
- **Tiled Forum**: https://discourse.mapeditor.org/

---

## 🎯 Checklist Final

Antes de considerar tu mapa completo, verifica:

### Diseño
- [ ] El mapa tiene dimensiones apropiadas (no demasiado grande)
- [ ] Las plataformas son navegables (no saltos imposibles)
- [ ] Hay caminos alternativos o desafíos opcionales
- [ ] La meta es claramente alcanzable

### Técnico
- [ ] Tileset guardado como `.tsx` en `tilesets_*/`
- [ ] Mapa guardado como `.tmx` en `levels/*/`
- [ ] Rutas relativas correctas entre TMX y TSX
- [ ] Tiles con colisión tienen `<objectgroup>` en TSX
- [ ] Tamaño de tile consistente (18×18) en todo

### Integración
- [ ] `mapLoader.loadMapa()` apunta a tu TMX
- [ ] Compilación sin errores
- [ ] El mapa se renderiza correctamente en juego
- [ ] Las colisiones funcionan (jugadores no atraviesan)
- [ ] Decoraciones aparecen sin bloquear movimiento

### Optimización
- [ ] No hay tiles con colisión innecesarios
- [ ] Capas organizadas lógicamente
- [ ] Archivo TMX razonablemente pequeño (< 500 KB)

---

## 🚀 Conclusión

¡Ahora tienes todo el conocimiento necesario para crear tus propios niveles!

**Resumen del flujo completo**:

```
1. Preparar imagen de tileset (PNG)
              ↓
2. Crear tileset en Tiled (.tsx)
              ↓
3. Configurar colisiones (Tile Collision Editor)
              ↓
4. Guardar tileset
              ↓
5. Crear nuevo mapa en Tiled (.tmx)
              ↓
6. Importar tileset al mapa
              ↓
7. Crear capas (Plataformas, Decoraciones, etc.)
              ↓
8. Dibujar el nivel
              ↓
9. Guardar mapa
              ↓
10. Actualizar MainGameState::init()
              ↓
11. Compilar proyecto (make)
              ↓
12. ¡JUGAR Y PROBAR!
```

**Próximos pasos sugeridos**:
1. Crea un nivel de prueba pequeño (15×10) para familiarizarte
2. Experimenta con diferentes diseños de plataformas
3. Agrega decoraciones para hacer el nivel visualmente interesante
4. Prueba diferentes configuraciones de colisión (plataformas que solo colisionan arriba, etc.)
5. Crea una serie de niveles con dificultad progresiva

---

**Documento creado**: 24 de noviembre de 2025  
**Versión**: 1.0  
**Autor**: Guía exhaustiva basada en análisis del proyecto p1-fire-and-water-dca

¡Mucha suerte con la creación de tus niveles! 🎮✨
