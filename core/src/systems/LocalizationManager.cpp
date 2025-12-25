#include "systems/LocalizationManager.hpp"
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <clocale>

// Compatibilidad con Windows para variables de entorno
#if defined(_WIN32)
    #include <windows.h>
    #define setenv(k, v, o) _putenv_s(k, v)
#endif

LocalizationManager* LocalizationManager::getInstance() {
    static LocalizationManager instance;
    return &instance;
}

LocalizationManager::LocalizationManager() : currentLang("en") {}

void LocalizationManager::init() {
    setlocale(LC_ALL, ""); 

    // Configurar rutas y codificación de gettext
    bindtextdomain("fire_water_game", "assets/locales");
    bind_textdomain_codeset("fire_water_game", "UTF-8");
    textdomain("fire_water_game");
    
    setLanguage("es"); 
}

void LocalizationManager::setLanguage(const std::string& langCode) {
    currentLang = langCode;
    
    std::string targetLocale = langCode;
    if (langCode == "en") targetLocale = "en_US.UTF-8";
    else if (langCode == "es") targetLocale = "es_ES.UTF-8";

    #if defined(_WIN32)
        _putenv_s("LANGUAGE", langCode.c_str());
    #else
        setenv("LANGUAGE", langCode.c_str(), 1);
        setenv("LC_ALL", targetLocale.c_str(), 1);
    #endif
    
    // Intentar aplicar locale, con fallback a .utf8 si falla
    const char* primaryLocaleResult = setlocale(LC_ALL, targetLocale.c_str());
    if (!primaryLocaleResult) {
        std::string fallback = targetLocale;
        std::size_t pos = fallback.find(".UTF-8");
        if (pos != std::string::npos) {
            fallback.replace(pos, 6, ".utf8");
            const char* fallbackLocaleResult = setlocale(LC_ALL, fallback.c_str());
            if (!fallbackLocaleResult) {
                std::cerr << "Warning: Failed to set locale to '" << targetLocale
                          << "' and fallback locale '" << fallback << "'." << std::endl;
            }
        } else {
            std::cerr << "Warning: Failed to set locale to '" << targetLocale
                      << "'. No suitable fallback locale was applied." << std::endl;
        }
    }
    
    // Recargar dominio para aplicar cambios
    textdomain("fire_water_game");
}
