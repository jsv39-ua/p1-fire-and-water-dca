#pragma once

#include <string>
#include <vector>
#include <libintl.h>
#include <locale.h>

// MACRO DE TRADUCCIÓN
// Tus compañeros usarán esto: DrawText(_("GAME OVER"), ...)
#define _(STRING) LocalizationManager::getInstance()->getString(STRING)

class LocalizationManager {
public:
    static LocalizationManager* getInstance();
    
    // Inicializa el sistema de gettext
    void init();
    
    // Cambia el idioma (ej: "es", "en")
    void setLanguage(const std::string& langCode);
    
    // Devuelve el texto traducido
    const char* getString(const char* msgid);
    
    std::string getCurrentLanguage() const { return currentLang; }

private:
    LocalizationManager();
    static LocalizationManager* instance;
    std::string currentLang;
};