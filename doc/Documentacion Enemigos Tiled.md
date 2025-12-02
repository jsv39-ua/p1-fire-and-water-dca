# Guía: Añadir Enemigos desde Tiled

Esta guía explica el flujo de trabajo para añadir enemigos a los niveles utilizando el editor de mapas Tiled. El sistema permite definir el tipo de enemigo y su comportamiento directamente desde las propiedades de la capa.

## Requisitos

- Tiled Map Editor instalado.
- Tileset de enemigos cargado en el mapa (ej. `tileset-characters.tsx`).

## Pasos para añadir enemigos

### 1. Crear una Capa de Tiles
Crea una nueva **Capa de Tiles** (Tile Layer) en tu mapa. Esta capa contendrá exclusivamente los tiles que representan a los enemigos.
> **Nota:** Puedes tener múltiples capas de enemigos si necesitas diferentes configuraciones (ej. una capa para enemigos de fuego y otra para agua).

### 2. Configurar Propiedades de la Capa
Para que el motor del juego (`MapLoader`) reconozca esta capa como una capa de generación de enemigos, debes añadir las siguientes **Propiedades Personalizadas** (Custom Properties) a la capa y asignarle la clase `Enemy`:

| Nombre Propiedad | Tipo   | Valor Requerido   | Descripción |
|------------------|--------|-------------------|-------------|
| `elementType`    | String | `FIRE` o `WATER`  | **Obligatorio**. Define el elemento del enemigo. La presencia de esta propiedad indica al loader que es una capa de enemigos. |
| `patrolRange`    | Float  | Ej: `50.0`, `150.0` | **Opcional**. Define la distancia en píxeles que el enemigo patrullará desde su punto de origen. Valor por defecto: `50.0`. |

### 3. Colocar los Enemigos
Selecciona el tile del enemigo deseado en tu tileset y píntalo en el mapa en la posición inicial deseada.
*   Cada tile pintado en esta capa se convertirá en una instancia de la clase `Enemy`.
*   El juego extraerá automáticamente la textura del tile seleccionado para renderizar al enemigo.

## Funcionamiento Técnico

El proceso es manejado por la clase `MapLoader` en el método `extractEnemies()`:

1.  **Escaneo**: El loader itera sobre todas las capas del mapa buscando aquellas que tengan la propiedad `elementType`.
2.  **Generación**: Por cada tile encontrado en estas capas, se instancia un objeto `Enemy` con:
    *   Posición del mundo (basada en coordenadas del tile).
    *   Tipo de elemento (`FIRE`/`WATER`).
    *   Rango de patrulla.
    *   Textura y recorte (Source Rect) obtenidos del GID del tile.
3.  **Ocultación**: Una vez extraídos los enemigos, la capa original se marca como invisible (`layer->visible = false`) para evitar que se renderice el tile estático duplicado detrás de la entidad dinámica.

## Ejemplo Práctico

Para crear una patrulla de enemigos de fuego:
1.  Crear capa llamada `Enemigos_Fuego_Nivel1`.
2.  Añadir propiedad `elementType` (String) con valor `FIRE`.
3.  Añadir propiedad `patrolRange` (Float) con valor `100.0`.
4.  Pintar los tiles del personaje de fuego en las plataformas deseadas.
