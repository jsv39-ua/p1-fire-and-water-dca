#include "LocalizationManager.hpp"
#include <cstdlib>
#include <iostream>
#include <filesystem> // <--- IMPORTANTE: Necesario para el chivato

// Compatibilidad con Windows para variables de entorno
#if defined(_WIN32)
    #include <windows.h>
    #define setenv(k, v, o) _putenv_s(k, v)
#endif

LocalizationManager* LocalizationManager::instance = nullptr;

LocalizationManager* LocalizationManager::getInstance() {
    if (instance == nullptr) {
        instance = new LocalizationManager();
    }
    return instance;
}

LocalizationManager::LocalizationManager() : currentLang("en") {}

void LocalizationManager::init() {
    // Inicializar locale del sistema
    setlocale(LC_ALL, ""); 

    // --- CÓDIGO DE DEPURACIÓN (EL CHIVATO) ---
    std::filesystem::path cwd = std::filesystem::current_path();
    std::filesystem::path localePath = cwd / "assets" / "locales";
    
    std::cout << "\n[LOCALIZATION DEBUG] ----------------------------------------" << std::endl;
    std::cout << "[LOCALIZATION DEBUG] Directorio de trabajo (CWD): " << cwd.string() << std::endl;
    std::cout << "[LOCALIZATION DEBUG] Buscando idiomas en: " << localePath.string() << std::endl;
    
    if (std::filesystem::exists(localePath)) {
        std::cout << "[LOCALIZATION DEBUG] ¡La carpeta EXISTE! Todo debería ir bien." << std::endl;
    } else {
        std::cerr << "[LOCALIZATION ERROR] ¡La carpeta NO EXISTE! Gettext no encontrará nada." << std::endl;
        std::cerr << "[LOCALIZATION ERROR] Solución: Copia la carpeta 'assets' junto al ejecutable." << std::endl;
    }
    std::cout << "[LOCALIZATION DEBUG] ----------------------------------------\n" << std::endl;
    // -----------------------------------------

    // Configurar la ruta de las traducciones
    // IMPORTANTE: Ajustar ruta relativa a donde esté el ejecutable
    // "assets/locales" funcionará si la carpeta assets está junto al .exe
    bindtextdomain("fire_water_game", "assets/locales");
    bind_textdomain_codeset("fire_water_game", "UTF-8");
    textdomain("fire_water_game");
    
    // Idioma por defecto (puedes cambiarlo aquí o en el main)
    setLanguage("es"); 
}

void LocalizationManager::setLanguage(const std::string& langCode) {
    currentLang = langCode;
    
    // 1. Decidir el locale exacto del sistema (Linux es quisquilloso con esto)
    std::string targetLocale;
    if (langCode == "en") {
        targetLocale = "en_US.UTF-8";
    } else if (langCode == "es") {
        targetLocale = "es_ES.UTF-8";
    } else {
        targetLocale = langCode; // Fallback
    }

    // 2. Forzar la variable LANGUAGE (Prioridad para gettext)
    // Nota: En Linux usamos setenv, en Windows _putenv_s
    #if defined(_WIN32)
        std::string envVar = "LANGUAGE=" + langCode;
        _putenv_s("LANGUAGE", langCode.c_str());
    #else
        setenv("LANGUAGE", langCode.c_str(), 1);
        setenv("LC_ALL", targetLocale.c_str(), 1); // Forzar también LC_ALL en entorno
    #endif
    
    // 3. Intentar aplicar el locale en C++
    char* result = setlocale(LC_ALL, targetLocale.c_str());
    
    // Chivato de Locale
    if (result == nullptr) {
        std::cerr << "[LOCALIZATION ERROR] Falló setlocale a '" << targetLocale << "'. Probando fallback..." << std::endl;
        // Intento desesperado: probar variantes comunes si falla
        if (langCode == "en") setlocale(LC_ALL, "en_US.utf8");
        if (langCode == "es") setlocale(LC_ALL, "es_ES.utf8");
    } else {
        std::cout << "[LOCALIZATION DEBUG] setlocale ÉXITO. Locale activo: " << result << std::endl;
    }
    
    // 4. Refrescar el dominio de texto (¡CRÍTICO!)
    textdomain("fire_water_game");
    
    std::cout << "[Localization] Idioma solicitado: " << langCode << " -> Locale aplicado: " << targetLocale << std::endl;
}

const char* LocalizationManager::getString(const char* msgid) {
    return gettext(msgid);
}