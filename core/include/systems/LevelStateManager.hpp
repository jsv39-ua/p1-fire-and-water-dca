#pragma once

#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief Estructura que guarda el estado persistente de una plataforma/botón
 */
struct PlatformState {
    int id;
    bool active;
    bool isPressed;
};

/**
 * @brief Estructura que guarda el estado completo de una zona
 */
struct ZoneState {
    std::vector<PlatformState> platformStates;
};

/**
 * @brief Sistema singleton para gestionar la persistencia del estado de las zonas
 * 
 * Este sistema permite guardar y restaurar el estado de plataformas y botones
 * cuando el jugador cambia entre zonas del mismo nivel, evitando que se reinicien
 * al volver a visitar una zona.
 */
class LevelStateManager {
public:
    static LevelStateManager& getInstance();

    // Guardar el estado de una zona específica
    void saveZoneState(const std::string& zonePath, const std::vector<PlatformState>& states);
    
    // Comprobar si existe estado guardado para una zona
    bool hasZoneState(const std::string& zonePath) const;
    
    // Obtener el estado guardado de una zona
    const ZoneState* getZoneState(const std::string& zonePath) const;
    
    // Limpiar el estado de un nivel completo (cuando se cambia de nivel o se reinicia)
    void clearLevelState(const std::string& levelPath);
    
    // Limpiar todo el estado (al volver al menú principal o reiniciar el juego)
    void clearAllState();
    
    // Obtener el path del nivel actual desde el path de una zona
    static std::string getLevelPathFromZone(const std::string& zonePath);

private:
    LevelStateManager() = default;
    ~LevelStateManager() = default;
    LevelStateManager(const LevelStateManager&) = delete;
    LevelStateManager& operator=(const LevelStateManager&) = delete;

    // Mapa de path de zona -> estado de la zona
    std::unordered_map<std::string, ZoneState> zoneStates;
};
