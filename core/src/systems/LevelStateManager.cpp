#include <systems/LevelStateManager.hpp>
#include <algorithm>

extern "C" {
    #include <raylib.h>
}

LevelStateManager& LevelStateManager::getInstance() {
    static LevelStateManager instance;
    return instance;
}

void LevelStateManager::saveZoneState(const std::string& zonePath, const std::vector<PlatformState>& states) {
    ZoneState zoneState;
    zoneState.platformStates = states;
    zoneStates[zonePath] = zoneState;
    TraceLog(LOG_INFO, "[LevelStateManager] Saved state for zone: %s (%d platforms)", 
             zonePath.c_str(), (int)states.size());
}

bool LevelStateManager::hasZoneState(const std::string& zonePath) const {
    return zoneStates.find(zonePath) != zoneStates.end();
}

const ZoneState* LevelStateManager::getZoneState(const std::string& zonePath) const {
    auto it = zoneStates.find(zonePath);
    if (it != zoneStates.end()) {
        return &it->second;
    }
    return nullptr;
}

void LevelStateManager::clearLevelState(const std::string& levelPath) {
    // Eliminar todas las zonas que pertenezcan a este nivel
    auto it = zoneStates.begin();
    while (it != zoneStates.end()) {
        // Si el path de la zona comienza con el path del nivel, la eliminamos
        if (it->first.find(levelPath) == 0) {
            TraceLog(LOG_INFO, "[LevelStateManager] Clearing state for zone: %s", it->first.c_str());
            it = zoneStates.erase(it);
        } else {
            ++it;
        }
    }
    TraceLog(LOG_INFO, "[LevelStateManager] Cleared all state for level: %s", levelPath.c_str());
}

void LevelStateManager::clearAllState() {
    zoneStates.clear();
    activatedTags.clear();
    TraceLog(LOG_INFO, "[LevelStateManager] Cleared all zone states and activated tags");
}

std::string LevelStateManager::getLevelPathFromZone(const std::string& zonePath) {
    // El path del nivel es la carpeta que contiene la zona
    // Ejemplo: "assets/levels/niveles/nivel-tutorial/zona-1.tmx" -> "assets/levels/niveles/nivel-tutorial/"
    size_t lastSlash = zonePath.find_last_of('/');
    if (lastSlash != std::string::npos) {
        return zonePath.substr(0, lastSlash + 1);
    }
    return zonePath;
}

void LevelStateManager::activateTag(const std::string& tag) {
    activatedTags.insert(tag);
    TraceLog(LOG_INFO, "[LevelStateManager] Tag activated globally: %s", tag.c_str());
}

bool LevelStateManager::isTagActivated(const std::string& tag) const {
    return activatedTags.find(tag) != activatedTags.end();
}

void LevelStateManager::clearActivatedTags() {
    activatedTags.clear();
    TraceLog(LOG_INFO, "[LevelStateManager] Cleared all activated tags");
}
