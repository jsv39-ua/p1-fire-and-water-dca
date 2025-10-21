# Música de Fondo - Documentación

## Descripción
Se ha añadido música de fondo al juego "Fuego y Agua" que se reproduce continuamente en loop durante toda la sesión de juego.

## Características
- **Música de fondo continua**: La música se reproduce automáticamente al iniciar el juego
- **Toggle en menú de opciones**: Los jugadores pueden activar/desactivar la música desde el menú de opciones
- **Estado persistente**: El estado de la música (activada/desactivada) se mantiene durante toda la sesión
- **Loop automático**: La música se repite continuamente sin interrupciones

## Cómo usar
1. Al iniciar el juego, la música comienza automáticamente
2. Desde el menú principal, haz clic en "OPCIONES"
3. En el menú de opciones, haz clic en el botón "MUSICA: ACTIVADA" o "MUSICA: DESACTIVADA" para alternar
4. El botón cambiará de color (verde cuando está activada, rojo cuando está desactivada)
5. Haz clic en "VOLVER" o presiona ESC para regresar al menú principal

## Implementación técnica

### Archivos modificados
- `src/main.cpp`: Inicialización del sistema de audio, carga y gestión de la música
- `src/MainMenuState.cpp`: Enlace al nuevo menú de opciones
- `vendor/include/MainMenuState.hpp`: Sin cambios (la declaración ya existía)

### Archivos nuevos
- `src/OptionsMenuState.cpp`: Implementación del menú de opciones
- `vendor/include/OptionsMenuState.hpp`: Declaración de la clase OptionsMenuState
- `assets/music/background_music.wav`: Archivo de música de fondo (placeholder generado)
- `assets/music/README.md`: Documentación del directorio de música

### Variables globales
- `g_musicEnabled`: Booleana que controla si la música está activada
- `g_backgroundMusic`: Objeto Music de Raylib que contiene el stream de música

### Flujo de ejecución
1. `main.cpp` inicializa el sistema de audio con `InitAudioDevice()`
2. Se carga el archivo de música con `LoadMusicStream()`
3. Se configura el loop con `g_backgroundMusic.looping = true`
4. En cada frame del bucle principal:
   - `UpdateMusicStream()` actualiza el buffer de audio
   - Se verifica el estado de `g_musicEnabled` para reproducir o detener la música
5. Al finalizar, se liberan recursos con `UnloadMusicStream()` y `CloseAudioDevice()`

## Reemplazar la música
Para usar música personalizada:
1. Reemplaza el archivo `assets/music/background_music.wav` con tu propio archivo de música
2. Formatos soportados: WAV, OGG, MP3, FLAC (según Raylib)
3. Se recomienda un archivo de ~30-60 segundos que funcione bien en loop

## Notas
- La música actual es un placeholder generado con tonos simples
- Se recomienda reemplazarla con música original o con licencia apropiada
- El sistema de audio de Raylib es robusto y soporta múltiples formatos
