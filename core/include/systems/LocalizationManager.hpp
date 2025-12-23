#pragma once

#include <string>
#include <vector>
#include <locale.h>

// Gettext (libintl) es opcional en Windows.
// Si no está disponible, usamos stubs: gettext(msgid) devuelve msgid.
#if defined(HAVE_GETTEXT) && (HAVE_GETTEXT == 1)
    #include <libintl.h>
#else
    static inline const char* gettext(const char* msgid) { return msgid; }
    static inline const char* textdomain(const char*) { return ""; }
    static inline const char* bindtextdomain(const char*, const char*) { return ""; }
    static inline const char* bind_textdomain_codeset(const char*, const char*) { return ""; }
#endif

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