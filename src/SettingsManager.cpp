#include "SettingsManager.hpp"
#include <fstream>
#include <sstream>

SettingsManager* SettingsManager::instance = nullptr;

SettingsManager::SettingsManager() {
    // Valores por defecto
    masterVolume = 1.0f;
    musicVolume = 0.8f;
    sfxVolume = 0.7f;
    language = "spanish";
    
    // Teclas por defecto
    fireLeft = KEY_A;
    fireRight = KEY_D;
    fireJump = KEY_W;
    fireAttack = KEY_S;
    
    waterLeft = KEY_LEFT;
    waterRight = KEY_RIGHT;
    waterJump = KEY_UP;
    waterAttack = KEY_DOWN;
}

SettingsManager* SettingsManager::getInstance() {
    if (instance == nullptr) {
        instance = new SettingsManager();
    }
    return instance;
}

void SettingsManager::loadSettings() {
    char* jsonText = LoadFileText(settingsPath);
    
    if (jsonText == nullptr) {
        TraceLog(LOG_WARNING, "Settings file not found, using defaults");
        return;
    }
    
    // Parsear JSON manualmente (simple)
    std::string content(jsonText);
    UnloadFileText(jsonText);
    
    // Buscar valores en el JSON (parseo simple)
    auto findValue = [&](const std::string& key) -> std::string {
        size_t pos = content.find("\"" + key + "\"");
        if (pos == std::string::npos) return "";
        
        pos = content.find(":", pos);
        if (pos == std::string::npos) return "";
        
        pos = content.find_first_not_of(" \t\n:", pos + 1);
        if (pos == std::string::npos) return "";
        
        size_t end = content.find_first_of(",}\n", pos);
        std::string value = content.substr(pos, end - pos);
        
        // Eliminar comillas si existen
        if (value.front() == '"') value = value.substr(1);
        if (value.back() == '"') value.pop_back();
        
        return value;
    };
    
    // Cargar valores
    std::string val;
    
    val = findValue("masterVolume");
    if (!val.empty()) masterVolume = std::stof(val);
    
    val = findValue("musicVolume");
    if (!val.empty()) musicVolume = std::stof(val);
    
    val = findValue("sfxVolume");
    if (!val.empty()) sfxVolume = std::stof(val);
    
    val = findValue("language");
    if (!val.empty()) language = val;
    
    // Cargar teclas de Fire Player
    size_t firePlayerPos = content.find("\"firePlayer\"");
    if (firePlayerPos != std::string::npos) {
        size_t leftPos = content.find("\"left\"", firePlayerPos);
        if (leftPos != std::string::npos) {
            size_t colonPos = content.find(":", leftPos);
            size_t commaPos = content.find(",", colonPos);
            fireLeft = std::stoi(content.substr(colonPos + 1, commaPos - colonPos - 1));
        }
        
        size_t rightPos = content.find("\"right\"", firePlayerPos);
        if (rightPos != std::string::npos) {
            size_t colonPos = content.find(":", rightPos);
            size_t commaPos = content.find(",", colonPos);
            fireRight = std::stoi(content.substr(colonPos + 1, commaPos - colonPos - 1));
        }
        
        size_t jumpPos = content.find("\"jump\"", firePlayerPos);
        if (jumpPos != std::string::npos) {
            size_t colonPos = content.find(":", jumpPos);
            size_t commaPos = content.find_first_of(",\n", colonPos);
            fireJump = std::stoi(content.substr(colonPos + 1, commaPos - colonPos - 1));
        }
        
        size_t attackPos = content.find("\"attack\"", firePlayerPos);
        if (attackPos != std::string::npos) {
            size_t colonPos = content.find(":", attackPos);
            size_t commaPos = content.find_first_of(",\n}", colonPos);
            fireAttack = std::stoi(content.substr(colonPos + 1, commaPos - colonPos - 1));
        }
    }
    
    // Cargar teclas de Water Player
    size_t waterPlayerPos = content.find("\"waterPlayer\"");
    if (waterPlayerPos != std::string::npos) {
        size_t leftPos = content.find("\"left\"", waterPlayerPos);
        if (leftPos != std::string::npos) {
            size_t colonPos = content.find(":", leftPos);
            size_t commaPos = content.find(",", colonPos);
            waterLeft = std::stoi(content.substr(colonPos + 1, commaPos - colonPos - 1));
        }
        
        size_t rightPos = content.find("\"right\"", waterPlayerPos);
        if (rightPos != std::string::npos) {
            size_t colonPos = content.find(":", rightPos);
            size_t commaPos = content.find(",", colonPos);
            waterRight = std::stoi(content.substr(colonPos + 1, commaPos - colonPos - 1));
        }
        
        size_t jumpPos = content.find("\"jump\"", waterPlayerPos);
        if (jumpPos != std::string::npos) {
            size_t colonPos = content.find(":", jumpPos);
            size_t commaPos = content.find_first_of(",\n", colonPos);
            waterJump = std::stoi(content.substr(colonPos + 1, commaPos - colonPos - 1));
        }
        
        size_t attackPos = content.find("\"attack\"", waterPlayerPos);
        if (attackPos != std::string::npos) {
            size_t colonPos = content.find(":", attackPos);
            size_t commaPos = content.find_first_of(",\n}", colonPos);
            waterAttack = std::stoi(content.substr(colonPos + 1, commaPos - colonPos - 1));
        }
    }
    
    TraceLog(LOG_INFO, "Settings loaded successfully");
}

void SettingsManager::saveSettings() {
    std::stringstream ss;
    
    ss << "{\n";
    ss << "    \"audio\": {\n";
    ss << "        \"masterVolume\": " << masterVolume << ",\n";
    ss << "        \"musicVolume\": " << musicVolume << ",\n";
    ss << "        \"sfxVolume\": " << sfxVolume << "\n";
    ss << "    },\n";
    ss << "    \"language\": \"" << language << "\",\n";
    ss << "    \"controls\": {\n";
    ss << "        \"firePlayer\": {\n";
    ss << "            \"left\": " << fireLeft << ",\n";
    ss << "            \"right\": " << fireRight << ",\n";
    ss << "            \"jump\": " << fireJump << ",\n";
    ss << "            \"attack\": " << fireAttack << "\n";
    ss << "        },\n";
    ss << "        \"waterPlayer\": {\n";
    ss << "            \"left\": " << waterLeft << ",\n";
    ss << "            \"right\": " << waterRight << ",\n";
    ss << "            \"jump\": " << waterJump << ",\n";
    ss << "            \"attack\": " << waterAttack << "\n";
    ss << "        }\n";
    ss << "    }\n";
    ss << "}\n";
    
    SaveFileText(settingsPath, (char*)ss.str().c_str());
    TraceLog(LOG_INFO, "Settings saved successfully");
}